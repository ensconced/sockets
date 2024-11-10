#include "./request.h"
#include "../error_handling/error_handling.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void request_block_until_resolved(request *req) {
  while (!req->resolved) {
    int lock_error = pthread_mutex_lock(req->mutex);
    if (lock_error) {
      fprintf(stderr, "Failed to lock request mutex: %s\n", strerror(lock_error));
      exit(1);
    }

    int wait_error = pthread_cond_wait(req->cond, req->mutex);
    if (wait_error) {
      fprintf(stderr, "Failed to wait on condition: %s\n", strerror(wait_error));
      exit(1);
    }
    int unlock_err = pthread_mutex_unlock(req->mutex);
    if (unlock_err) {
      fprintf(stderr, "Failed to unlock mutex: %s\n", strerror(unlock_err));
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
    fprintf(stderr, "Failed to initialise cond: %s\n", strerror(cond_init_err));
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
    fprintf(stderr, "Failed to destroy request mutex: %s\n", strerror(destroy_mutex_err));
    exit(1);
  }
  free(req->mutex);

  int cond_mutex_err = pthread_cond_destroy(req->cond);
  if (cond_mutex_err) {
    fprintf(stderr, "Failed to destroy request cond: %s\n", strerror(cond_mutex_err));
    exit(1);
  }
  free(req->cond);
  free(req);
}

void request_resolve(request *req) {
  int lock_err = pthread_mutex_lock(req->mutex);
  if (lock_err) {
    fprintf(stderr, "Failed to lock request mutex: %s\n", strerror(lock_err));
    exit(1);
  }

  req->resolved = true;

  int signal_err = pthread_cond_signal(req->cond);
  if (signal_err) {
    fprintf(stderr, "Failed to signal request cond: %s\n", strerror(signal_err));
    exit(1);
  }

  int unlock_err = pthread_mutex_unlock(req->mutex);
  if (unlock_err) {
    fprintf(stderr, "Failed to unlock request mutex: %s\n", strerror(unlock_err));
    exit(1);
  }
}
