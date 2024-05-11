#include "./tcp_stack.h"
#include "./tcp_connection_pool.h"
#include <errno.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define RAW_SOCKET_SEND_BUFFER_LEN 65536

tcp_raw_socket tcp_raw_socket_create(void) {
  int ip_sock_fd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
  if (ip_sock_fd == -1) {
    fprintf(stderr, "Failed to create socket: %s\n", strerror(errno));
    exit(1);
  }

  pthread_mutex_t socket_mutex;
  pthread_mutex_init(&socket_mutex, NULL);
  uint8_t *socket_send_buffer =
      malloc(RAW_SOCKET_SEND_BUFFER_LEN * sizeof(uint8_t));

  return (tcp_raw_socket){
      .fd = ip_sock_fd,
      .mutex = socket_mutex,
      .send_buffer = socket_send_buffer,
  };
}

void tcp_raw_socket_destroy(tcp_raw_socket *raw_socket) {
  if (close(raw_socket->fd) != 0) {
    fprintf(stderr, "Failed to close raw socket: %s\n", strerror(errno));
  }
  free(raw_socket->send_buffer);
  pthread_mutex_destroy(&raw_socket->mutex);
}

tcp_stack *tcp_stack_create(void) {
  // pthread_t incoming_datagram_handler_thread_id;
  // if (pthread_create(&incoming_datagram_handler_thread_id, NULL,
  //                    handle_incoming_datagrams, connections) != 0) {
  //   fprintf(stderr, "Failed to create datagram handling thread\n");
  //   exit(1);
  // };

  // pthread_t timeout_handler_thread_id;
  // if (pthread_create(&timeout_handler_thread_id, NULL, handle_timeouts,
  //                    connections) != 0) {
  //   fprintf(stderr, "Failed to crzeate timeout handling thread\n");
  //   exit(1);
  // };

  EVP_MD *md5_algorithm = EVP_MD_fetch(NULL, "MD5", "provider=default");
  if (md5_algorithm == NULL) {
    fprintf(stderr, "Failed to fetch md5 algorithm\n");
    exit(1);
  }

  tcp_stack *stack = malloc(sizeof(tcp_stack));
  *stack = (tcp_stack){
      .connection_pool = tcp_connection_pool_create(),
      .raw_socket = tcp_raw_socket_create(),
      .md5_algorithm = md5_algorithm,
  };
  return stack;
}

void tcp_stack_destroy(tcp_stack *stack) {
  // TODO - should also clean up / join pthreads...
  EVP_MD_free(stack->md5_algorithm);
  tcp_raw_socket_destroy(&stack->raw_socket);
  tcp_connection_pool_destroy(&stack->connection_pool);
  free(stack);
}
