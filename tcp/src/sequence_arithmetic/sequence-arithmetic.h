#pragma once

#include <stdbool.h>
#include <stdint.h>

// returns boolean indicating whether `a` is less than `b` in the sense
// described in RFC 1982
bool less_than(uint32_t a, uint32_t b);
