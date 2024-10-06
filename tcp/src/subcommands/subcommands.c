#include "./subcommands.h"

subcommand subcommands[SUBCOMMAND_COUNT] = {
    (subcommand){
        .name = "start",
        .handler = handle_start_subcommand,
        .help_text = START_HELP_TEXT,
    },
    (subcommand){
        .name = "stop",
        .handler = handle_stop_subcommand,
        .help_text = STOP_HELP_TEXT,
    },
    (subcommand){
        .name = "help",
        .handler = handle_help_subcommand,
        .help_text = HELP_HELP_TEXT,
    },
};
