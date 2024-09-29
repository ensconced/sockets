#include "./event_queue.h"
#include "../error_handling/error_handling.h"
#include <assert.h>
#include <stdlib.h>

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

void event_queue_test(void) {
  event_queue_basic_test();
  event_queue_multiple_events_test();
}
