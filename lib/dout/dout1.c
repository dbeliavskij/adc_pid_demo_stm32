#include "dout1.h"

#include "stm32l5xx.h"
#include "stm32l5xx_ll_bus.h"
#include "stm32l5xx_ll_gpio.h"

#define DOUT1_GPIO_PORT GPIOA
#define DOUT1_GPIO_PIN LL_GPIO_PIN_1

void dout1_init(void)
{
    LL_GPIO_InitTypeDef gpio_init = {0};

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

    gpio_init.Pin = DOUT1_GPIO_PIN;
    gpio_init.Mode = LL_GPIO_MODE_OUTPUT;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_LOW;
    gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(DOUT1_GPIO_PORT, &gpio_init);

    dout1_set_heater(false);
}

void dout1_set_heater(bool on)
{
    if (on) {
        LL_GPIO_ResetOutputPin(DOUT1_GPIO_PORT, DOUT1_GPIO_PIN);
    } else {
        LL_GPIO_SetOutputPin(DOUT1_GPIO_PORT, DOUT1_GPIO_PIN);
    }
}

bool dout1_is_heater_on(void)
{
    return LL_GPIO_IsOutputPinSet(DOUT1_GPIO_PORT, DOUT1_GPIO_PIN) == 0U;
}
