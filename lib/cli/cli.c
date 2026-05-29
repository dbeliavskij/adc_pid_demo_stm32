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

static void on_hello(EmbeddedCli *embedded_cli, char *args, void *context) {
  (void)embedded_cli;
  (void)args;
  (void)context;
  cli_log_info("Hello from STM32L552");
}

void cli_init(void) {
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

  CLI_LOG_ASSERT_MSG(
      embeddedCliAddBinding(cli,
                            (CliCommandBinding){
                                "get_set_temp",
                                "Returns currently set temperature",
                                false,
                                NULL,
                                on_hello,
                            }),
      "Failed to add get_set_temp binding");

  embeddedCliProcess(cli);
}

void cli_process(void) {
  uint8_t loop_failsafe = CLI_RX_BATCH_LIMIT;
  while (uart1_rx_ready() && (loop_failsafe--)) {
    embeddedCliReceiveChar(cli, (char)uart1_read());
  }

  embeddedCliProcess(cli);
}
