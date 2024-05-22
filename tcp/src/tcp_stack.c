#include "./tcp_stack.h"
#include "./config.h"
#include "./receive_datagrams/receive_datagrams.h"
#include "./tcp_connection/tcp_connection_pool.h"
#include <errno.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

tcp_raw_socket tcp_raw_socket_create(void) {
  struct sockaddr_in local_sock_addr;
  local_sock_addr.sin_family = AF_INET;
  local_sock_addr.sin_addr.s_addr = ntohl(LOCAL_IP);

  int ip_sock_fd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
  if (ip_sock_fd == -1) {
    fprintf(stderr, "Failed to create socket: %s\n", strerror(errno));
    exit(1);
  }

  if (bind(ip_sock_fd, (struct sockaddr *)&local_sock_addr,
           sizeof(local_sock_addr)) != 0) {
    fprintf(stderr, "Failed to bind raw socket: %s", strerror(errno));
    exit(1);
  }

  // TODO - error handling
  pthread_mutex_t *socket_mutex = malloc(sizeof(pthread_mutex_t));
  // TODO - error handling
  pthread_mutex_init(socket_mutex, NULL);
  // TODO - error handling
  uint8_t *socket_send_buffer =
      malloc(RAW_SOCKET_SEND_BUFFER_LEN * sizeof(uint8_t));
  // TODO - error handling
  uint8_t *socket_receive_buffer =
      malloc(RAW_SOCKET_RECEIVE_BUFFER_LEN * sizeof(uint8_t));

  return (tcp_raw_socket){
      .fd = ip_sock_fd,
      .mutex = socket_mutex,
      .send_buffer = socket_send_buffer,
      .receive_buffer = socket_receive_buffer,
  };
}

void tcp_raw_socket_destroy(tcp_raw_socket *raw_socket) {
  if (close(raw_socket->fd) != 0) {
    fprintf(stderr, "Failed to close raw socket: %s\n", strerror(errno));
  }
  free(raw_socket->send_buffer);
  free(raw_socket->receive_buffer);
  // TODO - error handling
  pthread_mutex_destroy(raw_socket->mutex);
  free(raw_socket->mutex);
}

tcp_stack *tcp_stack_create(void) {
  EVP_MD *md5_algorithm = EVP_MD_fetch(NULL, "MD5", "provider=default");
  if (md5_algorithm == NULL) {
    fprintf(stderr, "Failed to fetch md5 algorithm\n");
    exit(1);
  }

  // TODO - error handling
  atomic_bool *destroyed = malloc(sizeof(atomic_bool));
  atomic_init(destroyed, false);

  // TODO - error handling
  tcp_stack *stack = malloc(sizeof(tcp_stack));
  *stack = (tcp_stack){
      .connection_pool = tcp_connection_pool_create(),
      .raw_socket = tcp_raw_socket_create(),
      .md5_algorithm = md5_algorithm,
      .destroyed = destroyed,
  };

  if (pthread_create(&stack->incoming_datagram_handler_thread, NULL,
                     &receive_datagrams, stack) != 0) {
    fprintf(stderr, "Failed to create datagram handling thread\n");
    exit(1);
  };

  // pthread_t timeout_handler_thread_id;
  // if (pthread_create(&timeout_handler_thread_id, NULL, handle_timeouts,
  //                    connections) != 0) {
  //   fprintf(stderr, "Failed to crzeate timeout handling thread\n");
  //   exit(1);
  // };

  return stack;
}

void tcp_stack_destroy(tcp_stack *stack) {
  atomic_store(stack->destroyed, true);
  pthread_join(stack->incoming_datagram_handler_thread, NULL);
  free(stack->destroyed);
  EVP_MD_free(stack->md5_algorithm);
  tcp_raw_socket_destroy(&stack->raw_socket);
  pthread_mutex_lock(stack->connection_pool.mutex);
  tcp_connection_pool_destroy(&stack->connection_pool);
  free(stack);
}
