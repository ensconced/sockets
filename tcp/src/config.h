#pragma once

#include <stdint.h>

#define LOCAL_IP                                                               \
  ((uint32_t)((uint32_t)192 << 24)) | ((uint32_t)((uint32_t)168 << 16)) |      \
      ((uint32_t)((uint32_t)178 << 8)) | (uint32_t)202

#define REMOTE_IP                                                              \
  ((uint32_t)((uint32_t)192 << 24)) | ((uint32_t)((uint32_t)168 << 16)) |      \
      ((uint32_t)((uint32_t)178 << 8)) | (uint32_t)201
