#include "./checksum.h"
#include <arpa/inet.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void checksum_test_1() {
  uint32_t source_ip = htonl((192U << 24) | (168U << 16) | (174U << 8) | 128);
  uint32_t dest_ip = htonl((192U << 24) | (168U << 16) | (174U << 8) | 1);

  uint16_t protocol = 6;
  uint16_t data_len = 0x26;

  uint16_t pseudo_header[6] = {
      source_ip >> 16, source_ip & 0x0000FFFF, dest_ip >> 16, dest_ip & 0x0000FFFF, htons(protocol), htons(data_len),
  };

  uint32_t checksum = 0;

  checksum_update(&checksum, (void *)pseudo_header, 12);

  uint8_t data2[38] = {
      0x11, 0x5c, 0xdc, 0xba, 0x28, 0xd5, 0x41, 0xda, 0x64, 0xe8, 0x6a, 0x10, 0x80, 0x18, 0x01, 0xfe, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x01, 0x08, 0x0a, 0x5c, 0x86, 0xc6, 0xf8, 0xbd, 0x62, 0xe3, 0x6f, 0x6c, 0x69, 0x64, 0x6f, 0x72, 0x0a,
  };
  checksum_update(&checksum, (void *)data2, 38);
  assert(checksum_finalize(&checksum) == 0x67ea);
}

// static void checksum_test_2() {
//   uint32_t source_ip = (127 << 24) | 1;
//   uint32_t dest_ip = (127 << 24) | 1;
//   uint16_t data_len = 24;
//   uint8_t data[24] = {
//       0x0b, 0xb9, 0x0b, 0xb8, 0xde, 0xad, 0xbe, 0xef, 0x00, 0x00, 0x00, 0x00,
//       0x60, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x10, 0x00,
//   };
//   uint16_t checksum = compute_checksum(source_ip, dest_ip, data, data_len);
//   assert(checksum == 0xd6c9);
// }

void checksum_test() {
  checksum_test_1();
  // checksum_test_2();
}
