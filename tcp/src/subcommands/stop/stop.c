#include "../../daemon_client/daemon_client.h"
#include "../../daemon_server/daemon_server.h"
#include "stdint.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int handle_stop_subcommand(int, char **) {
  int fd = create_daemon_client();
  uint8_t message = SOCKET_MSG_DESTROY;
  ssize_t bytes_written = write(fd, &message, sizeof(message));
  if (bytes_written == -1) {
    fprintf(stderr, "Failed to write to daemon server socket: %s\n", strerror(errno));
    exit(1);
  } else if (bytes_written != 1) {
    fprintf(stderr, "Wrote %zd bytes to daemon server instead of 1 as expected\n", bytes_written);
    exit(1);
  }
  return 0;
}
