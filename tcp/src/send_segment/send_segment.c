#include <netinet/in.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>

#include "../checksum/checksum.h"
#include "../tcp_connection/tcp_connection.h"
#include "../tcp_stack.h"
#include "../utils.h"

// TODO - check how I'm meant to decide this value
#define MAX_SEGMENT_SIZE 4096
// TODO - I think this should be based on the size of my recieve buffer...but
// this will do for now
#define WINDOW 1024

void tcp_send_segment(tcp_stack *stack, tcp_connection *conn, uint8_t *payload,
                      size_t payload_len, uint8_t flags, uint32_t seq_number,
                      uint32_t ack_number) {
  pthread_mutex_lock(stack->raw_socket.mutex);

  uint8_t *data = stack->raw_socket.send_buffer;
  uint8_t *ptr = data;

  uint16_t source_port = htons(conn->local_socket.port);
  uint16_t dest_port = htons(conn->remote_socket.port);
  uint32_t seq = htonl(seq_number);
  uint32_t ack = htonl(ack_number);
  uint16_t window = htons(WINDOW);
  uint16_t initial_checksum = 0;
  uint16_t urgent_pointer = htons(0); // TODO
  uint8_t max_segment_size_option_kind = 2;
  uint8_t max_segment_size_option_length = 4;
  uint16_t max_segment_size_value = htons(MAX_SEGMENT_SIZE);
  uint8_t end_of_option_list = 0;
  uint8_t no_op_option = 1;

  // we'll initially write a zero for the data offset but will overwrite it once
  // we know the proper value
  uint8_t data_offset = 0;

  push_value(ptr, source_port);
  push_value(ptr, dest_port);
  push_value(ptr, seq);
  push_value(ptr, ack);

  // grab a pointer to the data offset so we can overwrite it later...
  uint8_t *data_offset_ptr = ptr;
  push_value(ptr, data_offset);
  push_value(ptr, flags);
  push_value(ptr, window);
  // grab a pointer to the checksum so we can overwrite it later too...
  uint8_t *checksum_ptr = ptr;
  push_value(ptr, initial_checksum);
  push_value(ptr, urgent_pointer);
  push_value(ptr, max_segment_size_option_kind);
  push_value(ptr, max_segment_size_option_length);
  push_value(ptr, max_segment_size_value);
  // pad out options until they reach a 32bit word boundary
  while ((ptr - data) % 4) {
    push_value(ptr, end_of_option_list);
  }
  // ...now that we know where the payload is going, we can insert the correct
  // value for the data_offset.
  uint8_t data_byte_offset = (uint8_t)(ptr - data);
  uint8_t data_word_offset = data_byte_offset / 4;
  *data_offset_ptr = (uint8_t)(data_word_offset << 4);

  // append the actual data
  if (payload_len > 0) {
    memcpy(ptr, payload, payload_len);
    ptr += payload_len;
  }

  // and now we can insert the correct value for the checksum
  uint16_t data_len = (uint16_t)(ptr - data);

  uint16_t checksum =
      htons(compute_checksum(conn->local_socket.ipv4_addr,
                             conn->remote_socket.ipv4_addr, data, data_len));

  memcpy(checksum_ptr, &checksum, sizeof(checksum));

  struct sockaddr_in dest_addr = {
      .sin_family = AF_INET,
      .sin_addr =
          (struct in_addr){.s_addr = htonl(conn->remote_socket.ipv4_addr)},
  };

  sendto(stack->raw_socket.fd, data, data_len, 0,
         (struct sockaddr *)(&dest_addr), sizeof(dest_addr));
  pthread_mutex_unlock(stack->raw_socket.mutex);
}
