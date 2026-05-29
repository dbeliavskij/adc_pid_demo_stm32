#include "cli.h"

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
  (void)args;
  (void)context;
  embeddedCliPrint(embedded_cli, "Hello from STM32L552\r\n");
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

  embeddedCliAddBinding(cli, (CliCommandBinding){
                                 "get set temp",
                                 "Returns currently set temperature",
                                 false,
                                 NULL,
                                 on_hello,
                             });

  embeddedCliProcess(cli);
}

void cli_process(void) {
  uint8_t loop_failsafe = CLI_RX_BATCH_LIMIT;
  while (uart1_rx_ready() && (loop_failsafe--)) {
    embeddedCliReceiveChar(cli, (char)uart1_read());
  }

  embeddedCliProcess(cli);
}
