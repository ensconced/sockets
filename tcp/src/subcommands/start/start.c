#include "../../tcp_stack/tcp_stack.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

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
