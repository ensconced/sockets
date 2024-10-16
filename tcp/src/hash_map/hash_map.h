#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct hash_map hash_map;
typedef struct hash_map_iterator hash_map_iterator;

// TODO - shouldn't expose these types - it's leaking internal details like the
// tombstone stuff...
typedef enum buffer_entry_type {
  // empty has to be zero so that we can use calloc to initialise the buffer
  empty,
  occupied,
  tombstone,
} buffer_entry_type;

typedef struct buffer_entry {
  buffer_entry_type type;
  void *key;
  size_t key_len;
  void *value;
} buffer_entry;

hash_map *hash_map_create();
void hash_map_destroy(hash_map *hm);
void hash_map_insert(hash_map *hm, void *key, size_t key_len, void *value);
void *hash_map_get(hash_map *hm, void *key, size_t key_len);
void hash_map_delete(hash_map *hm, void *key, size_t key_len);

hash_map_iterator *hash_map_iterator_create(hash_map *hm);

buffer_entry *hash_map_iterator_take(hash_map_iterator *iterator);
void hash_map_iterator_destroy(hash_map_iterator *iter);
