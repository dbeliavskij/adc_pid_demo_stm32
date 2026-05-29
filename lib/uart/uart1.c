#include "uart1.h"

#include "stm32l5xx.h"
#include "stm32l5xx_ll_bus.h"
#include "stm32l5xx_ll_gpio.h"
#include "stm32l5xx_ll_rcc.h"
#include "stm32l5xx_ll_usart.h"

#define UART1_BAUDRATE 115200U

void uart1_init(void)
{
    LL_GPIO_InitTypeDef gpio_init = {0};
    LL_USART_InitTypeDef usart_init = {0};

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);

    gpio_init.Pin = LL_GPIO_PIN_9 | LL_GPIO_PIN_10;
    gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init.Pull = LL_GPIO_PULL_NO;
    gpio_init.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOA, &gpio_init);

    LL_USART_StructInit(&usart_init);
    usart_init.BaudRate = UART1_BAUDRATE;
    usart_init.DataWidth = LL_USART_DATAWIDTH_8B;
    usart_init.StopBits = LL_USART_STOPBITS_1;
    usart_init.Parity = LL_USART_PARITY_NONE;
    usart_init.TransferDirection = LL_USART_DIRECTION_TX_RX;
    usart_init.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    usart_init.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART1, &usart_init);

    LL_USART_Enable(USART1);
    while (LL_USART_IsEnabled(USART1) == 0U) {
    }
}

void uart1_write(const char *data, size_t length)
{
    for (size_t i = 0U; i < length; ++i) {
        while (LL_USART_IsActiveFlag_TXE(USART1) == 0U) {
        }

        LL_USART_TransmitData8(USART1, (uint8_t)data[i]);
    }

    while (LL_USART_IsActiveFlag_TC(USART1) == 0U) {
    }
}

void uart1_puts(const char *str)
{
    const char *cursor = str;

    while (*cursor != '\0') {
        ++cursor;
    }

    uart1_write(str, (size_t)(cursor - str));
}

bool uart1_rx_ready(void)
{
    return LL_USART_IsActiveFlag_RXNE(USART1) != 0U;
}

int uart1_read(void)
{
    return (int)LL_USART_ReceiveData8(USART1);
}
