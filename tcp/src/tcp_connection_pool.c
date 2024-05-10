#include "./tcp_connection_pool.h"
#include "./constants.h"
#include <stdio.h>
#include <stdlib.h>

tcp_connection_pool tcp_connection_pool_create(void) {
  tcp_connection *connections_buffer =
      malloc(sizeof(tcp_connection) * MAX_CONNECTIONS);
  if (connections_buffer == 0) {
    fprintf(stderr, "Failed to malloc connections buffer\n");
    exit(1);
  }
  for (int i = 0; i < MAX_CONNECTIONS; i++) {
    connections_buffer[i] = (tcp_connection){.mode = PASSIVE, .state = CLOSED};
  }
  pthread_mutex_t mutex;
  pthread_mutex_init(&mutex, NULL);
  return (tcp_connection_pool){
      .buffer = connections_buffer, .length = 0, .mutex = mutex};
}

void tcp_connection_pool_destroy(tcp_connection_pool *connection_pool) {
  free(connection_pool->buffer);
  pthread_mutex_destroy(&connection_pool->mutex);
}
