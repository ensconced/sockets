#pragma once

#include <stdint.h>

uint16_t compute_checksum(uint32_t source_ip, uint32_t dest_ip, uint8_t *data,
                          uint16_t data_len);
