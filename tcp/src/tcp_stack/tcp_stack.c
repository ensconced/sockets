#include "./tcp_stack.h"
#include "../config.h"
#include "../error_handling/error_handling.h"
#include "../process_event/process_event.h"
#include "../receive_datagrams/receive_datagrams.h"
#include "../tcp_connection/tcp_connection_pool.h"
#include <errno.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static tcp_raw_socket tcp_raw_socket_create() {
  int ip_sock_fd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
  if (ip_sock_fd == -1) {
    fprintf(stderr, "Failed to create socket: %s\n", strerror(errno));
    exit(1);
  }
  pthread_mutex_t *socket_mutex = checked_malloc(sizeof(pthread_mutex_t), "socket mutex");
  pthread_mutex_init(socket_mutex, NULL);
  uint8_t *socket_send_buffer = checked_malloc(RAW_SOCKET_SEND_BUFFER_LEN * sizeof(uint8_t), "socket_send_buffer");
  uint8_t *socket_receive_buffer =
      checked_malloc(RAW_SOCKET_RECEIVE_BUFFER_LEN * sizeof(uint8_t), "socket_receive_buffer");

  return (tcp_raw_socket){
      .fd = ip_sock_fd,
      .mutex = socket_mutex,
      .send_buffer =
          {
              .buffer = socket_send_buffer,
              .len = RAW_SOCKET_SEND_BUFFER_LEN,
          },
      .receive_buffer =
          {
              .buffer = socket_receive_buffer,
              .len = RAW_SOCKET_RECEIVE_BUFFER_LEN,
          },
  };
}

static void tcp_raw_socket_destroy(tcp_raw_socket *raw_socket) {
  if (close(raw_socket->fd) != 0) {
    fprintf(stderr, "Failed to close raw socket: %s\n", strerror(errno));
    exit(1);
  }
  free(raw_socket->send_buffer.buffer);
  free(raw_socket->receive_buffer.buffer);
  int mutex_destroy_result = pthread_mutex_destroy(raw_socket->mutex);
  if (mutex_destroy_result != 0) {
    fprintf(stderr, "Failed to destroy raw socket mutex: %s\n", strerror(mutex_destroy_result));
    exit(1);
  };

  free(raw_socket->mutex);
}

tcp_stack *tcp_stack_create() {
  EVP_MD *md5_algorithm = EVP_MD_fetch(NULL, "MD5", "provider=default");
  if (md5_algorithm == NULL) {
    fprintf(stderr, "Failed to fetch md5 algorithm\n");
    exit(1);
  }

  atomic_bool *destroyed = checked_malloc(sizeof(atomic_bool), "destroyed flag");
  atomic_init(destroyed, false);

  tcp_stack *stack = checked_malloc(sizeof(tcp_stack), "tcp_stack");
  *stack = (tcp_stack){
      .connection_pool = tcp_connection_pool_create(),
      .raw_socket = tcp_raw_socket_create(),
      .md5_algorithm = md5_algorithm,
      .destroyed = destroyed,
  };

  return stack;
}

void tcp_stack_start(tcp_stack *stack) {
  if (pthread_create(&stack->incoming_datagram_handler_thread, NULL, &receive_datagrams, stack) != 0) {
    fprintf(stderr, "Failed to create datagram handling thread\n");
    exit(1);
  };
}

void tcp_stack_destroy(tcp_stack *stack) {
  atomic_store(stack->destroyed, true);
  pthread_join(stack->incoming_datagram_handler_thread, NULL);

  free(stack->destroyed);
  EVP_MD_free(stack->md5_algorithm);
  tcp_raw_socket_destroy(&stack->raw_socket);
  tcp_connection_pool_destroy(&stack->connection_pool);
  free(stack);
}
