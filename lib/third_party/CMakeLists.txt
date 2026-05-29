add_library(embedded_cli STATIC
    embedded-cli/lib/src/embedded_cli.c
)

target_include_directories(embedded_cli PUBLIC
    embedded-cli/lib/include
)