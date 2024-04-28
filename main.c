#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

const char *PORT = "9898";

int main(void) {
  int socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_fd == -1) {
    fprintf(stderr, "Failed to create socket. %s", strerror(errno));
    exit(1);
  }

  struct addrinfo hints = {
      .ai_flags = AI_ADDRCONFIG | AI_NUMERICSERV | AI_PASSIVE,
      .ai_family = PF_INET,
      .ai_socktype = SOCK_STREAM,
      .ai_protocol = IPPROTO_TCP,
  };

  struct addrinfo *addr_results = NULL;
  int gai_error = getaddrinfo(NULL, PORT, &hints, &addr_results);
  if (gai_error != 0) {
    fprintf(stderr, "Failed to get addr info. %s", gai_strerror(gai_error));
    exit(1);
  }
  if (addr_results == NULL) {
    fprintf(stderr, "No addr results found.");
    exit(1);
  }

  struct sockaddr *socket_addr = addr_results->ai_addr;
  int bind_err = bind(socket_fd, socket_addr, sizeof(struct sockaddr));
  if (bind_err != 0) {
    fprintf(stderr, "Failed to bind socket. %s", strerror(errno));
    exit(1);
  }

  freeaddrinfo(addr_results);
  return 0;
}
