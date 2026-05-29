#ifndef DOUT1_H
#define DOUT1_H

#include <stdbool.h>

void dout1_init(void);
void dout1_set_heater(bool on);
bool dout1_is_heater_on(void);

#endif
