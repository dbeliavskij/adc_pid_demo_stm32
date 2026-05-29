#include "tick1.h"

#include "stm32l5xx.h"
#include "stm32l5xx_ll_utils.h"

extern uint32_t SystemCoreClock;

volatile uint32_t tick1_milliseconds;

void SysTick_Handler(void) { tick1_milliseconds++; }

void tick1_init(void) {
  tick1_milliseconds = 0U;
  LL_Init1msTick(SystemCoreClock);
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

uint32_t tick1_get_ms(void) { return tick1_milliseconds; }
