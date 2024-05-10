#pragma once

#include <netinet/ip.h>
#include <openssl/evp.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

typedef struct {
  uint32_t ipv4_addr;
  uint16_t port;
} tcp_socket;

typedef struct tcp_connection tcp_connection;
typedef struct tcp_stack tcp_stack;

tcp_stack tcp_init(void);
tcp_connection *tcp_open_passive(tcp_stack *stack, tcp_socket local_socket);
tcp_connection *tcp_open_active(tcp_stack *stack, tcp_socket local_socket,
                                tcp_socket remote_socket);

// void tcp_send(tcp_connection *conn);
