#include "../checksum/checksum.h"
#include "../config.h"
#include "../lib.h"
#include "../tcp_stack.h"
#include "../utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>

#define HEADER_LENGTH_WITHOUT_OPTIONS_IN_WORDS 5

typedef struct ip_datagram {
  uint8_t version;
  uint8_t internet_header_length;
  uint8_t type_of_service;
  uint16_t identification;
  uint8_t flags;
  uint16_t fragment_offset;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t header_checksum;
  uint32_t source_address;
  uint32_t dest_address;
  vec options_in_receive_buffer;
  vec data_in_receive_buffer;
} ip_datagram;

typedef struct tcp_segment {
  uint16_t source_port;
  uint16_t dest_port;
  uint32_t sequence_number;
  uint32_t acknowledgement_number;
  uint8_t flags;
  uint16_t window;
  uint16_t checksum;
  uint16_t urgent_pointer;
  vec options;
  vec data;
} tcp_segment;

// TODO - if an ip is fragmented, will the OS already have combined them by the
// time I receive it? I think not - so I should really do it myself...

ip_datagram parse_datagram(vec datagram_vec) {
  uint8_t *ptr = datagram_vec.buffer;

  uint8_t version_and_header_length = take_uint8_t(datagram_vec, &ptr);
  uint8_t header_length_in_words = version_and_header_length & 0x0F;

  uint8_t type_of_service = take_uint8_t(datagram_vec, &ptr);
  uint16_t big_endian_total_length = take_uint16_t(datagram_vec, &ptr);
  uint16_t big_endian_identification = take_uint16_t(datagram_vec, &ptr);
  uint16_t flags_and_big_endian_fragment_offset =
      take_uint16_t(datagram_vec, &ptr);
  uint8_t ttl = take_uint8_t(datagram_vec, &ptr);
  uint8_t protocol = take_uint8_t(datagram_vec, &ptr);
  uint16_t big_endian_header_checksum = take_uint16_t(datagram_vec, &ptr);
  uint32_t big_endian_source_address = take_uint32_t(datagram_vec, &ptr);
  uint32_t big_endian_dest_address = take_uint32_t(datagram_vec, &ptr);

  size_t header_length_in_bytes = header_length_in_words * 4;
  size_t options_length_in_bytes =
      header_length_in_bytes - (HEADER_LENGTH_WITHOUT_OPTIONS_IN_WORDS * 4);
  vec options_in_receive_buffer = {
      .buffer = ptr,
      .len = options_length_in_bytes,
  };
  ptr += options_length_in_bytes;

  size_t data_length_in_bytes =
      ntohs(big_endian_total_length) - header_length_in_bytes;
  vec data_in_receive_buffer = {
      .buffer = ptr,
      .len = data_length_in_bytes,
  };

  return (ip_datagram){
      .version = version_and_header_length >> 4,
      .internet_header_length = header_length_in_words,
      .type_of_service = type_of_service,
      .identification = ntohs(big_endian_identification),
      .flags = flags_and_big_endian_fragment_offset >> 13,
      .fragment_offset = ntohs(flags_and_big_endian_fragment_offset & 0x1FFF),
      .ttl = ttl,
      .protocol = protocol,
      .header_checksum = ntohs(big_endian_header_checksum),
      .source_address = ntohl(big_endian_source_address),
      .dest_address = ntohl(big_endian_dest_address),
      .options_in_receive_buffer = options_in_receive_buffer,
      .data_in_receive_buffer = data_in_receive_buffer,
  };
}

