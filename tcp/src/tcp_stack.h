#pragma once

#include <openssl/evp.h>
#include <pthread.h>

typedef struct {
  int fd;
  uint8_t *send_buffer;
  pthread_mutex_t mutex;
} tcp_raw_socket;

typedef struct {
  tcp_connection_pool connection_pool;
  tcp_raw_socket raw_socket;
  EVP_MD *md5_algorithm;
} tcp_stack;

tcp_stack tcp_stack_create(void);
void tcp_stack_destroy(tcp_stack *stack);
