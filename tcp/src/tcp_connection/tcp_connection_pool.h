#pragma once

#include "../hash_map/hash_map.h"
#include "./tcp_connection.h"
#include <pthread.h>
#include <stdint.h>

typedef struct tcp_connection_pool {
  hash_map *connections_in_listen_state;
  hash_map *connections_not_in_listen_state;
} tcp_connection_pool;

tcp_connection_pool tcp_connection_pool_create();
void tcp_connection_pool_destroy(tcp_connection_pool *connection_pool);
void tcp_connection_pool_add(tcp_connection_pool connection_pool, tcp_connection *connection);
tcp_connection *tcp_connection_pool_find(tcp_connection_pool connection_pool, internal_tcp_socket local_socket,
                                         internal_tcp_socket remote_socket);
