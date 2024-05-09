#include <errno.h>
#include <netinet/ip.h>
#include <openssl/evp.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#include "./lib.h"
#include "./secret_key.h"
#include "./tcp_stack.h"
#include "./utils.h"

#define MAX_CONNECTIONS 256

uint32_t get_isn(tcp_stack *stack, tcp_socket local_socket,
                 tcp_socket remote_socket) {
  struct timespec time;
  if (clock_gettime(CLOCK_MONOTONIC, &time) != 0) {
    fprintf(stderr, "Error reading clock: %s", strerror(errno));
  }
  uint64_t microseconds =
      (uint64_t)time.tv_sec * 1000 * 1000 + (uint64_t)time.tv_nsec / 1000;
  uint32_t fours_of_microseconds = (uint32_t)(microseconds / 4);

  EVP_MD_CTX *hash_ctx = EVP_MD_CTX_new();
  EVP_DigestInit(hash_ctx, stack->md5_algorithm);

  EVP_DigestUpdate(hash_ctx, (uint8_t *)&local_socket.ipv4_addr,
                   sizeof(local_socket.ipv4_addr));
  EVP_DigestUpdate(hash_ctx, (uint8_t *)&local_socket.port,
                   sizeof(local_socket.port));
  EVP_DigestUpdate(hash_ctx, (uint8_t *)&remote_socket.ipv4_addr,
                   sizeof(remote_socket.ipv4_addr));
  EVP_DigestUpdate(hash_ctx, (uint8_t *)&remote_socket.port,
                   sizeof(remote_socket.port));
  EVP_DigestUpdate(hash_ctx, (uint8_t *)secret_key,
                   secret_key_len * sizeof(unsigned char));

  unsigned char hash[16];
  EVP_DigestFinal(hash, hash_ctx, NULL);

  EVP_MD_CTX_free(hash_ctx);

  uint32_t bottom_32_bits_of_hash;
  memcpy(&bottom_32_bits_of_hash, hash, sizeof(bottom_32_bits_of_hash));

  return fours_of_microseconds + bottom_32_bits_of_hash;
}

tcp_connection *tcp_open(tcp_stack *stack, tcp_socket local_socket,
                         tcp_socket remote_socket, tcp_connection_mode mode) {
  // TODO - I think we also need to check whether there is already a connection
  // using the same local & remote sockets - because you shouldn't be able to
  // open a new one using the same pair?
  tcp_connection *conn = NULL;
  pthread_mutex_lock(&stack->connection_pool.mutex);
  for (int i = 0; i < MAX_CONNECTIONS; i++) {
    conn = &stack->connection_pool.buffer[i];
    if (conn->state == CLOSED) {
      // TODO - spec says we should also fill in "Diffserv field,
      // security/compartment, and user timeout information".
      // And also "Verify the security and Diffserv value requested are allowed
      // for this user, if not, return "error: Diffserv value not allowed" or
      // "error: security/compartment not allowed""
      conn->mode = mode;
      conn->state = mode == PASSIVE ? LISTEN : SYN_SENT;
      conn->local_socket = local_socket;
      // For passive connections, we'll fill in these details at a later
      // point (when we receive a SYN).
      if (mode == ACTIVE) {
        conn->remote_socket = remote_socket;
        conn->initial_send_seq_number =
            get_isn(stack, local_socket, remote_socket);
      }
      break;
    }
  }

  // TODO - maybe instead of completely quitting here the spec says we should do
  // something else?
  if (conn == 0) {
    fprintf(stderr, "Failed to open connection\n");
    exit(1);
  }

  if (mode == ACTIVE) {
    // TODO
    // tcp_send_segment(SYN);
  }

  pthread_mutex_unlock(&stack->connection_pool.mutex);
  return conn;
}

tcp_connection_pool tcp_connection_pool_create() {
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
