#ifndef CLI_H
#define CLI_H

#include <stddef.h>
#include <stdbool.h>

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

#endif
