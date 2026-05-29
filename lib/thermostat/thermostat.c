#include "thermostat.h"

#include "adc1.h"
#include "cli_print.h"
#include "dout1.h"
#include "tick1.h"

#include <stddef.h>

#define THERMOSTAT_PERIOD_MS 100U
#define THERMOSTAT_PERIOD_S 0.1f
#define PID_TAU_S 0.2f
#define PID_OUTPUT_MIN 0.0f
#define PID_OUTPUT_MAX 1.0f
#define PID_INTEGRATOR_MIN -25.0f
#define PID_INTEGRATOR_MAX 25.0f

void thermostat_init(Thermostat *thermo) {
  if (thermo == NULL) {
    return;
  }

  *thermo = (Thermostat){
      .set_temperature_c = 21U,
      .enabled = false,
      .current_temperature_c = 20.0f,
      .last_tick = tick1_get_ms(),
      .pid =
          {
              .Kp = 1.0f,
              .Ki = 0.1f,
              .Kd = 0.01f,
              .tau = PID_TAU_S,
              .limMin = PID_OUTPUT_MIN,
              .limMax = PID_OUTPUT_MAX,
              .limMinInt = PID_INTEGRATOR_MIN,
              .limMaxInt = PID_INTEGRATOR_MAX,
              .T = THERMOSTAT_PERIOD_S,
          },
  };

  PIDController_Init(&thermo->pid);
}

void thermostat_process(Thermostat *thermo) {
  uint32_t now_ms;
  uint32_t elapsed_ms;
  bool output;

  if (thermo == NULL) {
    return;
  }

  now_ms = tick1_get_ms();
  elapsed_ms = now_ms - thermo->last_tick;

  if (elapsed_ms < THERMOSTAT_PERIOD_MS) {
    return;
  }

  thermo->last_tick = now_ms;

  thermo->current_temperature_c = adc1_read_celsius();

  if (!thermo->enabled) {
    dout1_set_heater(false);
    PIDController_Init(&thermo->pid);
    return;
  }

  output = PIDController_Update(&thermo->pid, (float)thermo->set_temperature_c,
                                thermo->current_temperature_c) > 0.0f;

  if (dout1_is_heater_on() != output) {
    dout1_set_heater(output);
    cli_log_debug("Turning heater %s", output ? "on" : "off");
  }
}