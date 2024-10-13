#include "./mpsc_queue.h"
#include "../error_handling/error_handling.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mpsc_queue_basic_test() {
  mpsc_queue *q = mpsc_queue_create();

  int val = 1;
  mpsc_queue_enqueue(q, &val);
  assert(*(int *)mpsc_queue_dequeue(q) == 1);
  mpsc_queue_destroy(q);
}

void mpsc_queue_multiple_events_test() {
  mpsc_queue *q = mpsc_queue_create();

  int val1 = 1;
  int val2 = 2;
  int val3 = 3;

  mpsc_queue_enqueue(q, &val1);
  mpsc_queue_enqueue(q, &val2);
  mpsc_queue_enqueue(q, &val3);

  assert(*(int *)mpsc_queue_dequeue(q) == 1);
  assert(*(int *)mpsc_queue_dequeue(q) == 2);
  assert(*(int *)mpsc_queue_dequeue(q) == 3);

  mpsc_queue_destroy(q);
}

typedef struct thread_context {
  int start_val;
  int end_val;
  mpsc_queue *q;
} thread_context;

void *thread_entrypoint(void *arg) {
  thread_context *context = arg;

  for (int i = context->start_val; i < context->end_val; i++) {
    int *val = checked_malloc(sizeof(int), "mpsc test value");
    *val = i;
    mpsc_queue_enqueue(context->q, val);
  }

  free(context);
  return NULL;
}

int cmp(const void *a, const void *b) { return *(int *)a - *(int *)b; }

void mpsc_queue_multithreaded_test() {

  int thread_count = 10;
  int vals_per_thread = 1000;
  mpsc_queue *q = mpsc_queue_create();

  pthread_t **threads = checked_malloc(sizeof(pthread_t *) * (size_t)thread_count, "threads");
  for (int i = 0; i < thread_count; i++) {
    thread_context *context = checked_malloc(sizeof(thread_context), "thread context");
    int start_val = i * vals_per_thread;
    int end_val = start_val + vals_per_thread;
    *context = (thread_context){.start_val = start_val, .end_val = end_val, .q = q};

    pthread_t *thread = checked_malloc(sizeof(pthread_t), "thread");
    int thread_create_error = pthread_create(thread, NULL, thread_entrypoint, context);
    if (thread_create_error) {
      fprintf(stderr, "Failed to create thread: %s\n", strerror(thread_create_error));
      exit(1);
    }

    threads[i] = thread;
  }

  for (int i = 0; i < thread_count; i++) {
    int join_error = pthread_join(*threads[i], NULL);
    if (join_error) {
      fprintf(stderr, "Error joining thread: %s\n", strerror(join_error));
      exit(1);
    }
    free(threads[i]);
  }
  free(threads);

  int expected_vals = thread_count * vals_per_thread;
  int *queue_contents = checked_malloc(sizeof(int) * (size_t)expected_vals, "queue contents");
  for (int i = 0; i < expected_vals; i++) {
    int *val = mpsc_queue_dequeue(q);
    queue_contents[i] = *val;
    free(val);
  }

  qsort(queue_contents, (size_t)expected_vals, sizeof(int), cmp);
  for (int i = 0; i < expected_vals; i++) {
    assert(queue_contents[i] == i);
  }

  free(queue_contents);

  mpsc_queue_destroy(q);
}

void mpsc_queue_test() {
  mpsc_queue_basic_test();
  mpsc_queue_multiple_events_test();
  mpsc_queue_multithreaded_test();
}
