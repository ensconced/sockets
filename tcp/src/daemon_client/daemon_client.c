#include "../config.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

int create_daemon_client() {
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
  return fd;
}
