#ifndef UART1_H
#define UART1_H

#include <stdbool.h>
#include <stddef.h>

void uart1_init(void);
void uart1_write(const char *data, size_t length);
void uart1_puts(const char *str);
bool uart1_rx_ready(void);
int uart1_read(void);

#endif
