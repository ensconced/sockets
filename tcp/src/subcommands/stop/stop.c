#include "../../config.h"
#include "../../daemon_server/daemon_server.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int handle_stop_subcommand(int, char **) {
  int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (fd == -1) {
    fprintf(stderr, "Failed to create unix domain socket: %s\n", strerror(errno));
    exit(1);
  };
  struct sockaddr_un socket_address = {
      .sun_family = AF_LOCAL,
      .sun_path = DAEMON_SOCKET_LOCATION,
  };

  if (connect(fd, (struct sockaddr *)&socket_address, sizeof(socket_address)) == -1) {
    fprintf(stderr, "Failed to connect to daemon server: %s\n", strerror(errno));
    exit(1);
  }

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
