#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "PID.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  PIDController pid;
  uint32_t last_tick;
  float current_temperature_c;
  uint8_t set_temperature_c;
  bool enabled;
} Thermostat;

void thermostat_init(Thermostat *thermo);
void thermostat_process(Thermostat *thermo);

#endif