#pragma once

#include "./lib.h"
#include <openssl/evp.h>
#include <pthread.h>

typedef enum tcp_conection_mode { PASSIVE, ACTIVE } tcp_connection_mode;

typedef enum tcp_connection_state {
  CLOSED,
  LISTEN,
  SYN_RECEIVED,
  SYN_SENT,
  ESTABLISHED,
  FIN_WAIT_1,
  FIN_WAIT_2,
  CLOSE_WAIT,
  CLOSING,
  LAST_ACK,
  TIME_WAIT
} tcp_connection_state;

typedef struct tcp_connection {
  tcp_connection_state state;
  tcp_connection_mode mode;
  tcp_socket local_socket;
  tcp_socket remote_socket;
  uint32_t initial_send_seq_number;
} tcp_connection;

typedef struct tcp_connection_pool {
  tcp_connection *buffer;
  size_t length;
  pthread_mutex_t mutex;
} tcp_connection_pool;

typedef struct tcp_raw_socket {
  int fd;
  uint8_t *send_buffer;
  pthread_mutex_t mutex;
} tcp_raw_socket;

typedef struct tcp_stack {
  tcp_connection_pool connection_pool;
  tcp_raw_socket raw_socket;
  EVP_MD *md5_algorithm;
} tcp_stack;

tcp_stack tcp_stack_create(void);
void tcp_stack_destroy(tcp_stack *stack);
