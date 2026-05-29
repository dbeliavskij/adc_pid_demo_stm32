#include "cli.h"

#include "cli_print.h"
#include "embedded_cli.h"
#include "uart1.h"

#include <stdbool.h>
#include <stdint.h>

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

void cli_process(void) {
  uint8_t loop_failsafe = CLI_RX_BATCH_LIMIT;

  while (uart1_rx_ready() && (loop_failsafe--)) {
    embeddedCliReceiveChar(cli, (char)uart1_read());
  }

  embeddedCliProcess(cli);
}
