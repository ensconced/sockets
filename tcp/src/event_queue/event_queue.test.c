#include "./event_queue.h"
#include "../error_handling/error_handling.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void event_queue_basic_test(void) {
  event_queue *q = event_queue_create();

  event_queue_enqueue(q, (event){.type = EVENT_SEGMENT_ARRIVES});
  event evt = event_queue_dequeue(q);
  assert(evt.type == EVENT_SEGMENT_ARRIVES);
  event_queue_destroy(q);
}

void event_queue_multiple_events_test(void) {
  event_queue *q = event_queue_create();

  event_queue_enqueue(q, (event){.type = EVENT_SEGMENT_ARRIVES});
  event_queue_enqueue(q, (event){.type = EVENT_ABORT});
  event_queue_enqueue(q, (event){.type = EVENT_CLOSE});

  event evt1 = event_queue_dequeue(q);
  event evt2 = event_queue_dequeue(q);
  event evt3 = event_queue_dequeue(q);
  assert(evt1.type == EVENT_SEGMENT_ARRIVES);
  assert(evt2.type == EVENT_ABORT);
  assert(evt3.type == EVENT_CLOSE);

  event_queue_destroy(q);
}

typedef struct thread_context {
  size_t start_val;
  event_queue *q;
} thread_context;

void *thread_entrypoint(void *arg) {
  thread_context *context = arg;

  // TODO - how can I attach numbers to the events, to be able to tell which is
  // which?? I think I can either make the queue generic, or add some properties
  // which I'll need anyway...
  event_queue_enqueue(context->q);

  free(context);
  return NULL;
}

void event_queue_multi_producer_single_consumer(void) {

  size_t thread_count = 10;
  size_t vals_per_thread = 10;
  event_queue *q = event_queue_create();

  pthread_t **threads =
      checked_malloc(sizeof(pthread_t *) * thread_count, "threads");
  for (size_t i = 0; i < thread_count; i++) {
    thread_context *context =
        checked_malloc(sizeof(thread_context), "thread context");
    *context = (thread_context){.start_val = i * vals_per_thread, .q = q};

    pthread_t *thread = checked_malloc(sizeof(pthread_t), "thread");
    int thread_create_error =
        pthread_create(thread, NULL, thread_entrypoint, context);
    if (thread_create_error) {
      fprintf(stderr, "Failed to create thread: %s\n",
              strerror(thread_create_error));
      exit(1);
    }

    threads[i] = thread;
  }

  for (size_t i = 0; i < thread_count; i++) {
    int join_error = pthread_join(*threads[i], NULL);
    if (join_error) {
      fprintf(stderr, "Error joining thread: %s\n", strerror(join_error));
      exit(1);
    }
    free(threads[i]);
  }

  // TODO - asserts...

  event_queue_destroy(q);
}

void event_queue_test(void) {
  event_queue_basic_test();
  event_queue_multiple_events_test();
  event_queue_multi_producer_single_consumer();
}
