#pragma once

#include "../tcp_socket.h"
#include "../tcp_stack.h"
#include <stdint.h>

uint32_t generate_isn(tcp_stack *stack, tcp_socket local_socket,
                      tcp_socket remote_socket);
