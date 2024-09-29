#pragma once

#include "../tcp_stack.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct event_send_details {
  uint8_t *bytes;
  size_t byte_count;
} event_send_details;

typedef enum event_type {
  EVENT_OPEN,
  EVENT_SEND,
  EVENT_RECEIVE,
  EVENT_CLOSE,
  EVENT_ABORT,
  EVENT_STATUS,
  EVENT_SEGMENT_ARRIVES,
  EVENT_TIMEOUT,
  EVENT_DESTROY_STACK,
} event_type;

// typedef struct event_open_details {
// } event_open_details;

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
  union {
    // event_open_details open;
    event_send_details send;
    // event_receive_details receive;
    // event_close_details close;
    // event_abort_details abort;
    // event_status_details status;
    // event_segment_arrives_details segment_arrives;
    // event_timeout_details timeout;
  } details;
} event;

void process_event(tcp_stack *stack, event *evt);
