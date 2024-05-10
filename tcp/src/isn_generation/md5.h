#pragma once

#include "./secret_key.h"
#include <openssl/evp.h>
#include <stdint.h>

typedef struct {
  uint8_t hash[16];
} md5_result;

md5_result md5(EVP_MD *md5_algorithm, uint32_t local_ipv4_addr,
               uint16_t local_port, uint32_t remote_ipv4_addr,
               uint16_t remote_port);
