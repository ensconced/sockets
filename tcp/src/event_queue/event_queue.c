#include "./event_queue.h"
#include "../error_handling/error_handling.h"
#include "stdbool.h"
#include "stdlib.h"

typedef struct event_queue_node {
  event evt;
  struct event_queue_node *next;
} event_queue_node;

struct event_queue {
  event_queue_node *back;
  event_queue_node *front;
};

event_queue *event_queue_create(void) {
  event_queue *q = checked_malloc(sizeof(event_queue), "event_queue");
  *q = (event_queue){.front = NULL, .back = NULL};
  return q;
}

void event_queue_destroy(event_queue *q) {
  event_queue_node *current_node = q->front;
  while (current_node) {
    free(current_node);
    current_node = current_node->next;
  }
  free(q);
}

void event_queue_enqueue(event_queue *q, event evt) {
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
}

bool event_queue_dequeue(event_queue *q, event *evt_result) {
  event_queue_node *front_node = q->front;
  if (front_node) {
    q->front = front_node->next;
    if (!q->front) {
      q->back = NULL;
    }
    *evt_result = front_node->evt;
    free(front_node);
    return true;
  }
  return false;
}
