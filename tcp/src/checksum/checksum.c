#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>

uint16_t compute_checksum(uint32_t source_ip, uint32_t dest_ip, uint8_t *data,
                          uint16_t data_len) {
  // The spec requires that we use 16 bit one's complement addition to compute
  // the checksum. An efficient way of acheiving the same result is by doing 32
  // bit addition (of the 16 bit chunks) and then at the end incorporating
  // anything that "overflowed" out of the bottom 16 bits back into the
  // bottom 16 bits...
  // Also note that endianness is not relevant to this calculation, since the
  // bottom byte of the 16bit sum overflows into the top byte, and the top byte
  // effectively overflows back into the bottom byte.
  // A good explanation can be found at
  // https://web.archive.org/web/20240203074319/https://locklessinc.com/articles/tcp_checksum/
  uint32_t acc = 0;

  // First, the ipv4 pseudo-header.
  acc += source_ip >> 16;
  acc += source_ip & 0x0000FFFF;
  acc += dest_ip >> 16;
  acc += dest_ip & 0x0000FFFF;
  acc += IPPROTO_TCP;
  acc += data_len;

  // And then the actual data of the segment.
  for (size_t i = 0; i < data_len; i += 2) {
    uint8_t msb = data[i];
    uint8_t lsb = i + 1 == data_len ? 0 : data[i + 1];
    acc += ((uint32_t)msb << 8) | (uint32_t)lsb;
  }

  // Now incorporate any overflows back into the lower 16 bits.
  uint32_t overflow = acc >> 16;
  acc = (acc & 0x0000FFFF) + overflow;
  // Adding the first one of overflows may actually have created another
  // overflow. We can simply repeat the same operation to account for this:
  overflow = acc >> 16;
  uint32_t with_second_overflow = (acc & 0x0000FFFF) + overflow;
  return ~(uint16_t)(with_second_overflow);
}
