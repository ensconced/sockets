#pragma once

#include "../tcp_connection/tcp_connection.h"
#include "../tcp_stack.h"
#include <stdint.h>
#include <stdlib.h>

void tcp_send_segment(tcp_stack *stack, tcp_connection *conn, uint8_t *payload,
                      size_t payload_len, uint8_t flags);
