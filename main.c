#include "cli.h"
#include "cli_print.h"
#include "uart1.h"

static void on_hello(EmbeddedCli *cli, char *args, void *context) {
  (void)cli;
  (void)args;
  (void)context;
  cli_log_info("Hello from STM32L552");
}

static const CliAppCommand app_commands[] = {
    {
        .name = "hello",
        .help = "Print greeting",
        .tokenize_args = false,
        .context = NULL,
        .handler = on_hello,
    },
};

int main(void) {
  uart1_init();
  cli_init(app_commands, sizeof(app_commands) / sizeof(app_commands[0]));

  cli_log_info("Launching adc_pid_demo_stm32");

  while (1) {
    cli_process();
  }
}
