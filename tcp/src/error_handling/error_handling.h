#pragma once

#include <stddef.h>

void *checked_malloc(size_t size, char *id);
void *checked_calloc(size_t count, size_t size, char *id);
