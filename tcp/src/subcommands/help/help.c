#include "./help.h"
#include "../subcommands.h"
#include <stdio.h>
#include <string.h>

char GENERAL_HELP_TEXT[] = "usage: sockets <subcommand>\n"
                           "\n"
                           "Available subcommands:\n"
                           "\n"
                           "start     Start the tcp stack as a daemon process\n"
                           "stop      Stop the tcp stack daemon process\n"
                           "help      Show this help output\n"
                           "\n"
                           "See 'sockets help <subcommand> to read about a specific subcommand.\n";

const char START_HELP_TEXT[] = "usage: sockets start";

const char STOP_HELP_TEXT[] = "usage: sockets stop";

const char HELP_HELP_TEXT[] = "usage: sockets help";

int handle_help_subcommand(int argc, char *const *argv) {
  if (argc > 0) {
    char *command_to_help_with = argv[0];
    for (int i = 0; i < SUBCOMMAND_COUNT; i++) {
      if (!strcmp(command_to_help_with, subcommands[i].name)) {
        printf("%s\n", subcommands[i].help_text);
        return 0;
      }
    }
  }
  printf("%s", GENERAL_HELP_TEXT);
  return 0;
}
