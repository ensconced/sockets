#pragma once

#include "../tcp_socket.h"
#include "../tcp_stack/tcp_stack.h"
#include <stdint.h>

uint32_t generate_isn(tcp_stack *stack, internal_tcp_socket local_socket, internal_tcp_socket remote_socket);
