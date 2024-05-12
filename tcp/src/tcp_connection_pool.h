#pragma once

#include "./hash_map/hash_map.h"
#include "./tcp_connection.h"
#include <pthread.h>
#include <stdint.h>

typedef struct tcp_connection_pool {
  hash_map *connections_in_listen_state;
  hash_map *connections_not_in_listen_state;
  pthread_mutex_t mutex;
} tcp_connection_pool;

tcp_connection_pool tcp_connection_pool_create(void);
void tcp_connection_pool_destroy(tcp_connection_pool *connection_pool);
