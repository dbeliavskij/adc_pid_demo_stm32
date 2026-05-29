add_library(cli_print STATIC
    cli_print.c
)

target_include_directories(cli_print
    PUBLIC
        .
)

target_link_libraries(cli_print
    PRIVATE
        embedded_cli
)
