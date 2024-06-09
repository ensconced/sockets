#pragma once

#define _POSIX_C_SOURCE 200112L

#include "./tcp_socket.h"
#include <stdint.h>

#define CWR 128
#define ECE 64
#define URG 32
#define ACK 16
#define PSH 8
#define RST 4
#define SYN 2
#define FIN 1

typedef struct tcp_connection tcp_connection;
typedef struct tcp_stack tcp_stack;

tcp_stack *tcp_stack_create(void);
void tcp_stack_destroy(tcp_stack *stack);

tcp_connection *tcp_open_passive(tcp_stack *stack, tcp_socket local_socket);
tcp_connection *tcp_open_active(tcp_stack *stack, tcp_socket local_socket,
                                tcp_socket remote_socket);

void tcp_send(tcp_connection *conn, void *data, uint32_t data_size);
