#include "./hash_map.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void hash_map_test_basic_functionality(void) {
  hash_map *hm = hash_map_create();

  char *key1 = "france";
  char *value1 = "paris";
  hash_map_insert(hm, key1, strlen(key1), value1);

  char *key2 = "germany";
  char *value2 = "berlin";
  hash_map_insert(hm, key2, strlen(key2), value2);

  char *key3 = "united kingdom";
  char *value3 = "london";
  hash_map_insert(hm, key3, strlen(key3), value3);

  char *retrieved_value1 = hash_map_get(hm, "france", strlen("france"));
  assert(retrieved_value1 != NULL);
  assert(strcmp(retrieved_value1, "paris") == 0);

  char *retrieved_value2 = hash_map_get(hm, "germany", strlen("germany"));
  assert(retrieved_value2 != NULL);
  assert(strcmp(retrieved_value2, "berlin") == 0);

  char *retrieved_value3 =
      hash_map_get(hm, "united kingdom", strlen("united kingdom"));
  assert(retrieved_value3 != NULL);
  assert(strcmp(retrieved_value3, "london") == 0);

  hash_map_delete(hm, "france", strlen("france"));
  assert(hash_map_get(hm, "france", strlen("france")) == NULL);

  hash_map_delete(hm, "united kingdom", strlen("united kingdom"));
  assert(hash_map_get(hm, "united kingdom", strlen("united kingdom")) == NULL);

  hash_map_delete(hm, "germany", strlen("germany"));
  assert(hash_map_get(hm, "germany", strlen("germany")) == NULL);

  hash_map_destroy(hm);
}

void hash_map_test_high_load(void) {
  hash_map *hm = hash_map_create();
  size_t count = 1000000;
  uint32_t *keys = malloc(count * sizeof(uint32_t));
  uint32_t *values = malloc(count * sizeof(uint32_t));
  for (uint32_t i = 0; i < count; i++) {
    keys[i] = i;
    values[i] = i * 2;
    hash_map_insert(hm, &keys[i], sizeof(uint32_t), &values[i]);
  }

  for (uint32_t i = 0; i < count; i++) {
    uint32_t val = *(uint32_t *)hash_map_get(hm, &keys[i], sizeof(uint32_t));
    assert(val == keys[i] * 2);
  }

  free(keys);
  free(values);
  hash_map_destroy(hm);
}

void hash_map_test_iteration(void) {
  hash_map *hm = hash_map_create();
  size_t count = 1000;
  uint32_t *keys = malloc(count * sizeof(uint32_t));
  uint32_t *values = malloc(count * sizeof(uint32_t));
  for (uint32_t i = 0; i < count; i++) {
    keys[i] = i;
    values[i] = i * 2;
    hash_map_insert(hm, &keys[i], sizeof(uint32_t), &values[i]);
  }

  uint32_t *iterator_values = malloc(count * sizeof(uint32_t));
  hash_map_iterator *iterator = hash_map_iterator_create(hm);

  uint32_t *val;
  while ((val = hash_map_iterator_take(iterator)) != NULL) {
    iterator_values[*val / 2] = *val;
  }
  assert(memcmp(values, iterator_values, count * sizeof(uint32_t)) == 0);

  free(keys);
  free(values);
  free(iterator_values);
  hash_map_iterator_destroy(iterator);
  hash_map_destroy(hm);
}

void hash_map_test(void) {
  hash_map_test_basic_functionality();
  hash_map_test_high_load();
  hash_map_test_iteration();
}
