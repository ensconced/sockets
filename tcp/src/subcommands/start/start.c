#include "../../tcp_stack/tcp_stack.h"
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

typedef struct parsed_options {
  bool detach;
} parsed_options;

parsed_options default_options = (parsed_options){.detach = false};

char optstring[] = "d";

struct option longopts[] = {
    (struct option){
        .name = "detach",
        .has_arg = no_argument,
        .flag = NULL,
        .val = 'd',
    },
    // sentinel value required by getopt_long to signify end of array
    (struct option){0, 0, 0, 0},
};
parsed_options parse_options(int argc, char **argv) {
  parsed_options options = default_options;
  int opt = 0;
  while ((opt = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
    switch (opt) {
    case 'd': {
      options.detach = true;
      break;
    }
    default: {
      // TODO - should use quit_with_error or smth here
      printf("unrecognized option\n");
      exit(1);
    }
    }
  }
  return options;
}

int handle_start_subcommand(int argc, char **argv) {
  parsed_options options = parse_options(argc, argv);

  if (options.detach) {
    pid_t fork_pid = fork();
    if (fork_pid == -1) {
      fprintf(stderr, "Failed to fork daemon process: %s", strerror(errno));
      return 1;
    }
    if (fork_pid == 0) {
      tcp_stack *stack = tcp_stack_create();
      tcp_stack_start(stack);
    }
  } else {
    tcp_stack *stack = tcp_stack_create();
    tcp_stack_start(stack);
  }

  return 0;
}
