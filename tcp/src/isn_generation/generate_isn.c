#define _POSIX_C_SOURCE 199309L

#include "../lib.h"
#include "../tcp_stack.h"
#include "./md5.h"
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

uint32_t generate_isn(tcp_stack *stack, tcp_socket local_socket,
                      tcp_socket remote_socket) {
  struct timespec time;
  if (clock_gettime(CLOCK_MONOTONIC, &time) != 0) {
    fprintf(stderr, "Error reading clock: %s", strerror(errno));
  }
  uint64_t microseconds =
      (uint64_t)time.tv_sec * 1000 * 1000 + (uint64_t)time.tv_nsec / 1000;
  uint32_t fours_of_microseconds = (uint32_t)(microseconds / 4);

  md5_result hash_result =
      md5(stack->md5_algorithm, local_socket.ipv4_addr, local_socket.port,
          remote_socket.ipv4_addr, remote_socket.port);

  uint32_t bottom_32_bits_of_hash;
  memcpy(&bottom_32_bits_of_hash, hash_result.hash,
         sizeof(bottom_32_bits_of_hash));

  return fours_of_microseconds + bottom_32_bits_of_hash;
}
