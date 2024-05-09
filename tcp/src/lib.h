#pragma once

#include <netinet/ip.h>
#include <openssl/evp.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

typedef enum {
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

typedef enum { PASSIVE, ACTIVE } tcp_connection_mode;

// TODO - we might want to use opaque types for some of these...

typedef struct {
  uint32_t ipv4_addr;
  uint16_t port;
} tcp_socket;

typedef struct {
  tcp_connection_state state;
  tcp_connection_mode mode;
  tcp_socket local_socket;
  tcp_socket remote_socket;
  uint32_t initial_send_seq_number;
} tcp_connection;

typedef struct {
  tcp_connection *buffer;
  size_t length;
  pthread_mutex_t mutex;
} tcp_connection_pool;

typedef struct tcp_stack tcp_stack;

tcp_stack tcp_init(void);
tcp_connection *tcp_open_passive(tcp_stack *stack, tcp_socket local_socket);
tcp_connection *tcp_open_active(tcp_stack *stack, tcp_socket local_socket,
                                tcp_socket remote_socket);

// void tcp_send(tcp_connection *conn);