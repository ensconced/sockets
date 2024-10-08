#include "../config.h"
#include "../tcp_stack/tcp_stack.h"
#include "./md5/md5.h"
#include "./secret_key/secret_key.h"
#include <errno.h>
#include <stdint.h>
#include <string.h>

uint32_t generate_isn(tcp_stack *stack, internal_tcp_socket local_socket, internal_tcp_socket remote_socket) {
  struct timespec time;
  if (clock_gettime(CLOCK_MONOTONIC, &time) != 0) {
    fprintf(stderr, "Error reading clock: %s", strerror(errno));
    exit(1);
  }
  uint64_t microseconds = (uint64_t)time.tv_sec * 1000 * 1000 + (uint64_t)time.tv_nsec / 1000;
  uint32_t fours_of_microseconds = (uint32_t)(microseconds / 4);

  md5_result hash_result =
      md5(stack->md5_algorithm, local_socket.host_order_ipv4_addr, local_socket.host_order_port,
          remote_socket.host_order_ipv4_addr, remote_socket.host_order_port, secret_key, secret_key_len);

  uint32_t truncated_hash;
  memcpy(&truncated_hash, hash_result.hash, sizeof(truncated_hash));
  return fours_of_microseconds + truncated_hash;
}
