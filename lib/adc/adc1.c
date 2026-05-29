#include "adc1.h"

#include <stdbool.h>
#include <stdint.h>
#include "stm32l5xx_ll_adc.h"
#include "stm32l5xx_ll_bus.h"
#include "stm32l5xx_ll_gpio.h"
#include "stm32l5xx_ll_rcc.h"

#define ADC1_CHANNEL LL_ADC_CHANNEL_5
#define ADC1_GPIO_PORT GPIOA
#define ADC1_GPIO_PIN LL_GPIO_PIN_0

#define ADC1_VREF_V 3.3f
#define ADC1_SENSOR_MAX_V 5.0f
#define ADC1_TEMPERATURE_MAX_C 50.0f
#define ADC1_MAX_RAW 4095U

#define ADC1_INIT_TIMEOUT 1000000U

static bool adc1_wait_flag(uint32_t (*is_set)(const ADC_TypeDef *))
{
    uint32_t timeout = ADC1_INIT_TIMEOUT;

    while (is_set(ADC1) == 0U) {
        if (--timeout == 0U) {
            return false;
        }
    }

    return true;
}

static bool adc1_wait_calibration_done(void)
{
    uint32_t timeout = ADC1_INIT_TIMEOUT;

    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0U) {
        if (--timeout == 0U) {
            return false;
        }
    }

    return true;
}

static void adc1_delay_us(uint32_t microseconds)
{
    volatile uint32_t cycles = microseconds * 4U;

    while (cycles > 0U) {
        --cycles;
    }
}

static void adc1_configure_channel(void)
{
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, ADC1_CHANNEL);
    LL_ADC_SetChannelSamplingTime(ADC1, ADC1_CHANNEL, LL_ADC_SAMPLINGTIME_247CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC1, ADC1_CHANNEL, LL_ADC_SINGLE_ENDED);
}

void adc1_init(void)
{
    LL_ADC_InitTypeDef adc_init = {0};
    LL_ADC_REG_InitTypeDef reg_init = {0};
    LL_ADC_CommonInitTypeDef common_init = {0};

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_SYSCLK);

    LL_GPIO_SetPinMode(ADC1_GPIO_PORT, ADC1_GPIO_PIN, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(ADC1_GPIO_PORT, ADC1_GPIO_PIN, LL_GPIO_PULL_NO);

    common_init.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &common_init);

    adc_init.Resolution = LL_ADC_RESOLUTION_12B;
    adc_init.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    adc_init.LowPowerMode = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(ADC1, &adc_init);

    reg_init.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    reg_init.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
    reg_init.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    reg_init.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    reg_init.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
    reg_init.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
    LL_ADC_REG_Init(ADC1, &reg_init);

    LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);
    adc1_configure_channel();

    LL_ADC_DisableDeepPowerDown(ADC1);
    LL_ADC_EnableInternalRegulator(ADC1);
    adc1_delay_us(10U);

    LL_ADC_Enable(ADC1);
    (void)adc1_wait_flag(LL_ADC_IsActiveFlag_ADRDY);

    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    (void)adc1_wait_calibration_done();
}

uint16_t adc1_read_raw(void)
{
    uint32_t timeout = ADC1_INIT_TIMEOUT;

    adc1_configure_channel();
    LL_ADC_ClearFlag_EOC(ADC1);
    LL_ADC_REG_StartConversion(ADC1);

    while (LL_ADC_IsActiveFlag_EOC(ADC1) == 0U) {
        if (--timeout == 0U) {
            return 0U;
        }
    }

    return LL_ADC_REG_ReadConversionData12(ADC1);
}

float adc1_read_voltage(void)
{
    return ((float)adc1_read_raw() * ADC1_VREF_V) / (float)ADC1_MAX_RAW;
}

float adc1_read_celsius(void)
{
    float voltage = adc1_read_voltage();

    if (voltage < 0.0f) {
        voltage = 0.0f;
    }

    if (voltage > ADC1_SENSOR_MAX_V) {
        voltage = ADC1_SENSOR_MAX_V;
    }

    return (voltage / ADC1_SENSOR_MAX_V) * ADC1_TEMPERATURE_MAX_C;
}
