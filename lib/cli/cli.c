#include "cli.h"

#include "cli_print.h"
#include "embedded_cli.h"
#include "uart1.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CLI_BUFFER_SIZE 1024U
#define CLI_RX_BATCH_LIMIT 100U

static CLI_UINT cli_buffer[BYTES_TO_CLI_UINTS(CLI_BUFFER_SIZE)];
static EmbeddedCli *cli;

static void cli_write_char(EmbeddedCli *embedded_cli, char c) {
  (void)embedded_cli;
  uart1_write(&c, 1U);
}

static void cli_register_commands(const CliAppCommand *commands,
                                  size_t command_count) {
  if (commands == NULL) {
    return;
  }

  for (size_t i = 0U; i < command_count; ++i) {
    const CliAppCommand *command = &commands[i];

    CLI_LOG_ASSERT_MSG(command->name != NULL,
                       "Command name is NULL at index %u", (unsigned)i);
    CLI_LOG_ASSERT_MSG(command->handler != NULL,
                       "Command handler is NULL for '%s'", command->name);

    CLI_LOG_ASSERT_MSG(embeddedCliAddBinding(cli,
                                             (CliCommandBinding){
                                                 command->name,
                                                 command->help,
                                                 command->tokenize_args,
                                                 command->context,
                                                 command->handler,
                                             }),
                       "Failed to add command '%s'", command->name);
  }
}

void cli_init(const CliAppCommand *commands, size_t command_count) {
  EmbeddedCliConfig *config = embeddedCliDefaultConfig();
  config->maxBindingCount = command_count;

  config->cliBuffer = cli_buffer;
  config->cliBufferSize = CLI_BUFFER_SIZE;
  config->invitation = "> ";

  cli = embeddedCliNew(config);
  if (cli == NULL) {
    uart1_puts("CLI init failed\r\n");
    while (1) {
    }
  }

  cli->writeChar = cli_write_char;
  cli_print_init(cli);
  cli_register_commands(commands, command_count);
  embeddedCliProcess(cli);
}

bool cli_parse_uint8_arg(const char *arg, uint8_t *value) {
  char *end = NULL;
  unsigned long parsed;

  if ((arg == NULL) || (value == NULL)) {
    return false;
  }

  errno = 0;
  parsed = strtoul(arg, &end, 10);

  if ((errno != 0) || (end == arg) || (*end != '\0') || (parsed > UINT8_MAX)) {
    return false;
  }

  *value = (uint8_t)parsed;
  return true;
}

bool cli_parse_float_arg(const char *arg, float *value) {
  char *end = NULL;

  if ((arg == NULL) || (value == NULL)) {
    return false;
  }

  errno = 0;
  *value = strtof(arg, &end);

  if ((errno != 0) || (end == arg) || (*end != '\0')) {
    return false;
  }

  return true;
}

bool cli_parse_on_off_arg(const char *arg, bool *value) {
  if ((arg == NULL) || (value == NULL)) {
    return false;
  }

  if (strcmp(arg, "on") == 0) {
    *value = true;
    return true;
  }

  if (strcmp(arg, "off") == 0) {
    *value = false;
    return true;
  }

  return false;
}

void cli_process(void) {
  uint8_t loop_failsafe = CLI_RX_BATCH_LIMIT;

  while (uart1_rx_ready() && (loop_failsafe--)) {
    embeddedCliReceiveChar(cli, (char)uart1_read());
  }

  embeddedCliProcess(cli);
}
