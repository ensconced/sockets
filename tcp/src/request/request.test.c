#include "./request.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>

void *thread_entrypoint(void *arg) {
  request *req = arg;
  request_resolve(req);
  return NULL;
}

void request_test() {
  request *req = request_create();
  assert(req->resolved == false);
  pthread_t thread;
  pthread_create(&thread, NULL, thread_entrypoint, req);
  request_block_until_resolved(req);
  assert(req->resolved == true);
  request_destroy(req);
}
