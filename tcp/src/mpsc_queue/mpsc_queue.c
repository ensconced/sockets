#include "./mpsc_queue.h"
#include "../error_handling/error_handling.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct mpsc_queue_node {
  void *evt;
  struct mpsc_queue_node *next;
} mpsc_queue_node;

struct mpsc_queue {
  mpsc_queue_node *back;
  mpsc_queue_node *front;
  pthread_mutex_t *mutex;
  pthread_cond_t *cond;
};

void mpsc_queue_lock(mpsc_queue *q) {
  int lock_error = pthread_mutex_lock(q->mutex);
  if (lock_error) {
    fprintf(stderr, "Failed to lock mpsc queue: %s\n", strerror(lock_error));
    exit(1);
  }
}

void mpsc_queue_unlock(mpsc_queue *q) {
  int unlock_error = pthread_mutex_unlock(q->mutex);
  if (unlock_error) {
    fprintf(stderr, "Failed to unlock mpsc queue: %s\n", strerror(unlock_error));
    exit(1);
  }
}

mpsc_queue *mpsc_queue_create() {
  mpsc_queue *q = checked_malloc(sizeof(mpsc_queue), "mpsc queue");
  pthread_mutex_t *mutex = checked_malloc(sizeof(pthread_mutex_t), "mpsc queue mutex");
  int mutex_init_error = pthread_mutex_init(mutex, NULL);
  if (mutex_init_error) {
    fprintf(stderr, "Failed to init mpsc queue mutex: %s\n", strerror(mutex_init_error));
    exit(1);
  }
  pthread_cond_t *cond = checked_malloc(sizeof(pthread_cond_t), "mpsc queue cond");
  int cond_init_error = pthread_cond_init(cond, NULL);
  if (cond_init_error) {
    fprintf(stderr, "Failed to init mpsc queue cond: %s\n", strerror(cond_init_error));
    exit(1);
  }
  *q = (mpsc_queue){.front = NULL, .back = NULL, .mutex = mutex, .cond = cond};
  return q;
}

void mpsc_queue_destroy(mpsc_queue *q) {
  mpsc_queue_node *current_node = q->front;
  while (current_node) {
    mpsc_queue_node *next = current_node->next;
    free(current_node->evt);
    free(current_node);
    current_node = next;
  }
  int mutex_destroy_error = pthread_mutex_destroy(q->mutex);
  if (mutex_destroy_error) {
    fprintf(stderr, "Failed to destroy mpsc queue mutex: %s\n", strerror(mutex_destroy_error));
    exit(1);
  }
  free(q->mutex);
  free(q->cond);
  free(q);
}

void mpsc_queue_enqueue(mpsc_queue *q, void *evt) {
  mpsc_queue_lock(q);
  mpsc_queue_node *new_node = checked_malloc(sizeof(mpsc_queue_node), "mpsc queue node");
  *new_node = (mpsc_queue_node){.evt = evt, .next = NULL};
  mpsc_queue_node *prev_back = q->back;
  q->back = new_node;
  if (prev_back) {
    prev_back->next = new_node;
  } else {
    q->front = new_node;
  }
  pthread_cond_signal(q->cond);
  mpsc_queue_unlock(q);
}

void *mpsc_queue_dequeue(mpsc_queue *q) {
  mpsc_queue_lock(q);
  mpsc_queue_node *front_node;
  while (!(front_node = q->front)) {
    pthread_cond_wait(q->cond, q->mutex);
  }

  q->front = front_node->next;
  if (!q->front) {
    q->back = NULL;
  }
  void *evt = front_node->evt;
  free(front_node);
  mpsc_queue_unlock(q);
  return evt;
}
