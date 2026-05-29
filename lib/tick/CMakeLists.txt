add_library(tick STATIC
    tick1.c
)

target_include_directories(tick
    PUBLIC
        .
)

target_link_libraries(tick
    PRIVATE
        bsp
)
