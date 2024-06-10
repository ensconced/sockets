#pragma once

#include <stdint.h>

// TODO - just define these as strings and use the pton or whatever it is
// method...
#define LOCAL_IP                                                               \
  (((uint32_t)((uint32_t)192 << 24)) | ((uint32_t)((uint32_t)168 << 16)) |     \
   ((uint32_t)((uint32_t)178 << 8)) | (uint32_t)202)

#define REMOTE_IP                                                              \
  (((uint32_t)((uint32_t)93 << 24)) | ((uint32_t)((uint32_t)184 << 16)) |      \
   ((uint32_t)((uint32_t)215 << 8)) | (uint32_t)14)
