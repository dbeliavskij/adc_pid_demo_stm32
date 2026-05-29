#include "cli_print.h"

#include "embedded_cli.h"

#include <stdio.h>

static EmbeddedCli *cli_instance;
static bool debug_enabled = true;

static const char *level_prefix(CliPrintLevel level)
{
    switch (level) {
    case CLI_PRINT_LEVEL_INFO:
        return "[INFO] ";
    case CLI_PRINT_LEVEL_WARN:
        return "[WARN] ";
    case CLI_PRINT_LEVEL_ERROR:
        return "[ERROR] ";
    case CLI_PRINT_LEVEL_DEBUG:
        return "[DEBUG] ";
    default:
        return "";
    }
}

void cli_print_init(EmbeddedCli *cli)
{
    cli_instance = cli;
}

static void cli_print_formatted(const char *message)
{
    if ((cli_instance == NULL) || (message == NULL)) {
        return;
    }

    embeddedCliPrint(cli_instance, message);
}

static void cli_print_vformat(CliPrintLevel level, const char *format, va_list args)
{
    char buffer[CLI_PRINT_BUFFER_SIZE];
    const char *prefix = level_prefix(level);
    int prefix_len = snprintf(buffer, sizeof(buffer), "%s", prefix);

    if (prefix_len < 0) {
        return;
    }

    if ((size_t)prefix_len >= sizeof(buffer)) {
        cli_print_formatted("[ERROR] Log message truncated");
        return;
    }

    vsnprintf(buffer + prefix_len, sizeof(buffer) - (size_t)prefix_len, format, args);
    cli_print_formatted(buffer);
}

void cli_print(const char *message)
{
    cli_print_formatted(message);
}

void cli_printf(const char *format, ...)
{
    char buffer[CLI_PRINT_BUFFER_SIZE];
    va_list args;

    if (format == NULL) {
        return;
    }

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    cli_print_formatted(buffer);
}

void cli_log(CliPrintLevel level, const char *format, ...)
{
    va_list args;

    if ((level == CLI_PRINT_LEVEL_DEBUG) && !debug_enabled) {
        return;
    }

    if (format == NULL) {
        return;
    }

    va_start(args, format);
    cli_print_vformat(level, format, args);
    va_end(args);
}

void cli_log_info(const char *format, ...)
{
    va_list args;

    if (format == NULL) {
        return;
    }

    va_start(args, format);
    cli_print_vformat(CLI_PRINT_LEVEL_INFO, format, args);
    va_end(args);
}

void cli_log_warn(const char *format, ...)
{
    va_list args;

    if (format == NULL) {
        return;
    }

    va_start(args, format);
    cli_print_vformat(CLI_PRINT_LEVEL_WARN, format, args);
    va_end(args);
}

void cli_log_error(const char *format, ...)
{
    va_list args;

    if (format == NULL) {
        return;
    }

    va_start(args, format);
    cli_print_vformat(CLI_PRINT_LEVEL_ERROR, format, args);
    va_end(args);
}

void cli_log_debug(const char *format, ...)
{
    va_list args;

    if (!debug_enabled || (format == NULL)) {
        return;
    }

    va_start(args, format);
    cli_print_vformat(CLI_PRINT_LEVEL_DEBUG, format, args);
    va_end(args);
}

void cli_log_set_debug_enabled(bool enabled)
{
    debug_enabled = enabled;
}
