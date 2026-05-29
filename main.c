#include "cli.h"
#include "cli_print.h"
#include "uart1.h"

int main(void) {
  uart1_init();
  cli_init();

  cli_log_info("Launching adc_pid_demo_stm32");

  while (1) {
    cli_process();
  }
}
