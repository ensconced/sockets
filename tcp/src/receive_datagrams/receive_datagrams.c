#include "../buffer_state/buffer_state.h"
#include "../checksum/checksum.h"
#include "../config.h"
#include "../tcp_stack/tcp_stack.h"
#include "./process_incoming_segment.h"
#include "./segment.h"
#include <arpa/inet.h>
#include <errno.h>
#include <linux/if_packet.h>
#include <netinet/in.h>
#include <stdatomic.h>
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
  buffer options_in_receive_buffer;
  buffer data_in_receive_buffer;
} ip_datagram;

// TODO - if an ip is fragmented, will the OS already have combined them by the
// time I receive it? I think not - so I should really do it myself...
static ip_datagram parse_datagram(buffer datagram_vec) {
  buffer_state reader = {.buffer = datagram_vec, .ptr = datagram_vec.data};

  uint8_t version_and_header_length = take_uint8_t(&reader);
  uint8_t header_length_in_words = version_and_header_length & 0x0F;

  uint8_t type_of_service = take_uint8_t(&reader);
  uint16_t network_order_total_length = take_uint16_t(&reader);
  uint16_t network_order_identification = take_uint16_t(&reader);
  uint16_t flags_and_network_order_fragment_offset = take_uint16_t(&reader);
  uint8_t ttl = take_uint8_t(&reader);
  uint8_t protocol = take_uint8_t(&reader);
  uint16_t network_order_header_checksum = take_uint16_t(&reader);
  uint32_t network_order_source_address = take_uint32_t(&reader);
  uint32_t network_order_dest_address = take_uint32_t(&reader);

  size_t header_length_in_bytes = header_length_in_words * 4;
  size_t options_length_in_bytes = header_length_in_bytes - (HEADER_LENGTH_WITHOUT_OPTIONS_IN_WORDS * 4);
  buffer options_in_receive_buffer = {
      .data = reader.ptr,
      .size_bytes = options_length_in_bytes,
  };
  reader.ptr = (char *)reader.ptr + options_length_in_bytes;

  size_t data_length_in_bytes = ntohs(network_order_total_length) - header_length_in_bytes;
  buffer data_in_receive_buffer = {
      .data = reader.ptr,
      .size_bytes = data_length_in_bytes,
  };

  return (ip_datagram){
      .version = version_and_header_length >> 4,
      .internet_header_length = header_length_in_words,
      .type_of_service = type_of_service,
      .identification = ntohs(network_order_identification),
      .flags = flags_and_network_order_fragment_offset >> 13,
      .fragment_offset = ntohs(flags_and_network_order_fragment_offset & 0x1FFF),
      .ttl = ttl,
      .protocol = protocol,
      .header_checksum = ntohs(network_order_header_checksum),
      .source_address = ntohl(network_order_source_address),
      .dest_address = ntohl(network_order_dest_address),
      .options_in_receive_buffer = options_in_receive_buffer,
      .data_in_receive_buffer = data_in_receive_buffer,
  };
}

static tcp_segment parse_segment(buffer segment_vec) {
  buffer_state reader = {.buffer = segment_vec, .ptr = segment_vec.data};

  uint16_t network_order_source_port = take_uint16_t(&reader);
  uint16_t network_order_dest_port = take_uint16_t(&reader);
  uint32_t network_order_sequence_number = take_uint32_t(&reader);
  uint32_t network_order_acknowledgement_number = take_uint32_t(&reader);
  uint8_t data_offset_and_reserved_space = take_uint8_t(&reader);
  uint8_t flags = take_uint8_t(&reader);
  uint16_t network_order_window = take_uint16_t(&reader);
  uint16_t network_order_checksum = take_uint16_t(&reader);
  uint16_t network_order_urgent_pointer = take_uint16_t(&reader);
  uint8_t *options_ptr = reader.ptr;
  uint8_t data_offset_in_words = data_offset_and_reserved_space >> 4;
  uint8_t *data_ptr = (uint8_t *)segment_vec.data + data_offset_in_words * 4;
  size_t options_len = (size_t)(data_ptr - options_ptr);
  size_t data_len = segment_vec.size_bytes - (size_t)(data_ptr - (uint8_t *)segment_vec.data);

  return (tcp_segment){
      .source_port = ntohs(network_order_source_port),
      .dest_port = ntohs(network_order_dest_port),
      .sequence_number = ntohl(network_order_sequence_number),
      .acknowledgement_number = ntohl(network_order_acknowledgement_number),
      .flags = flags,
      .window = ntohs(network_order_window),
      .checksum = ntohs(network_order_checksum),
      .urgent_pointer = ntohs(network_order_urgent_pointer),
      .options =
          {
              .data = options_ptr,
              .size_bytes = options_len,
          },
      .data =
          {
              .data = data_ptr,
              .size_bytes = data_len,
          },
  };
}

static bool verify_checksum(uint8_t *buffer, size_t header_length_in_32bit_words) {
  uint32_t csum = 0;
  checksum_update(&csum, buffer, header_length_in_32bit_words * 4);
  return checksum_finalize(&csum) == 0;
}

void *receive_datagrams(tcp_stack *stack) {
  while (!atomic_load(stack->destroyed)) {
    ssize_t bytes_received =
        recv(stack->raw_socket.fd, stack->raw_socket.receive_buffer.data, RAW_SOCKET_RECEIVE_BUFFER_LEN, 0);

    if (bytes_received == -1) {
      fprintf(stderr, "Failed to receive from raw socket: %s\n", strerror(errno));
      exit(1);
    }

    ip_datagram packet = parse_datagram((buffer){
        .data = stack->raw_socket.receive_buffer.data,
        .size_bytes = (size_t)bytes_received,
    });

    if (packet.protocol == IPPROTO_TCP &&
        verify_checksum(stack->raw_socket.receive_buffer.data, packet.internet_header_length)) {
      tcp_segment segment = parse_segment(packet.data_in_receive_buffer);
      process_incoming_segment(stack, packet.source_address, packet.dest_address, segment);
    }
  }
  return NULL;
}
