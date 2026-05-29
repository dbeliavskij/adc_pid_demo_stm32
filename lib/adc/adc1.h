#ifndef ADC1_H
#define ADC1_H

#include <stdint.h>

void adc1_init(void);
uint16_t adc1_read_raw(void);
float adc1_read_voltage(void);
float adc1_read_celsius(void);

#endif
