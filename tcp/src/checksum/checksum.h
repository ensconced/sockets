#pragma once

#include <stdint.h>
#include <stdlib.h>

void checksum_update(uint32_t *csum, uint8_t *data, size_t data_len);
uint16_t checksum_finalize(uint32_t *csum);
