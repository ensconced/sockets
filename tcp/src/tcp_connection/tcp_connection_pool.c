#include "./tcp_connection_pool.h"
#include "../hash_map/hash_map.h"
#include <stdio.h>
#include <stdlib.h>

tcp_connection_pool tcp_connection_pool_create(void) {
  hash_map *connections_in_listen_state = hash_map_create();
  hash_map *connections_not_in_listen_state = hash_map_create();
  pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex, NULL);
  return (tcp_connection_pool){
      .connections_in_listen_state = connections_in_listen_state,
      .connections_not_in_listen_state = connections_not_in_listen_state,
      .mutex = mutex,
  };
}

void tcp_connection_pool_add(tcp_connection_pool connection_pool,
                             tcp_connection *connection) {
  tcp_connection_id connection_id = tcp_connection_id_create(
      connection->local_socket, connection->remote_socket);
  hash_map *hash_map_to_add_to =
      connection->mode == PASSIVE
          ? connection_pool.connections_in_listen_state
          : connection_pool.connections_not_in_listen_state;

  // TODO - could optimise this with a `hash_map_insert_if_not_already_present`
  // function.
  tcp_connection *existing_connection = hash_map_get(
      hash_map_to_add_to, connection_id.buffer, connection_id.buffer_len);
  if (existing_connection != NULL) {
    fprintf(stderr, "Connection already exists\n");
    exit(1);
  }
  hash_map_insert(hash_map_to_add_to, connection_id.buffer,
                  connection_id.buffer_len, connection);
  tcp_connection_id_destroy(connection_id);
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