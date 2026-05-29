set(CMSIS_DEVICE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmsis-device-l5)

set(LINKER_SCRIPT_SOURCE
    ${CMSIS_DEVICE_DIR}/Source/Templates/gcc/linker/STM32L552xE_FLASH.ld
)

# GCC 10 does not support the (READONLY) section attribute used in recent ST linker scripts.
file(READ ${LINKER_SCRIPT_SOURCE} LINKER_SCRIPT_CONTENT)
string(REPLACE " (READONLY)" "" LINKER_SCRIPT_CONTENT "${LINKER_SCRIPT_CONTENT}")
set(BSP_LINKER_SCRIPT ${CMAKE_BINARY_DIR}/STM32L552xE_FLASH.ld)
file(WRITE ${BSP_LINKER_SCRIPT} "${LINKER_SCRIPT_CONTENT}")

add_library(bsp STATIC
    cmsis-device-l5/Source/Templates/system_stm32l5xx.c
    cmsis-device-l5/Source/Templates/gcc/startup_stm32l552xx.s
    stm32l5xx-hal-driver/Src/stm32l5xx_ll_adc.c
    stm32l5xx-hal-driver/Src/stm32l5xx_ll_gpio.c
    stm32l5xx-hal-driver/Src/stm32l5xx_ll_rcc.c
    stm32l5xx-hal-driver/Src/stm32l5xx_ll_usart.c
    stm32l5xx-hal-driver/Src/stm32l5xx_ll_utils.c
    stm32l5xx-hal-driver/Src/stm32l5xx_ll_pwr.c
)

target_compile_definitions(bsp PUBLIC
    STM32L552xx
    USE_FULL_LL_DRIVER
)

target_include_directories(bsp PUBLIC
    cmsis-device-l5/Include
    cmsis-core/Include
    stm32l5xx-hal-driver/Inc
)

set(BSP_LINKER_SCRIPT ${BSP_LINKER_SCRIPT} PARENT_SCOPE)
