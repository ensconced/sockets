#include "./config.h"
#include "./subcommands/help/help.h"
#include "./subcommands/subcommands.h"
#include "./tcp_stack/tcp_stack.h"
#include <stdio.h>
#include <string.h>

void quit_with_error(void) {
  printf("%s", GENERAL_HELP_TEXT);
  exit(1);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    quit_with_error();
  }

  // Skip over binary name
  argc--;
  argv++;

  char *subcommand = argv[0];

  for (int i = 0; i < SUBCOMMAND_COUNT; i++) {
    if (!strcmp(subcommand, subcommands[i].name)) {
      return subcommands[i].handler(argc, argv);
    }
  }

  quit_with_error();
}
