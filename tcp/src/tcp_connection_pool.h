#pragma once

#include "./tcp_connection.h"
#include <pthread.h>
#include <stdint.h>

typedef struct tcp_connection_pool {
  tcp_connection *buffer;
  uint32_t length;
  pthread_mutex_t mutex;
} tcp_connection_pool;

tcp_connection_pool tcp_connection_pool_create(void);
void tcp_connection_pool_destroy(tcp_connection_pool *connection_pool);
