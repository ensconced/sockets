#pragma once

#include "../config.h"
#include "../tcp_connection/tcp_connection_pool.h"
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
  tcp_connection_pool connection_pool;
  tcp_raw_socket raw_socket;
  EVP_MD *md5_algorithm;
  pthread_t incoming_datagram_handler_thread;
} tcp_stack;

tcp_stack *tcp_stack_create();
void tcp_stack_start(tcp_stack *stack);
void tcp_stack_destroy(tcp_stack *stack);
