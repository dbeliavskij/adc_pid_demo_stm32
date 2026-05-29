#ifndef CLI_PRINT_H
#define CLI_PRINT_H

#include <stdarg.h>
#include <stdbool.h>

#define CLI_PRINT_BUFFER_SIZE 160U

typedef struct EmbeddedCli EmbeddedCli;

typedef enum {
    CLI_PRINT_LEVEL_INFO,
    CLI_PRINT_LEVEL_WARN,
    CLI_PRINT_LEVEL_ERROR,
    CLI_PRINT_LEVEL_DEBUG,
} CliPrintLevel;

void cli_print_init(EmbeddedCli *cli);

void cli_print(const char *message);
void cli_printf(const char *format, ...);
void cli_log(CliPrintLevel level, const char *format, ...);
void cli_log_info(const char *format, ...);
void cli_log_warn(const char *format, ...);
void cli_log_error(const char *format, ...);
void cli_log_debug(const char *format, ...);

void cli_log_set_debug_enabled(bool enabled);

#define CLI_LOG_ASSERT(condition)                                              \
    do {                                                                       \
        if (!(condition)) {                                                    \
            cli_log_error("[%s:%d] Assert failed: %s", __FILE__, __LINE__,     \
                          #condition);                                         \
        }                                                                      \
    } while (0)

#define CLI_LOG_ASSERT_MSG(condition, format, ...)                             \
    do {                                                                       \
        if (!(condition)) {                                                    \
            cli_log_error("[%s:%d] Assert failed: %s - " format, __FILE__,    \
                          __LINE__, #condition, ##__VA_ARGS__);                \
        }                                                                      \
    } while (0)

#endif
