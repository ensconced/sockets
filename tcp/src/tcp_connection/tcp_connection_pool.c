#include "./tcp_connection_pool.h"
#include "../buffer_state/buffer_state.h"
#include "../error_handling/error_handling.h"
#include "../hash_map/hash_map.h"
#include <stdio.h>
#include <stdlib.h>

tcp_connection_pool tcp_connection_pool_create() {
  hash_map *connections_in_listen_state = hash_map_create();
  hash_map *connections_not_in_listen_state = hash_map_create();
  return (tcp_connection_pool){
      .connections_in_listen_state = connections_in_listen_state,
      .connections_not_in_listen_state = connections_not_in_listen_state,
  };
}

void tcp_connection_pool_add(tcp_connection_pool connection_pool, tcp_connection *connection) {
  buffer connection_id = tcp_connection_id_create(connection->local_socket, connection->remote_socket);
  hash_map *hash_map_to_add_to = connection->mode == PASSIVE ? connection_pool.connections_in_listen_state
                                                             : connection_pool.connections_not_in_listen_state;

  // TODO - could optimise this with a `hash_map_insert_if_not_already_present`
  // function.
  tcp_connection *existing_connection = hash_map_get(hash_map_to_add_to, connection_id.data, connection_id.size_bytes);
  if (existing_connection != NULL) {
    fprintf(stderr, "Connection already exists\n");
    exit(1);
  }
  hash_map_insert(hash_map_to_add_to, connection_id.data, connection_id.size_bytes, connection);
}

tcp_connection *tcp_connection_pool_find(tcp_connection_pool connection_pool, internal_tcp_socket local_socket,
                                         internal_tcp_socket remote_socket) {
  buffer connection_id = tcp_connection_id_create(local_socket, remote_socket);
  tcp_connection *found =
      hash_map_get(connection_pool.connections_not_in_listen_state, connection_id.data, connection_id.size_bytes);
  if (found == NULL) {
    found = hash_map_get(connection_pool.connections_in_listen_state, connection_id.data, connection_id.size_bytes);
  }
  tcp_connection_id_destroy(connection_id);
  return found;
}

static void tcp_connection_pool_free_all_connections(tcp_connection_pool *connection_pool) {
  hash_map_iterator *listen_state_iter = hash_map_iterator_create(connection_pool->connections_in_listen_state);
  hash_map_iterator *not_listen_state_iter = hash_map_iterator_create(connection_pool->connections_not_in_listen_state);

  buffer_entry *entry;
  while ((entry = hash_map_iterator_take(listen_state_iter)) != NULL) {
    free(entry->key);
    free(entry->value);
  }
  while ((entry = hash_map_iterator_take(not_listen_state_iter)) != NULL) {
    free(entry->key);
    free(entry->value);
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
}
