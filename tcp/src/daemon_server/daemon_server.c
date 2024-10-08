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

  void *read_buffer = checked_malloc(DAEMON_SOCKET_READ_BUFFER_SIZE, "daemon server read buffer");

  pthread_t *thread = checked_malloc(sizeof(pthread_t), "daemon server thread");

  *server = (daemon_server){
      .socket_fd = socket_fd,
      .poll_fds = poll_fds,
      .poll_fds_len = poll_fds_len,
      .thread = thread,
      .read_buffer = read_buffer,
  };

  return server;
}

int accept_connection(daemon_server *server) {
  if (server->poll_fds_len == POLL_FDS_CAPACITY) {
    fprintf(stderr, "Exceeded POLL_FDS_CAPACITY\n");
    exit(1);
  } else {
    int fd = accept(server->socket_fd, NULL, NULL);
    if (fd == -1) {
      fprintf(stderr, "Failed to accept connection on daemon server: %s\n", strerror(errno));
      exit(1);
    }
    server->poll_fds[server->poll_fds_len++] = (struct pollfd){
        .fd = fd,
        .events = POLLIN,
    };
  }
}

uint8_t read_single_byte_or_close(daemon_server *server, int fd, size_t idx) {
  uint8_t byte;
  ssize_t bytes_read = read(fd, &byte, 1);
  switch (bytes_read) {
  case 1:
    return byte;
  case 0: {
    if (close(fd) == -1) {
      fprintf(stderr, "Failed to close fd: %s\n", strerror(errno));
      exit(1);
    } else {
      // remove from pollfds
      // TODO - this does mean the item that was at the end will end up getting skipped on this iteration of the for
      // loop...
      // ...this is probably fine though? Reaslistically it's not going to be a long list anyway...
      server->poll_fds[idx] = server->poll_fds[server->poll_fds_len - 1];
      server->poll_fds_len--;
    }
    break;
  }
  case -1: {
    fprintf(stderr, "Failed to read byte: %s\n", strerror(errno));
    exit(1);
  }
  default: {
    fprintf(stderr, "Unexpected error reading byte\n");
    exit(1);
  }
  }
}

void *daemon_server_thread_entrypoint(void *arg) {
  tcp_stack *stack = arg;
  daemon_server *server = stack->daemon_server;
  while (!atomic_load(stack->destroyed)) {
    int ready_fds = poll(server->poll_fds, 1, DAEMON_SOCKET_POLL_TIMEOUT_MS);
    if (ready_fds > 0) {
      for (size_t poll_fd_idx = 0; poll_fd_idx < server->poll_fds_len; poll_fd_idx++) {
        struct pollfd poll_fd = server->poll_fds[poll_fd_idx];
        if (poll_fd.revents & POLLIN) {
          if (poll_fd.fd == server->socket_fd) {
            accept_connection(server);
          } else {
            uint8_t byte = read_single_byte_or_close(server, poll_fd.fd, poll_fd_idx);
            // TODO - interpret byte as action, and apply action handler
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
  free(server->read_buffer);
  free(server);
}
