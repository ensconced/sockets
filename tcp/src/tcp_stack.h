#pragma once

#include "./mpsc_queue/mpsc_queue.h"
#include "./tcp_connection/tcp_connection_pool.h"
#include <openssl/evp.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct tcp_raw_socket {
  int fd;
  vec send_buffer;
  vec receive_buffer;
  pthread_mutex_t *mutex;
} tcp_raw_socket;

typedef struct tcp_raw_socket_pool {
  hash_map *raw_sockets_by_local_ip;
} tcp_raw_socket_pool;

typedef struct tcp_stack {
  atomic_bool *destroyed;
  pthread_t incoming_datagram_handler_thread;
  tcp_connection_pool connection_pool;
  tcp_raw_socket raw_socket;
  EVP_MD *md5_algorithm;
  mpsc_queue *event_queue;
} tcp_stack;

void tcp_stack_create(void);
void tcp_stack_destroy(tcp_stack *stack);