tcp_segment parse_segment(vec segment_vec) {
  uint8_t *ptr = segment_vec.buffer;
  uint16_t big_endian_source_port = take_uint16_t(segment_vec, &ptr);
  uint16_t big_endian_dest_port = take_uint16_t(segment_vec, &ptr);
  uint32_t big_endian_sequence_number = take_uint32_t(segment_vec, &ptr);
  uint32_t big_endian_acknowledgement_number = take_uint32_t(segment_vec, &ptr);
  uint8_t data_offset_and_reserved_space = take_uint8_t(segment_vec, &ptr);
  uint8_t flags = take_uint8_t(segment_vec, &ptr);
  uint16_t big_endian_window = take_uint16_t(segment_vec, &ptr);
  uint16_t big_endian_checksum = take_uint16_t(segment_vec, &ptr);
  uint16_t big_endian_urgent_pointer = take_uint16_t(segment_vec, &ptr);
  uint8_t *options_ptr = ptr;
  uint8_t data_offset_in_words = data_offset_and_reserved_space >> 4;
  uint8_t *data_ptr = segment_vec.buffer + data_offset_in_words * 4;
  size_t options_len = (size_t)(data_ptr - options_ptr);
  size_t data_len = segment_vec.len - (size_t)(data_ptr - segment_vec.buffer);

  return (tcp_segment){
      .source_port = ntohs(big_endian_source_port),
      .dest_port = ntohs(big_endian_dest_port),
      .sequence_number = ntohl(big_endian_sequence_number),
      .acknowledgement_number = ntohl(big_endian_acknowledgement_number),
      .flags = flags,
      .window = ntohs(big_endian_window),
      .checksum = ntohs(big_endian_checksum),
      .urgent_pointer = ntohs(big_endian_urgent_pointer),
      .options =
          (vec){
              .buffer = options_ptr,
              .len = options_len,
          },
      .data =
          (vec){
              .buffer = data_ptr,
              .len = data_len,
          },
  };
}

void process_incoming_segment(tcp_stack *stack, uint32_t source_address,
                              uint32_t dest_address, tcp_segment segment) {
  pthread_mutex_lock(stack->connection_pool.mutex);
  tcp_socket local_socket = {
      .ipv4_addr = dest_address,
      .port = segment.dest_port,
  };

  tcp_socket remote_socket = {
      .ipv4_addr = source_address,
      .port = segment.source_port,
  };

  tcp_connection *connection = tcp_connection_pool_find(
      stack->connection_pool, local_socket, remote_socket);

  if (connection) {
    printf("found connection, now lets process! flags: 0x%x\n", segment.flags);
  }

  pthread_mutex_unlock(stack->connection_pool.mutex);
}

bool verify_checksum(uint8_t *buffer, size_t header_length_in_32bit_words) {
  uint32_t csum = 0;
  checksum_update(&csum, buffer, header_length_in_32bit_words * 4);
  return checksum_finalize(&csum) == 0;
}

void *receive_datagrams(tcp_stack *stack) {
  while (!atomic_load(stack->destroyed)) {
    struct sockaddr_in remote_addr;
    socklen_t remote_addr_len;
    // TODO - can I just use recv instead?
    ssize_t bytes_received =
        recvfrom(stack->raw_socket.fd, stack->raw_socket.receive_buffer.buffer,
                 RAW_SOCKET_RECEIVE_BUFFER_LEN, 0,
                 (struct sockaddr *)(&remote_addr), &remote_addr_len);

    if (bytes_received == -1) {
      fprintf(stderr, "Failed to receive from raw socket: %s\n",
              strerror(errno));
      exit(1);
    }

    ip_datagram packet = parse_datagram((vec){
        .buffer = stack->raw_socket.receive_buffer.buffer,
        .len = (size_t)bytes_received,
    });

    // if (packet.source_address == REMOTE_IP) {
    //   printf("remote IP: %x, src: %x, dst: %x\n", REMOTE_IP,
    //          packet.source_address, packet.dest_address);
    // }

    // TODO - I guess this check is actually redundant, since I'm using
    // IPPROTO_TCP when creating the raw socket?
    if (packet.protocol == IPPROTO_TCP) {
      if (verify_checksum(stack->raw_socket.receive_buffer.buffer,
                          packet.internet_header_length)) {
        tcp_segment segment = parse_segment(packet.data_in_receive_buffer);
        process_incoming_segment(stack, packet.source_address,
                                 packet.dest_address, segment);
      } else {
        printf("checksum failed\n");
      }
    }
  }
  return NULL;
}
