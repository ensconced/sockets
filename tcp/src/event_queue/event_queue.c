#include "./event_queue.h"
#include "../error_handling/error_handling.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct event_queue_node {
  event evt;
  struct event_queue_node *next;
} event_queue_node;

struct event_queue {
  event_queue_node *back;
  event_queue_node *front;
  pthread_mutex_t *mutex;
  pthread_cond_t *cond;
};

void event_queue_lock(event_queue *q) {
  int lock_error = pthread_mutex_lock(q->mutex);
  if (lock_error) {
    fprintf(stderr, "Failed to lock event queue: %s\n", strerror(lock_error));
    exit(1);
  }
}

void event_queue_unlock(event_queue *q) {
  int unlock_error = pthread_mutex_unlock(q->mutex);
  if (unlock_error) {
    fprintf(stderr, "Failed to unlock event queue: %s\n",
            strerror(unlock_error));
    exit(1);
  }
}

event_queue *event_queue_create(void) {
  event_queue *q = checked_malloc(sizeof(event_queue), "event queue");
  pthread_mutex_t *mutex =
      checked_malloc(sizeof(pthread_mutex_t), "event queue mutex");
  int mutex_init_error = pthread_mutex_init(mutex, NULL);
  if (mutex_init_error) {
    fprintf(stderr, "Failed to init event queue mutex: %s\n",
            strerror(mutex_init_error));
    exit(1);
  }
  pthread_cond_t *cond =
      checked_malloc(sizeof(pthread_cond_t), "event queue cond");
  int cond_init_error = pthread_cond_init(cond, NULL);
  if (cond_init_error) {
    fprintf(stderr, "Failed to init event queue cond: %s\n",
            strerror(cond_init_error));
    exit(1);
  }
  *q = (event_queue){.front = NULL, .back = NULL, .mutex = mutex, .cond = cond};
  return q;
}

void event_queue_destroy(event_queue *q) {
  event_queue_node *current_node = q->front;
  while (current_node) {
    free(current_node);
    current_node = current_node->next;
  }
  int mutex_destroy_error = pthread_mutex_destroy(q->mutex);
  if (mutex_destroy_error) {
    fprintf(stderr, "Failed to destroy mutex: %s\n",
            strerror(mutex_destroy_error));
    exit(1);
  }
  free(q->mutex);
  free(q->cond);
  free(q);
}

void event_queue_enqueue(event_queue *q, event evt) {
  event_queue_lock(q);
  event_queue_node *new_node =
      checked_malloc(sizeof(event_queue_node), "event event_queue node");
  *new_node = (event_queue_node){.evt = evt, .next = NULL};
  event_queue_node *prev_back = q->back;
  q->back = new_node;
  if (prev_back) {
    prev_back->next = new_node;
  } else {
    q->front = new_node;
  }
  pthread_cond_signal(q->cond);
  event_queue_unlock(q);
}

event event_queue_dequeue(event_queue *q) {
  event_queue_lock(q);
  event_queue_node *front_node;
  while (!(front_node = q->front)) {
    pthread_cond_wait(q->cond, q->mutex);
  }

  q->front = front_node->next;
  if (!q->front) {
    q->back = NULL;
  }
  event evt = front_node->evt;
  free(front_node);
  event_queue_unlock(q);
  return evt;
}
