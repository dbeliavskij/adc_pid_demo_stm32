add_library(uart STATIC
    uart1.c
)

target_include_directories(uart
    PUBLIC
        .
)

target_link_libraries(uart
    PRIVATE
        bsp
)