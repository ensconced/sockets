#include <errno.h>
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

void write_tcp_segment(vec buffer, uint8_t **ptr, tcp_connection *conn,
                       uint8_t *payload, size_t payload_len, uint8_t flags,
                       uint32_t seq_number, uint32_t ack_number) {
  uint16_t source_port = htons(conn->local_socket.port);
  uint16_t dest_port = htons(conn->remote_socket.port);
  uint32_t seq = htonl(seq_number);
  uint32_t ack = htonl(ack_number);
  uint16_t window = htons(WINDOW);
  uint16_t checksum = 0;
  uint16_t urgent_pointer = htons(0); // TODO
  uint8_t max_segment_size_option_kind = 2;
  uint8_t max_segment_size_option_length = 4;
  uint16_t max_segment_size_value = htons(MAX_SEGMENT_SIZE);
  uint8_t end_of_option_list = 0;

  // we'll initially write a zero for the data offset but will overwrite it once
  // we know the proper value
  uint8_t data_offset = 0;

  uint8_t *start = *ptr;

  push_uint16_t(buffer, ptr, source_port);
  push_uint16_t(buffer, ptr, dest_port);
  push_uint32_t(buffer, ptr, seq);
  push_uint32_t(buffer, ptr, ack);

  // grab a pointer to the data offset so we can overwrite it later...
  uint8_t *data_offset_ptr = *ptr;
  push_uint8_t(buffer, ptr, data_offset);
  push_uint8_t(buffer, ptr, flags);
  push_uint16_t(buffer, ptr, window);
  // grab a pointer to the checksum so we can overwrite it later too...
  uint8_t *checksum_ptr = *ptr;
  push_uint16_t(buffer, ptr, checksum);
  push_uint16_t(buffer, ptr, urgent_pointer);
  push_uint8_t(buffer, ptr, max_segment_size_option_kind);
  push_uint8_t(buffer, ptr, max_segment_size_option_length);
  push_uint16_t(buffer, ptr, max_segment_size_value);
  // pad out options until they reach a 32bit word boundary
  while ((*ptr - start) % 4) {
    push_uint8_t(buffer, ptr, end_of_option_list);
  }
  // ...now that we know where the payload is going, we can insert the correct
  // value for the data_offset.
  uint8_t data_byte_offset = (uint8_t)(*ptr - start);
  uint8_t data_word_offset = data_byte_offset / 4;
  *data_offset_ptr = (uint8_t)(data_word_offset << 4);

  // append the actual data
  if (payload_len > 0) {
    if (*ptr + payload_len <= buffer.buffer + buffer.len) {
      memcpy(*ptr, payload, payload_len);
      ptr += payload_len;
    } else {
      fprintf(stderr, "Payload does not fit in buffer\n");
      exit(1);
    }
  }

  uint16_t data_len = (uint16_t)(*ptr - start);

  uint16_t pseudo_header[6] = {
      htonl(conn->local_socket.ipv4_addr) >> 16,
      htonl(conn->local_socket.ipv4_addr) & 0xFFFF,
      htonl(conn->remote_socket.ipv4_addr) >> 16,
      htonl(conn->remote_socket.ipv4_addr) & 0xFFFF,
      htons(IPPROTO_TCP),
      htons(data_len),
  };

  uint32_t csum = 0;
  checksum_update(&csum, (void *)pseudo_header, sizeof(pseudo_header));
  checksum_update(&csum, start, data_len);
  uint16_t big_endian_checksum = htons(checksum_finalize(&csum));
  memcpy(checksum_ptr, &big_endian_checksum, sizeof(big_endian_checksum));
}

