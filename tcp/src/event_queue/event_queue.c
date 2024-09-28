#include "./event_queue.h"
#include "../error_handling/error_handling.h"

typedef struct event_queue_node {
  event evt;
  event_queue_node *next;
} event_queue_node;

struct queue {
  event_queue_node *back;
  event_queue_node *front;
};

queue *queue_create() {
  queue *q = checked_malloc(sizeof(queue), "queue");
  *q = (queue){.front = NULL, .back = NULL};
  return q;
}

void queue_destroy(queue *q) {
  event_queue_node *current_node = q->front;
  while (current_node) {
    free(current_node);
    current_node = current_node->next;
  }
  free(q);
}

void queue_enqueue(queue *q, event evt) {
  event_queue_node *prev_back = q->back;
  event_queue_node *new_node =
      checked_malloc(sizeof(event_queue_node), "event queue node");
  *new_node = (event_queue_node){.evt = evt, .next = NULL};
  q->back = new_node;
  if (q->front == NULL) {
    q->front = new_node;
  }
}

void queue_dequeue(queue *q, event *evt_result) {
  // TODO
  // - shift the front pointer along
  // - if new front pointer would be null that means it's now empty so we should
  //   set back pointer to null as well
  // - free the released node
  // - set evt_result
}
