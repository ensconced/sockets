#pragma once

#include <pthread.h>

typedef struct request {
  pthread_mutex_t *mutex;
  pthread_cond_t *cond;
  bool resolved;
} request;

void request_block_until_resolved(request *req);
request *request_create();
void request_destroy(request *req);
void request_resolve(request *req);
