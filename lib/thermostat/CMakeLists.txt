add_library(thermostat STATIC
    thermostat.c
)

target_include_directories(thermostat
    PUBLIC
        .
)

target_link_libraries(thermostat
    PUBLIC
        pid
    PRIVATE
        adc
        dout
        tick
        cli_print
)
