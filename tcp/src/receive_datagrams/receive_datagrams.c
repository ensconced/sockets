
#include "../tcp_stack.h"
#include "../utils.h"
#include <netinet/in.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>

/*
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |Version|  IHL  |Type of Service|          Total Length         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         Identification        |Flags|      Fragment Offset    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Time to Live |    Protocol   |         Header Checksum       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                       Source Address                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Destination Address                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Options                    |    Padding    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

                    Example Internet Datagram Header
*/

#define HEADER_LENGTH_WITHOUT_OPTIONS_IN_WORDS 5

typedef struct ip_header {
  uint8_t version;
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
} ip_header;

// TODO - if an ip is fragmented, will the OS already have combined them by the
// time I receive it? I think not - so I should really do it myself...

ip_header parse_ip_datagram(uint8_t *receive_buffer) {
  // TODO - there is no bounds checking here!
  uint8_t *ptr = receive_buffer;
  uint8_t version_and_header_length;
  uint8_t type_of_service;
  uint16_t big_endian_total_length;
  uint16_t big_endian_identification;
  uint16_t flags_and_big_endian_fragment_offset;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t big_endian_header_checksum;
  uint32_t big_endian_source_address;
  uint32_t big_endian_dest_address;

  take_value(ptr, version_and_header_length);
  uint8_t header_length_in_words = version_and_header_length & 0x0F;

  take_value(ptr, type_of_service);
  take_value(ptr, big_endian_total_length);
  take_value(ptr, big_endian_identification);
  take_value(ptr, flags_and_big_endian_fragment_offset);
  take_value(ptr, ttl);
  take_value(ptr, protocol);
  take_value(ptr, big_endian_header_checksum);
  take_value(ptr, big_endian_source_address);
  take_value(ptr, big_endian_dest_address);

  size_t header_length_in_bytes = header_length_in_words * 4;
  size_t options_length_in_bytes =
      header_length_in_bytes - (HEADER_LENGTH_WITHOUT_OPTIONS_IN_WORDS * 4);
  vec options_in_receive_buffer = {
      .ptr = ptr,
      .len = options_length_in_bytes,
  };
  ptr += options_length_in_bytes;

  size_t data_length_in_bytes =
      ntohs(big_endian_total_length) - header_length_in_bytes;
  vec data_in_receive_buffer = {
      .ptr = ptr,
      .len = data_length_in_bytes,
  };

  return (ip_header){
      .version = version_and_header_length >> 4,
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

void *receive_datagrams(tcp_stack *stack) {
  while (!atomic_load(stack->destroyed)) {
    struct sockaddr remote_addr;
    socklen_t remote_addr_len;
    ssize_t bytes_received = recvfrom(
        stack->raw_socket.fd, stack->raw_socket.receive_buffer,
        RAW_SOCKET_RECEIVE_BUFFER_LEN, 0, &remote_addr, &remote_addr_len);

    ip_header ip_datagram = parse_ip_datagram(stack->raw_socket.receive_buffer);
    // TODO - check checksum - procedure is same as TCP checksum
    if (ip_datagram.version == 4 && ip_datagram.protocol == IPPROTO_TCP) {
      printf("received %zu bytes from addr %d.%d.%d.%d\n", bytes_received,
             ip_datagram.source_address >> 24,
             (ip_datagram.source_address << 8) >> 24,
             (ip_datagram.source_address << 16) >> 24,
             (ip_datagram.source_address << 24) >> 24);
      printf("\n");
    }
  }
  return NULL;
}
