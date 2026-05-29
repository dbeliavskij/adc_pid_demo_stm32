cmake_minimum_required(VERSION 3.22)

project(adc_pid_demo_stm32 C ASM)

add_subdirectory(bsp)
add_subdirectory(lib)

add_executable(adc_pid_demo_stm32
    main.c
)

target_link_libraries(adc_pid_demo_stm32 PRIVATE
    cli
    uart
    cli_print
)

target_link_options(adc_pid_demo_stm32 PRIVATE
    -T "${BSP_LINKER_SCRIPT}"
    -Wl,-Map=$<TARGET_FILE_DIR:adc_pid_demo_stm32>
    -Wl,-u,_printf_float
)