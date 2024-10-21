#include "./request.h"
#include "./error_handling/error_handling.h"
#include <pthread.h>
#include <stdio.h>

void request_block_until_resolved(request *req) {
  while (!req->resolved) {
    int lock_error = pthread_mutex_lock(req->mutex);
    if (lock_error) {
      fprintf(stderr, "Failed to lock request mutex\n");
      exit(1);
    }

    int wait_error = pthread_cond_wait(req->cond, req->mutex);
    if (wait_error) {
      fprintf(stderr, "Failed to wait on condition\n");
      exit(1);
    }
  }
}

request *request_create() {
  pthread_mutex_t *mutex = checked_malloc(sizeof(pthread_mutex_t), "request mutex");
  pthread_cond_t *cond = checked_malloc(sizeof(pthread_cond_t), "request cond");
  request *req = checked_malloc(sizeof(request), "request");

  pthread_mutex_init(mutex, NULL);

  int cond_init_err = pthread_cond_init(cond, NULL);
  if (cond_init_err) {
    fprintf(stderr, "Failed to initialise cond\n");
    exit(1);
  }

  *req = (request){
      .mutex = mutex,
      .cond = cond,
      .resolved = false,
  };
  return req;
}

void request_destroy(request *req) {
  int destroy_mutex_err = pthread_mutex_destroy(req->mutex);
  if (destroy_mutex_err) {
    fprint(stderr, "Failed to destroy mutex\n");
    exit(1);
  }
  free(req->mutex);

  int cond_mutex_err = pthread_cond_destroy(req->cond);
  if (cond_mutex_err) {
    fprint(stderr, "Failed to destroy cond\n");
    exit(1);
  }
  free(req->cond);
  free(req);
}

void request_resolve(request *req) {
  int lock_err = pthread_mutex_lock(req->mutex);
  if (lock_err) {
    fprint(stderr, "Failed to lock mutex\n");
    exit(1);
  }

  req->resolved = true;

  int signal_err = pthread_cond_signal(req->cond);
  if (signal_err) {
    fprint(stderr, "Failed to signal cond\n");
    exit(1);
  }
}