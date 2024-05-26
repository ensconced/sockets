#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>

// The ipv4 and TCP specs require that we use 16 bit one's complement addition
// to compute checksums. An efficient way of acheiving the same result is by
// doing 32 bit addition (of the 16 bit chunks) and then at the end
// incorporating anything that "overflowed" out of the bottom 16 bits back into
// the bottom 16 bits... Also note that endianness is not relevant to this
// calculation, since the bottom byte of the 16bit sum overflows into the top
// byte, and the top byte effectively overflows back into the bottom byte. A
// good explanation can be found at
// https://web.archive.org/web/20240203074319/https://locklessinc.com/articles/tcp_checksum/

void checksum_update(uint32_t *csum, uint8_t *data, size_t data_len) {
  for (size_t i = 0; i < data_len; i += 2) {
    uint8_t msb = data[i];
    uint8_t lsb = i + 1 == data_len ? 0 : data[i + 1];
    *csum += ((uint32_t)msb << 8) | (uint32_t)lsb;
  }
}

uint16_t checksum_finalize(uint32_t *csum) {
  // Now incorporate any overflows back into the lower 16 bits.
  uint32_t overflow = *csum >> 16;
  *csum = (*csum & 0x0000FFFF) + overflow;
  // Adding the first one of overflows may actually have created another
  // overflow. We can simply repeat the same operation to account for this:
  overflow = *csum >> 16;
  uint32_t with_second_overflow = (*csum & 0x0000FFFF) + overflow;
  return ~(uint16_t)(with_second_overflow);
}
