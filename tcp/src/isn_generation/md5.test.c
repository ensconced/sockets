#include "./md5.h"
#include <assert.h>
#include <openssl/evp.h>

uint32_t local_ipv4 = 2062917599;
uint16_t local_port = 9457;
uint32_t remote_ipv4 = 1179665456;
uint16_t remote_port = 50785;

unsigned char test_key[] = {0x81, 0x6c, 0x66, 0x75, 0x16, 0xf2, 0xe5, 0xc,
                            0xf1, 0x5f, 0x21, 0xe9, 0xcb, 0xab, 0x5a, 0xf2};
unsigned int test_key_len = 16;

uint8_t expected_hash = {0x8f, 0x8b, 0x70, 0xc1, 0x0b, 0x26, 0xa0, 0x18,
                         0x61, 0x45, 0x5f, 0x63, 0x5c, 0xc4, 0x8b, 0x20};

void md5_test(void) {
  EVP_MD *md5_algorithm = EVP_MD_fetch(NULL, "MD5", "provider=default");
  assert(md5_algorithm != NULL);
  md5_result result =
      md5(md5_algorithm, local_ipv4, local_port, remote_ipv4, remote_port);
  assert(memcmp(&result.hash, &expected_hash, 16) == 0);
}
