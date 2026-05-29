#include "adc1.h"
#include "cli.h"
#include "cli_print.h"
#include "dout1.h"
#include "uart1.h"

#include <stdbool.h>
#include <stdint.h>

#define TEMPERATURE_MIN_C 0U
#define TEMPERATURE_MAX_C 50U

typedef struct {
  uint8_t set_temperature_c;
  bool enabled;
  float current_temperature_c;
  float kp;
  float ki;
  float kd;
} ThermostatState;

static ThermostatState thermostat = {
    .set_temperature_c = 21U,
    .enabled = false,
    .current_temperature_c = 20.0f,
    .kp = 1.0f,
    .ki = 0.1f,
    .kd = 0.01f,
};

static void on_set_temp(EmbeddedCli *cli, char *args, void *context) {
  uint8_t *stored_temperature = context;
  uint8_t temperature_c;

  (void)cli;

  if (embeddedCliGetTokenCount(args) < 1U) {
    cli_log_error("Usage: set_temp <0-50>");
    return;
  }

  if (!cli_parse_uint8_arg(embeddedCliGetToken(args, 1U), &temperature_c)) {
    cli_log_error("Invalid temperature value");
    return;
  }

  if (temperature_c > TEMPERATURE_MAX_C) {
    cli_log_error("Temperature must be between %u and %u C", TEMPERATURE_MIN_C,
                  TEMPERATURE_MAX_C);
    return;
  }

  if (stored_temperature != NULL) {
    *stored_temperature = temperature_c;
  }

  cli_log_info("Set temperature to %u C", temperature_c);
}

static void on_set_thermostat(EmbeddedCli *cli, char *args, void *context) {
  bool *enabled = context;
  bool requested_status;

  (void)cli;

  if (embeddedCliGetTokenCount(args) < 1U) {
    cli_log_error("Usage: set_thermostat <on|off>");
    return;
  }

  if (!cli_parse_on_off_arg(embeddedCliGetToken(args, 1U), &requested_status)) {
    cli_log_error("Invalid thermostat status, use on or off");
    return;
  }

  if (enabled != NULL) {
    *enabled = requested_status;
  }

  cli_log_info("Thermostat set to %s", requested_status ? "on" : "off");
}

static void on_set_kp(EmbeddedCli *cli, char *args, void *context) {
  float *kp = context;
  float value;

  (void)cli;

  if (embeddedCliGetTokenCount(args) < 1U) {
    cli_log_error("Usage: set_kp <value>");
    return;
  }

  if (!cli_parse_float_arg(embeddedCliGetToken(args, 1U), &value)) {
    cli_log_error("Invalid Kp value");
    return;
  }

  if (kp != NULL) {
    *kp = value;
  }

  cli_log_info("Kp set to %.2f", value);
}

static void on_set_ki(EmbeddedCli *cli, char *args, void *context) {
  float *ki = context;
  float value;

  (void)cli;

  if (embeddedCliGetTokenCount(args) < 1U) {
    cli_log_error("Usage: set_ki <value>");
    return;
  }

  if (!cli_parse_float_arg(embeddedCliGetToken(args, 1U), &value)) {
    cli_log_error("Invalid Ki value");
    return;
  }

  if (ki != NULL) {
    *ki = value;
  }

  cli_log_info("Ki set to %.2f", value);
}

static void on_set_kd(EmbeddedCli *cli, char *args, void *context) {
  float *kd = context;
  float value;

  (void)cli;

  if (embeddedCliGetTokenCount(args) < 1U) {
    cli_log_error("Usage: set_kd <value>");
    return;
  }

  if (!cli_parse_float_arg(embeddedCliGetToken(args, 1U), &value)) {
    cli_log_error("Invalid Kd value");
    return;
  }

  if (kd != NULL) {
    *kd = value;
  }

  cli_log_info("Kd set to %.2f", value);
}

static void on_get_temp(EmbeddedCli *cli, char *args, void *context) {
  float *current_temperature = context;
  float temperature_c = adc1_read_celsius();

  (void)cli;
  (void)args;

  if (current_temperature != NULL) {
    *current_temperature = temperature_c;
  }

  cli_log_info("Current temperature=%.2f C", temperature_c);
}

static void on_get_status(EmbeddedCli *cli, char *args, void *context) {
  bool *enabled = context;

  (void)cli;
  (void)args;

  if (enabled == NULL) {
    cli_log_error("Thermostat status is unavailable");
    return;
  }

  cli_log_info("Thermostat is %s", *enabled ? "on" : "off");
}

static void on_get_set_temp(EmbeddedCli *cli, char *args, void *context) {
  uint8_t *set_temperature = context;

  (void)cli;
  (void)args;

  if (set_temperature == NULL) {
    cli_log_error("Set temperature is unavailable");
    return;
  }

  cli_log_info("Set temperature is %u C", *set_temperature);
}

