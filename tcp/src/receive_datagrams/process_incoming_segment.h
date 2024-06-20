#pragma once

#include "../tcp_stack.h"
#include "./segment.h"

void process_incoming_segment(tcp_stack *stack, uint32_t source_address,
                              uint32_t dest_address, tcp_segment segment);
