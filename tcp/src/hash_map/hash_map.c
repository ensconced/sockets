#include "./hash_map.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../error_handling/error_handling.h"

#define FNV_PRIME 0x01000193
#define FNV_OFFSET_BASIS 0x811c9dc5
#define INITIAL_BUFFER_LEN 32
#define MAX_LOAD_FACTOR 0.5

typedef struct hash_map {
  buffer_entry *buffer;
  uint32_t buffer_len;
  uint32_t occupied_count;
  uint32_t tombstone_count;
} hash_map;

typedef struct hash_map_iterator {
  hash_map *hm;
  size_t idx;
} hash_map_iterator;

static uint32_t fnv1a_hash(void *data, size_t data_len) {
  uint32_t hash = FNV_OFFSET_BASIS;
  for (size_t i = 0; i < data_len; i++) {
    hash = hash ^ ((uint8_t *)data)[i];
    hash = hash * FNV_PRIME;
  }
  return hash;
}

hash_map *hash_map_create() {
  hash_map *hm = checked_malloc(sizeof(hash_map), "hash_map");
  buffer_entry *buffer = checked_calloc(INITIAL_BUFFER_LEN, sizeof(buffer_entry), "buffer_entry");
  *hm = (hash_map){
      .buffer = buffer,
      .buffer_len = INITIAL_BUFFER_LEN,
      .occupied_count = 0,
  };
  return hm;
}

void hash_map_destroy(hash_map *hm) {
  free(hm->buffer);
  free(hm);
}

void hash_map_insert_key_value(hash_map *hm, void *key, size_t key_len, void *value) {
  uint32_t idx = fnv1a_hash(key, key_len) % hm->buffer_len;
  while (hm->buffer[idx].type == occupied) {
    idx = (idx + 1) % hm->buffer_len;
  }
  if (hm->buffer[idx].type == tombstone) {
    hm->tombstone_count--;
  }
  hm->occupied_count++;
  hm->buffer[idx] = (buffer_entry){
      .type = occupied,
      .value = value,
      .key = key,
      .key_len = key_len,
  };
}

static void hash_map_rehash(hash_map *hm, uint32_t new_buffer_len) {
  buffer_entry *new_buffer = checked_calloc(new_buffer_len, sizeof(buffer_entry), "buffer_entry");
  hash_map new_hash_map = {
      .buffer = new_buffer,
      .buffer_len = new_buffer_len,
      .tombstone_count = 0,
      .occupied_count = hm->occupied_count,
  };

  for (uint32_t i = 0; i < hm->buffer_len; i++) {
    buffer_entry entry = hm->buffer[i];
    if (entry.type == occupied) {
      hash_map_insert_key_value(&new_hash_map, entry.key, entry.key_len, entry.value);
    }
  }

  free(hm->buffer);

  *hm = new_hash_map;
}

static void hash_map_rehash_if_necessary(hash_map *hm) {
  // What would the load factor be if we added one more entry, assuming it
  // didn't take a tombstone place?
  float new_load_factor = (float)(hm->occupied_count + hm->tombstone_count + 1) / (float)(hm->buffer_len);

  if (new_load_factor >= MAX_LOAD_FACTOR) {
    // We need to re-hash. But should we increase the buffer length? That
    // depends on whether we would be exceeding the max load factor even when
    // all the tombstones are removed.
    float new_load_factor_excluding_tombstones = (float)(hm->occupied_count + 1) / (float)(hm->buffer_len);

    uint32_t new_buffer_len =
        new_load_factor_excluding_tombstones >= MAX_LOAD_FACTOR ? hm->buffer_len * 2 : hm->buffer_len;

    hash_map_rehash(hm, new_buffer_len);
  }
}

static buffer_entry *hash_map_find_entry(hash_map *hm, void *key, size_t key_len) {
  uint32_t idx = fnv1a_hash(key, key_len) % hm->buffer_len;
  // The enforcement of the max load factor means that there are always some
  // empty entries and therefore this loop will always terminate.
  while (true) {
    buffer_entry *entry = &hm->buffer[idx];
    if (entry->type == empty) {
      return NULL;
    } else if (entry->type == occupied) {
      if (entry->key_len == key_len && memcmp(entry->key, key, key_len) == 0) {
        return entry;
      }
    }
    idx = (idx + 1) % hm->buffer_len;
  }
}

void hash_map_insert(hash_map *hm, void *key, size_t key_len, void *value) {
  hash_map_rehash_if_necessary(hm);
  hash_map_insert_key_value(hm, key, key_len, value);
}

void *hash_map_get(hash_map *hm, void *key, size_t key_len) {
  buffer_entry *entry = hash_map_find_entry(hm, key, key_len);
  if (entry == NULL) {
    return NULL;
  }
  return entry->value;
}

void hash_map_delete(hash_map *hm, void *key, size_t key_len) {
  buffer_entry *entry = hash_map_find_entry(hm, key, key_len);
  if (entry == NULL) {
    return;
  }
  entry->type = tombstone;
}

// TODO - probably shouldn't exposure buffer_entry type...
buffer_entry *hash_map_iterator_take(hash_map_iterator *iterator) {
  while (iterator->idx < iterator->hm->buffer_len && iterator->hm->buffer[iterator->idx].type != occupied) {
    iterator->idx++;
  }
  if (iterator->idx == iterator->hm->buffer_len)
    return NULL;
  return &iterator->hm->buffer[iterator->idx++];
}

hash_map_iterator *hash_map_iterator_create(hash_map *hm) {
  hash_map_iterator *iter = checked_malloc(sizeof(hash_map_iterator), "hash_map_iterator");
  *iter = (hash_map_iterator){
      .hm = hm,
      .idx = 0,
  };
  return iter;
}

void hash_map_iterator_destroy(hash_map_iterator *iter) { free(iter); }
