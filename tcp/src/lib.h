#pragma once

#include "./tcp_socket.h"
#include <stdint.h>

typedef struct tcp_connection tcp_connection;
typedef struct tcp_stack tcp_stack;

tcp_stack *tcp_stack_create(void);
void tcp_stack_destroy(tcp_stack *stack);

tcp_connection *tcp_open_passive(tcp_stack *stack, tcp_socket local_socket);
tcp_connection *tcp_open_active(tcp_stack *stack, tcp_socket local_socket,
                                tcp_socket remote_socket);

void tcp_send(tcp_connection *conn, void *data, uint32_t data_size);
