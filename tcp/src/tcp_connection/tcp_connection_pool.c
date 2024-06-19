#include "./tcp_connection_pool.h"
#include "../error_handling/error_handling.h"
#include "../hash_map/hash_map.h"
#include "../utils.h"
#include <stdio.h>
#include <stdlib.h>

tcp_connection_pool tcp_connection_pool_create(void) {
  hash_map *connections_in_listen_state = hash_map_create();
  hash_map *connections_not_in_listen_state = hash_map_create();
  pthread_mutex_t *mutex =
      checked_malloc(sizeof(pthread_mutex_t), "connection pool mutex");
  pthread_mutex_init(mutex, NULL);
  return (tcp_connection_pool){
      .connections_in_listen_state = connections_in_listen_state,
      .connections_not_in_listen_state = connections_not_in_listen_state,
      .mutex = mutex,
  };
}

void tcp_connection_pool_add(tcp_connection_pool connection_pool,
                             tcp_connection *connection) {
  vec connection_id = tcp_connection_id_create(connection->local_socket,
                                               connection->remote_socket);
  hash_map *hash_map_to_add_to =
      connection->mode == PASSIVE
          ? connection_pool.connections_in_listen_state
          : connection_pool.connections_not_in_listen_state;

  // TODO - could optimise this with a `hash_map_insert_if_not_already_present`
  // function.
  tcp_connection *existing_connection =
      hash_map_get(hash_map_to_add_to, connection_id.buffer, connection_id.len);
  if (existing_connection != NULL) {
    fprintf(stderr, "Connection already exists\n");
    exit(1);
  }
  hash_map_insert(hash_map_to_add_to, connection_id.buffer, connection_id.len,
                  connection);
}

tcp_connection *tcp_connection_pool_find(tcp_connection_pool connection_pool,
                                         internal_tcp_socket local_socket,
                                         internal_tcp_socket remote_socket) {
  vec connection_id = tcp_connection_id_create(local_socket, remote_socket);
  tcp_connection *found =
      hash_map_get(connection_pool.connections_not_in_listen_state,
                   connection_id.buffer, connection_id.len);
  if (found == NULL) {
    found = hash_map_get(connection_pool.connections_in_listen_state,
                         connection_id.buffer, connection_id.len);
  }
  tcp_connection_id_destroy(connection_id);
  return found;
}

void tcp_connection_pool_free_all_connections(
    tcp_connection_pool *connection_pool) {
  hash_map_iterator *listen_state_iter =
      hash_map_iterator_create(connection_pool->connections_in_listen_state);
  hash_map_iterator *not_listen_state_iter = hash_map_iterator_create(
      connection_pool->connections_not_in_listen_state);

  tcp_connection *conn;
  while ((conn = hash_map_iterator_take(listen_state_iter)) != NULL) {
    free(conn);
  }
  while ((conn = hash_map_iterator_take(not_listen_state_iter)) != NULL) {
    free(conn);
  }
  hash_map_iterator_destroy(listen_state_iter);
  hash_map_iterator_destroy(not_listen_state_iter);
}

void tcp_connection_pool_destroy(tcp_connection_pool *connection_pool) {
  // TODO
  // - set atomic flag telling other threads to stop
  // - join other thread
  // - destroy mutex

  // TODO - we may want to actually CLOSE all the connections?
  tcp_connection_pool_free_all_connections(connection_pool);
  hash_map_destroy(connection_pool->connections_in_listen_state);
  hash_map_destroy(connection_pool->connections_not_in_listen_state);
  pthread_mutex_destroy(connection_pool->mutex);
  free(connection_pool->mutex);
}