void write_ipv4_header(vec send_buffer, uint8_t **ptr, uint32_t source_address,
                       uint32_t destination_address,
                       uint8_t **total_length_ptr_result,
                       uint8_t **checksum_ptr_result) {
  uint8_t version = 4;
  // TODO - compute this based on the options being used. This is the correct
  // value when not using any options.
  uint8_t internet_header_length = 5;
  uint8_t type_of_service = 0;
  // this gets filled in later
  uint16_t total_length = 0;
  // TODO - this is fine as zero for now, because I'm using "Don't Fragment"
  uint16_t identification = 0;
  // TODO - I guess this is correct if we're not using fragmentation?
  uint16_t flags_and_fragment_offset = 0;
  uint8_t time_to_live = 64;
  uint8_t protocol = IPPROTO_TCP;
  uint16_t checksum = 0;
  uint8_t version_and_ihl = (uint8_t)(version << 4) | internet_header_length;

  push_uint8_t(send_buffer, ptr, version_and_ihl);
  push_uint8_t(send_buffer, ptr, type_of_service);
  *total_length_ptr_result = *ptr;
  push_uint16_t(send_buffer, ptr, total_length);
  push_uint16_t(send_buffer, ptr, identification);
  push_uint16_t(send_buffer, ptr, flags_and_fragment_offset);
  push_uint8_t(send_buffer, ptr, time_to_live);
  push_uint8_t(send_buffer, ptr, protocol);
  *checksum_ptr_result = *ptr;
  push_uint16_t(send_buffer, ptr, checksum);
  uint32_t big_endian_source_address = htonl(source_address);
  push_uint32_t(send_buffer, ptr, big_endian_source_address);
  uint32_t big_endian_destination_address = htonl(destination_address);
  push_uint32_t(send_buffer, ptr, big_endian_destination_address);
}

// TODO - should we use the tcp_segment type as the param here?
void tcp_send_segment(tcp_stack *stack, tcp_connection *conn, uint8_t *payload,
                      size_t payload_len, uint8_t flags, uint32_t seq_number,
                      uint32_t ack_number) {
  pthread_mutex_lock(stack->raw_socket.mutex);

  uint8_t *ptr = stack->raw_socket.send_buffer.buffer;

  uint8_t *total_ip_packet_length_ptr;
  uint8_t *ip_header_checksum_ptr;
  write_ipv4_header(stack->raw_socket.send_buffer, &ptr,
                    conn->local_socket.ipv4_addr, conn->remote_socket.ipv4_addr,
                    &total_ip_packet_length_ptr, &ip_header_checksum_ptr);
  write_tcp_segment(stack->raw_socket.send_buffer, &ptr, conn, payload,
                    payload_len, flags, seq_number, ack_number);
  uint16_t total_ip_packet_length =
      (uint16_t)(ptr - stack->raw_socket.send_buffer.buffer);
  uint16_t big_endian_total_ip_packet_length = htons(total_ip_packet_length);
  memcpy(total_ip_packet_length_ptr, &big_endian_total_ip_packet_length,
         sizeof(big_endian_total_ip_packet_length));

  uint32_t csum = 0;
  checksum_update(&csum, stack->raw_socket.send_buffer.buffer,
                  total_ip_packet_length);
  uint16_t big_endian_checksum = htons(checksum_finalize(&csum));
  memcpy(ip_header_checksum_ptr, &big_endian_checksum,
         sizeof(big_endian_checksum));

  //  TODO - this could just be created once and kept on the tcp connection
  //  object.
  struct sockaddr_in dest_addr = {
      .sin_family = AF_INET,
      .sin_addr =
          (struct in_addr){.s_addr = htonl(conn->remote_socket.ipv4_addr)},
  };

  uint16_t data_len = (uint16_t)(ptr - stack->raw_socket.send_buffer.buffer);
  if (sendto(stack->raw_socket.fd, stack->raw_socket.send_buffer.buffer,
             data_len, 0, (struct sockaddr *)(&dest_addr),
             sizeof(dest_addr)) == -1) {
    fprintf(stderr, "Failed to send IP packet: %s\n", strerror(errno));
  };

  pthread_mutex_unlock(stack->raw_socket.mutex);
}
