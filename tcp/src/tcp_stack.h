#pragma once

#include "./lib.h"
#include "./tcp_connection/tcp_connection_pool.h"
#include <openssl/evp.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct tcp_raw_socket {
  int fd;
  uint8_t *send_buffer;
  uint8_t *receive_buffer;
  pthread_mutex_t *mutex;
} tcp_raw_socket;

typedef struct tcp_stack {
  atomic_bool *destroyed;
  pthread_t incoming_datagram_handler_thread;
  tcp_connection_pool connection_pool;
  tcp_raw_socket raw_socket;
  EVP_MD *md5_algorithm;
} tcp_stack;
