#include "../config.h"
#include "../error_handling/error_handling.h"
#include "../tcp_stack/tcp_stack.h"
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

// One fd for each connection, plus one for the socket fd itself.
#define POLL_FDS_CAPACITY (DAEMON_SOCKET_MAX_CONNECTIONS + 1)

daemon_server *daemon_server_create(void) {
  daemon_server *server = checked_malloc(sizeof(daemon_server), "daemon server");

  int socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    fprintf(stderr, "Failed to create daemon server socket: %s\n", strerror(errno));
    exit(1);
  }

  struct sockaddr_un socket_address = (struct sockaddr_un){
      .sun_family = AF_LOCAL,
      .sun_path = DAEMON_SOCKET_LOCATION,
  };

  if (bind(socket_fd, (struct sockaddr *)&socket_address, sizeof(struct sockaddr_un)) == -1) {
    fprintf(stderr, "Failed to bind daemon server socket to address: %s\n", strerror(errno));
    exit(1);
  }

  if (listen(socket_fd, DAEMON_SOCKET_CONNECTION_QUEUE_LEN) == -1) {
    fprintf(stderr, "Failed to mark daemon server socket as passive: %s\n", strerror(errno));
    exit(1);
  };

  struct pollfd *poll_fds = checked_malloc(POLL_FDS_CAPACITY * sizeof(struct pollfd), "daemon server poll fds");
  int poll_fds_len = 1;
  poll_fds[0] = (struct pollfd){
      .fd = socket_fd,
      .events = POLLIN,
  };

  pthread_t *thread = checked_malloc(sizeof(pthread_t), "daemon server thread");

  *server = (daemon_server){
      .socket_fd = socket_fd,
      .poll_fds = poll_fds,
      .poll_fds_len = poll_fds_len,
      .thread = thread,
  };

  return server;
}

void *daemon_server_thread_entrypoint(void *arg) {
  tcp_stack *stack = arg;
  while (true) {
    int ready_fds = poll(stack->daemon_server->poll_fds, 1, DAEMON_SOCKET_POLL_TIMEOUT_MS);
    if (ready_fds > 0) {
      for (int i = 0; i < stack->daemon_server->poll_fds_len; i++) {
        struct pollfd poll_fd = stack->daemon_server->poll_fds[i];
        if (poll_fd.revents & POLLIN) {
          if (poll_fd.fd == stack->daemon_server->socket_fd) {
            // ready to accept a connection
          } else {
            // ready to read from connection
          }
        }
      }
    } else if (ready_fds == -1) {
      fprintf(stderr, "Error polling daemon server socket: %s\n", strerror(errno));
      exit(1);
    }
  }
}

void daemon_server_destroy(daemon_server *server) {
  if (unlink(DAEMON_SOCKET_LOCATION) == -1) {
    fprintf(stderr, "Failed to unlink daemon server socket: %s\n", strerror(errno));
    exit(1);
  }
  free(server->poll_fds);
  free(server->thread);
  free(server);
}
