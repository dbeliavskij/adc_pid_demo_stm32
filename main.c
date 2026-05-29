#include "cli.h"
#include "uart1.h"

int main(void)
{
    uart1_init();
    cli_init();

    while (1) {
        cli_process();
    }
}
