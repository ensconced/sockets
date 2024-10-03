#pragma once

#include "../mpsc_queue/mpsc_queue.h"
#include "../tcp_stack.h"

void daemon_server(tcp_stack *stack);
