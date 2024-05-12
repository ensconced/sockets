#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct hash_map hash_map;
typedef struct hash_map_iterator hash_map_iterator;

hash_map *hash_map_create(void);
void hash_map_destroy(hash_map *hm);
void hash_map_insert(hash_map *hm, void *key, size_t key_len, void *value);
void *hash_map_get(hash_map *hm, void *key, size_t key_len);
void hash_map_delete(hash_map *hm, void *key, size_t key_len);

hash_map_iterator *hash_map_iterator_create(hash_map *hm);
void *hash_map_iterator_current(hash_map_iterator *iterator);
void *hash_map_iterator_next(hash_map_iterator *iterator);
bool hash_map_iterator_done(hash_map_iterator *iterator);