static void on_get_kp(EmbeddedCli *cli, char *args, void *context) {
  float *kp = context;

  (void)cli;
  (void)args;

  if (kp == NULL) {
    cli_log_error("Kp is unavailable");
    return;
  }

  cli_log_info("Kp=%.2f", *kp);
}

static void on_get_ki(EmbeddedCli *cli, char *args, void *context) {
  float *ki = context;

  (void)cli;
  (void)args;

  if (ki == NULL) {
    cli_log_error("Ki is unavailable");
    return;
  }

  cli_log_info("Ki=%.2f", *ki);
}

static void on_get_kd(EmbeddedCli *cli, char *args, void *context) {
  float *kd = context;

  (void)cli;
  (void)args;

  if (kd == NULL) {
    cli_log_error("Kd is unavailable");
    return;
  }

  cli_log_info("Kd=%.2f", *kd);
}

static void on_set_heater(EmbeddedCli *cli, char *args, void *context) {
  bool requested_status;

  (void)cli;
  (void)context;

  if (embeddedCliGetTokenCount(args) < 1U) {
    cli_log_error("Usage: set_heater <on|off>");
    return;
  }

  if (!cli_parse_on_off_arg(embeddedCliGetToken(args, 1U), &requested_status)) {
    cli_log_error("Invalid heater status, use on or off");
    return;
  }

  dout1_set_heater(requested_status);
  cli_log_info("Heater set to %s", requested_status ? "on" : "off");
}

static void on_get_heater(EmbeddedCli *cli, char *args, void *context) {
  (void)cli;
  (void)args;
  (void)context;

  cli_log_info("Heater is %s", dout1_is_heater_on() ? "on" : "off");
}

static const CliAppCommand app_commands[] = {
    {
        .name = "set_temp",
        .help = "Set target temperature (T set): set_temp <0-50>",
        .tokenize_args = true,
        .context = &thermostat.set_temperature_c,
        .handler = on_set_temp,
    },
    {
        .name = "set_thermostat",
        .help = "Enable or disable thermostat: set_thermostat <on|off>",
        .tokenize_args = true,
        .context = &thermostat.enabled,
        .handler = on_set_thermostat,
    },
    {
        .name = "set_kp",
        .help = "Set PID Kp coefficient: set_kp <value>",
        .tokenize_args = true,
        .context = &thermostat.kp,
        .handler = on_set_kp,
    },
    {
        .name = "set_ki",
        .help = "Set PID Ki coefficient: set_ki <value>",
        .tokenize_args = true,
        .context = &thermostat.ki,
        .handler = on_set_ki,
    },
    {
        .name = "set_kd",
        .help = "Set PID Kd coefficient: set_kd <value>",
        .tokenize_args = true,
        .context = &thermostat.kd,
        .handler = on_set_kd,
    },
    {
        .name = "get_temp",
        .help = "Get current temperature (T)",
        .tokenize_args = false,
        .context = &thermostat.current_temperature_c,
        .handler = on_get_temp,
    },
    {
        .name = "get_status",
        .help = "Get thermostat status (Tstat)",
        .tokenize_args = false,
        .context = &thermostat.enabled,
        .handler = on_get_status,
    },
    {
        .name = "get_set_temp",
        .help = "Get configured target temperature (T set)",
        .tokenize_args = false,
        .context = &thermostat.set_temperature_c,
        .handler = on_get_set_temp,
    },
    {
        .name = "get_kp",
        .help = "Get PID Kp coefficient",
        .tokenize_args = false,
        .context = &thermostat.kp,
        .handler = on_get_kp,
    },
    {
        .name = "get_ki",
        .help = "Get PID Ki coefficient",
        .tokenize_args = false,
        .context = &thermostat.ki,
        .handler = on_get_ki,
    },
    {
        .name = "get_kd",
        .help = "Get PID Kd coefficient",
        .tokenize_args = false,
        .context = &thermostat.kd,
        .handler = on_get_kd,
    },
    {
        .name = "set_heater",
        .help = "Turn heater on or off: set_heater <on|off>",
        .tokenize_args = true,
        .context = NULL,
        .handler = on_set_heater,
    },
    {
        .name = "get_heater",
        .help = "Get heater output state",
        .tokenize_args = false,
        .context = NULL,
        .handler = on_get_heater,
    },
};

int main(void) {
  uart1_init();
  adc1_init();
  dout1_init();
  tick1_init();
  cli_init(app_commands, sizeof(app_commands) / sizeof(app_commands[0]));

  cli_log_info("Launching adc_pid_demo_stm32");

  while (1) {
    cli_process();
  }
}
