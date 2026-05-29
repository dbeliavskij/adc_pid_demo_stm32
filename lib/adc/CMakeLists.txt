add_library(adc STATIC
    adc1.c
)

target_include_directories(adc
    PUBLIC
        .
)

target_link_libraries(adc
    PRIVATE
        bsp
)
