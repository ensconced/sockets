#pragma once

#include "../mpsc_queue/mpsc_queue.h"
#include <pthread.h>

typedef struct tcp_stack tcp_stack;

typedef struct daemon_server {
  int socket_fd;
  struct pollfd *poll_fds;
  int poll_fds_len;
  pthread_t *thread;
  void *read_buffer;
} daemon_server;

typedef enum daemon_server_action {
  DAEMON_STOP,
} daemon_server_action;

daemon_server *daemon_server_create(void);
void *daemon_server_thread_entrypoint(void *stack);
void daemon_server_destroy(daemon_server *server);
