#pragma once

#include <stdbool.h>

/*
This queue is threadsafe provided it is used in a "single-consumer, multiple
producer" manner. i.e. `event_queue_enqueue` can be called by many
different threads, but `event_queue_dequeue` must only be called from a single
thread.
*/
typedef struct event_queue event_queue;

typedef enum event_type {
  EVENT_OPEN,
  EVENT_SEND,
  EVENT_RECEIVE,
  EVENT_CLOSE,
  EVENT_ABORT,
  EVENT_STATUS,
  EVENT_SEGMENT_ARRIVES,
  EVENT_TIMEOUT,
} event_type;

// typedef struct event_open_details {
// } event_open_details;

// typedef struct event_send_details {
// } event_send_details;

// typedef struct event_receive_details {
// } event_receive_details;

// typedef struct event_close_details {
// } event_close_details;

// typedef struct event_abort_details {
// } event_abort_details;

// typedef struct event_status_details {
// } event_status_details;

// typedef struct event_segment_arrives_details {
// } event_segment_arrives_details;

// typedef struct event_timeout_details {
// } event_timeout_details;

typedef struct event {
  event_type type;
  // union {
  //   event_open_details open;
  //   event_send_details send;
  //   event_receive_details receive;
  //   event_close_details close;
  //   event_abort_details abort;
  //   event_status_details status;
  //   event_segment_arrives_details segment_arrives;
  //   event_timeout_details timeout;
  // } details;
} event;

event_queue *event_queue_create(void);

void event_queue_destroy(event_queue *q);

void event_queue_enqueue(event_queue *q, event evt);

event event_queue_dequeue(event_queue *q);
