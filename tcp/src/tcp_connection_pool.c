#include "./tcp_connection_pool.h"
#include "./hash_map/hash_map.h"
#include <stdio.h>
#include <stdlib.h>

tcp_connection_pool tcp_connection_pool_create(void) {
  hash_map *connections_in_listen_state = hash_map_create();
  hash_map *connections_not_in_listen_state = hash_map_create();
  pthread_mutex_t mutex;
  pthread_mutex_init(&mutex, NULL);
  return (tcp_connection_pool){
      .connections_in_listen_state = connections_in_listen_state,
      .connections_not_in_listen_state = connections_not_in_listen_state,
      .mutex = mutex,
  };
}

void tcp_connection_pool_destroy(tcp_connection_pool *connection_pool) {
  hash_map_destroy(connection_pool->connections_in_listen_state);
  hash_map_destroy(connection_pool->connections_not_in_listen_state);
  pthread_mutex_destroy(&connection_pool->mutex);
}
