#include "./event_queue.h"
#include "../error_handling/error_handling.h"
#include "assert.h"
#include "stdlib.h"

void event_queue_basic_test(void) {
  event_queue *q = event_queue_create();

  event *evt = checked_malloc(sizeof(event), "event");
  // dequeues nothing initially because we haven't enqueued anything yet
  assert(!event_queue_dequeue(q, evt));

  // successfully dequeues after enqueuing.
  event_queue_enqueue(q, (event){.type = EVENT_SEGMENT_ARRIVES});
  assert(event_queue_dequeue(q, evt));
  assert(evt->type == EVENT_SEGMENT_ARRIVES);

  // now it should be empty again
  assert(!event_queue_dequeue(q, evt));

  event_queue_destroy(q);
  free(evt);
}

void event_queue_multiple_events_test(void) {
  event_queue *q = event_queue_create();

  event *evt = checked_malloc(sizeof(event), "event");
  assert(!event_queue_dequeue(q, evt));

  event_queue_enqueue(q, (event){.type = EVENT_SEGMENT_ARRIVES});
  event_queue_enqueue(q, (event){.type = EVENT_ABORT});
  event_queue_enqueue(q, (event){.type = EVENT_CLOSE});

  assert(event_queue_dequeue(q, evt));
  assert(evt->type == EVENT_SEGMENT_ARRIVES);
  assert(event_queue_dequeue(q, evt));
  assert(evt->type == EVENT_ABORT);
  assert(event_queue_dequeue(q, evt));
  assert(evt->type == EVENT_CLOSE);
  assert(!event_queue_dequeue(q, evt));

  event_queue_destroy(q);
  free(evt);
}

void event_queue_test(void) {
  event_queue_basic_test();
  event_queue_multiple_events_test();
}
