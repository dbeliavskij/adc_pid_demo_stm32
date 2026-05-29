add_library(dout STATIC
    dout1.c
)

target_include_directories(dout
    PUBLIC
        .
)

target_link_libraries(dout
    PRIVATE
        bsp
)
