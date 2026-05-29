# ADC PID Demo (STM32L552)

Firmware demo for a UART-controlled thermostat: read temperature from ADC, run a PID loop, and drive a heater output. Developed and validated in **Renode** simulation.

## Important: hardware status

**This project has not been tested on real hardware.**

Peripheral register configuration (ADC, GPIO, USART, RCC, SysTick) is based on the STM32L5 reference manual and ST LL driver usage patterns, but it is **only theoretically correct** and **not verified** on a physical board. Clock setup, pin muxing, ADC sampling times, and timing may need changes before deploying to silicon.

All functional testing described below was done in Renode.

## Features

- **ADC1** on PA0 (channel 5): temperature sensor input, 0–50°C
- **Heater output** on PA1 (active-low digital output)
- **USART1** CLI on PA9/PA10 at 115200 baud
- **PID thermostat** using [pms67/PID](https://github.com/pms67/PID) (git submodule)
- **Renode** simulation with a simple thermal plant tied to the heater GPIO

## Hardware mapping (target)

| Signal        | Pin  | Peripheral        | Notes                          |
|---------------|------|-------------------|--------------------------------|
| Temperature   | PA0  | ADC1 IN5          | 0–3.3 V at pin after divider   |
| Heater DOUT   | PA1  | GPIO output       | Low = heater ON                |
| UART TX       | PA9  | USART1            |                                |
| UART RX       | PA10 | USART1            |                                |

### Temperature scaling

The homework sensor is specified as **0–5 V → 0–50°C**. The STM32 ADC reference is **3.3 V**, so the design assumes a **voltage divider** (or equivalent conditioning) so that 0–50°C appears as **0–3.3 V at PA0**. Firmware and Renode both model the voltage **at the ADC pin**, not the raw 5 V sensor output.

## Project layout

```
adc_pid_demo_stm32/
├── main.c                 Application entry, CLI commands
├── bsp/                   CMSIS + STM32L5 LL driver (submodules)
├── lib/
│   ├── adc/               ADC1 driver
│   ├── dout/              Heater GPIO
│   ├── uart/              USART1 console
│   ├── tick/              1 ms SysTick timebase
│   ├── thermostat/        PID control loop
│   ├── cli/               Command-line interface
│   └── third_party/
│       ├── embedded-cli/  CLI library (submodule)
│       └── pid/           pms67 PID (submodule)
└── renode/
    ├── main_simulation.resc
    └── adc1.repl            ADC stub + thermal model
```

## Prerequisites

- **CMake** 3.22+
- **Ninja**
- **arm-none-eabi-gcc** (GNU Arm Embedded Toolchain)
- **Renode** 1.16+ (for simulation)
- **Git** with submodule support

## Build

Clone with submodules:

```bash
git clone --recurse-submodules <repo-url>
cd adc_pid_demo_stm32
```

If already cloned:

```bash
git submodule update --init --recursive
```

Configure and build:

```bash
cmake --preset default
cmake --build build --config Debug
```

Firmware output: `build/Debug/adc_pid_demo_stm32`

## Renode simulation

From the project root:

```bash
cmake --build build --config Debug
renode renode/main_simulation.resc
```

A UART analyzer window opens for **USART1**. Use it as the CLI console.

The overlay `renode/adc1.repl` adds an ADC1 model and thermal plant:

- Heater ON (PA1 low): temperature rises (~2°C/s)
- Heater OFF (PA1 high): temperature falls (~0.5°C/s)
- Range clamped to 0–50°C

## CLI commands

| Command | Description |
|---------|-------------|
| `set_temp <0-50>` | Set target temperature (°C) |
| `set_thermostat on\|off` | Enable or disable PID control |
| `get_temp` | Read current temperature |
| `get_set_temp` | Read configured setpoint |
| `get_status` | Thermostat on/off |
| `set_kp <value>` | Set PID Kp |
| `set_ki <value>` | Set PID Ki |
| `set_kd <value>` | Set PID Kd |
| `get_kp` / `get_ki` / `get_kd` | Read PID gains |

## Example session

```
set_thermostat on
set_temp 30
get_temp
get_status
```

Default PID gains at startup: **Kp = 1.0**, **Ki = 0.1**, **Kd = 0.01**. The control loop runs every **100 ms**. Heater is driven **on/off** from the PID output sign, so overshoot on large setpoint steps is expected until gains are tuned.

## PID library

PID implementation: [pms67/PID](https://github.com/pms67/PID) (MIT), included as `lib/third_party/pid`. Features derivative-on-measurement, filtered derivative, trapezoidal integration, and output/integrator limiting.

## License

Application code: see repository license if present. Third-party components retain their own licenses (ST HAL/CMSIS, embedded-cli, pms67 PID).
