#include <netinet/in.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>

#include "./lib.h"

// Flags
#define CWR 128
#define ECE 64
#define URG 32
#define ACK 16
#define PSH 8
#define RST 4
#define SYN 2
#define FIN 1

#define push_value(ptr, value)                                                 \
  do {                                                                         \
    memcpy(ptr, &value, sizeof(value));                                        \
    ptr += sizeof(value);                                                      \
  } while (0)

uint16_t compute_checksum(uint32_t source_ip, uint32_t dest_ip, uint8_t *data,
                          uint16_t data_len) {
  // The spec requires that we use 16 bit one's complement addition to compute
  // the checksum. An efficient way of acheiving the same result is by doing 32
  // bit addition (of the 16 bit chunks) and then at the end incorporating
  // anything that "overflowed" out of the bottom 16 bits back into the
  // bottom 16 bits...
  uint32_t acc = 0;

  // First, the ipv4 pseudo-header.
  uint32_t network_order_source_ip = htonl(source_ip);
  acc += network_order_source_ip >> 16;
  acc += network_order_source_ip & 0x0000FFFF;
  uint32_t network_order_dest_ip = htonl(dest_ip);
  acc += network_order_dest_ip >> 16;
  acc += network_order_dest_ip & 0x0000FFFF;
  acc += IPPROTO_TCP;
  acc += data_len;

  // And then the actual data of the segment.
  for (size_t i = 0; i < data_len; i += 2) {
    uint8_t msb = data[i];
    uint8_t lsb = i + 1 == data_len ? 0 : data[i + 1];
    acc += (msb << 8) & lsb;
  }

  // Now incorporate any overflows back into the lower 16 bits.
  uint32_t overflow = acc >> 16;
  acc += overflow;
  // Adding the first one of overflows may actually have created another
  // overflow. We can simply repeat the same operation to account for this:
  overflow = acc >> 16;
  return ~(uint16_t)(acc + overflow);
}

void tcp_send_segment(tcp_stack *stack, tcp_connection *conn, uint8_t *payload,
                      size_t payload_len, uint16_t max_segment_size) {
  pthread_mutex_lock(&stack->raw_socket.mutex);

  uint8_t *data = stack->raw_socket.send_buffer;
  uint8_t *ptr = data;

  uint16_t source_port = htons(conn->local_socket.port);
  uint16_t dest_port = htons(conn->remote_socket.port);
  uint32_t seq_number = htonl(1234); // TODO!
  uint32_t ack_number = htonl(1234); // TODO!
  uint8_t flags = SYN & ACK;         // TODO!
  uint16_t window = htons(1234);     // TODO!
  uint16_t initial_checksum = 0;
  uint16_t urgent_pointer = htons(1234); // TODO!
  uint8_t max_segment_size_option_kind = 2;
  uint8_t max_segment_size_option_length = 4;
  uint16_t max_segment_size_value = htons(max_segment_size);
  uint8_t end_of_option_list = 0;
  uint8_t no_op_option = 1;

  // we'll initially write a zero for the data offset but will overwrite it once
  // we know the proper value
  uint8_t data_offset = 0;

  push_value(ptr, source_port);
  push_value(ptr, dest_port);
  push_value(ptr, seq_number);
  push_value(ptr, ack_number);

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
  push_value(ptr, end_of_option_list);
  // pad out options until they reach a 32bit word boundary
  while ((ptr - data) % 4) {
    push_value(ptr, no_op_option);
  }
  // ...now that we know where the payload is going, we can insert the correct
  // value for the data_offset.
  uint8_t data_byte_offset = (uint8_t)(ptr - data);
  uint8_t data_word_offset = data_byte_offset / 4;
  *data_offset_ptr = (uint8_t)(data_word_offset << 4);

  // append the actual data
  memcpy(ptr, payload, payload_len);

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
  pthread_mutex_unlock(&stack->raw_socket.mutex);
}
