#include "./help.h"
#include "../subcommands.h"
#include <stdio.h>
#include <string.h>

const char GENERAL_HELP_TEXT[] = "Usage:\n"
                                 "  sockets <subcommand>\n"
                                 "\n"
                                 "Available subcommands:\n"
                                 "\n"
                                 "  start     Start the tcp stack\n"
                                 "  stop      Stop the tcp stack\n"
                                 "  help      Show this help output\n"
                                 "  open      Create a new TCP connection\n"
                                 "\n"
                                 "See 'sockets help <subcommand> to read about a specific subcommand.";

const char START_HELP_TEXT[] = "USAGE\n"
                               "  sockets start [options]\n"
                               "\n"
                               "OPTIONS\n"
                               "  --detach\n"
                               "    Run tcp stack in the background";

const char STOP_HELP_TEXT[] = "Usage:\n"
                              "  sockets stop";

const char OPEN_HELP_TEXT[] = "Usage:\n"
                              "  socket open --local=<local addr>:<local port> [--remote=<local addr>:<local port>]\n"
                              "\n"
                              "This opens a TCP connection.\n"
                              "If the --remote option is included, the connection is opened in \"active\" mode. "
                              "Otherwise, the connection is opened in \"passive \" mode.";

const char HELP_HELP_TEXT[] = "Usage:\n"
                              "  sockets help";

int handle_help_subcommand(int argc, char **argv) {
  // Skip over "help" argument
  argc--;
  argv++;

  if (argc > 0) {
    char *command_to_help_with = argv[0];
    for (int i = 0; i < SUBCOMMAND_COUNT; i++) {
      if (!strcmp(command_to_help_with, subcommands[i].name)) {
        printf("%s\n", subcommands[i].help_text);
        return 0;
      }
    }
  }
  printf("%s\n", GENERAL_HELP_TEXT);
  return 0;
}
