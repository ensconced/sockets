#include "./subcommands.h"
#include "./help/help.h"
#include "./open/open.h"
#include "./start/start.h"
#include "./stop/stop.h"

subcommand subcommands[SUBCOMMAND_COUNT] = {
    {
        .name = "start",
        .handler = handle_start_subcommand,
        .help_text = START_HELP_TEXT,
    },
    {
        .name = "stop",
        .handler = handle_stop_subcommand,
        .help_text = STOP_HELP_TEXT,
    },
    {
        .name = "help",
        .handler = handle_help_subcommand,
        .help_text = HELP_HELP_TEXT,
    },
    {
        .name = "open",
        .handler = handle_open_command,
        .help_text = OPEN_HELP_TEXT,
    },
};
