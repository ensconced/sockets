#include <stdint.h>

#define HALF_UINT32_RANGE 1 << 30

bool less_than(uint32_t a, uint32_t b) {
  return (b > a && b - a < HALF_UINT32_RANGE) || (a > b && a - b > HALF_UINT32_RANGE);
}
