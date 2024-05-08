#define push_value(ptr, value)                                                 \
  do {                                                                         \
    memcpy(ptr, &value, sizeof(value));                                        \
    ptr += sizeof(value);                                                      \
  } while (0)
