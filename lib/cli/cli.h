#ifndef CLI_H
#define CLI_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "embedded_cli.h"

typedef struct EmbeddedCli EmbeddedCli;

typedef void (*CliCommandHandler)(EmbeddedCli *cli, char *args, void *context);

typedef struct {
    const char *name;
    const char *help;
    bool tokenize_args;
    void *context;
    CliCommandHandler handler;
} CliAppCommand;

void cli_init(const CliAppCommand *commands, size_t command_count);
void cli_process(void);
bool cli_parse_uint8_arg(const char *arg, uint8_t *value);
bool cli_parse_float_arg(const char *arg, float *value);
bool cli_parse_on_off_arg(const char *arg, bool *value);

#endif
