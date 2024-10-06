#include "./config.h"
#include "./tcp_stack/tcp_stack.h"
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SUBCOMMAND_COUNT 2

char GENERAL_HELP_TEXT[] = "usage: sockets <subcommand>\n"
                           "\n"
                           "Available subcommands:\n"
                           "\n"
                           "start     Start the tcp stack as a daemon process\n"
                           "help      Show this help output\n"
                           "\n"
                           "See 'sockets help <subcommand> to read about a specific subcommand.\n";

const char START_HELP_TEXT[] = "usage: sockets start";

const char HELP_HELP_TEXT[] = "usage: sockets help";

typedef struct subcommand {
  const char *name;
  int (*handler)(int argc, char *const *argv);
  const char *help_text;
} subcommand;

int handle_start_subcommand(int argc, char *const *argv);
int handle_help_subcommand(int argc, char *const *argv);

subcommand subcommands[SUBCOMMAND_COUNT] = {
    (subcommand){
        .name = "start",
        .handler = handle_start_subcommand,
        .help_text = START_HELP_TEXT,
    },
    (subcommand){
        .name = "help",
        .handler = handle_help_subcommand,
        .help_text = HELP_HELP_TEXT,
    },
};

void quit_with_error(void) {
  printf("%s", GENERAL_HELP_TEXT);
  exit(1);
}

int handle_start_subcommand(int argc, char *const *argv) {
  pid_t fork_pid = fork();
  if (fork_pid == -1) {
    fprintf(stderr, "Failed to create daemon process: %s", strerror(errno));
    return 1;
  }
  if (fork_pid == 0) {
    tcp_stack *stack = tcp_stack_create();
    tcp_stack_start(stack);
  }
  return 0;
}

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

int main(int argc, char *const argv[]) {
  if (argc < 2) {
    quit_with_error();
  }

  // Skip over binary name
  argc--;
  argv++;

  char *subcommand = argv[0];
  argc--;
  argv++;

  for (int i = 0; i < SUBCOMMAND_COUNT; i++) {
    if (!strcmp(subcommand, subcommands[i].name)) {
      return subcommands[i].handler(argc, argv);
    }
  }

  quit_with_error();
}
