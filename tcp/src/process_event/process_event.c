#include "./process_event.h"
#include "../tcp_stack/tcp_stack.h"

void process_event(tcp_stack *stack, event *evt) {
  switch (evt->type) {
  case EVENT_DESTROY_STACK: {
    tcp_stack_destroy(stack);
    break;
  }
  case EVENT_SEND: {
    // TODO - send data
    fprintf(stderr, "send event is not yet implemented\n");
    exit(1);
    break;
  }
  case EVENT_ABORT: {
    fprintf(stderr, "abort event is not yet implemented\n");
    exit(1);
  }
  case EVENT_CLOSE: {
    fprintf(stderr, "close event is not yet implemented\n");
    exit(1);
  }
  case EVENT_OPEN: {
    fprintf(stderr, "open event is not yet implemented\n");
    exit(1);
  }
  case EVENT_RECEIVE: {
    fprintf(stderr, "receive event is not yet implemented\n");
    exit(1);
  }
  case EVENT_SEGMENT_ARRIVES: {
    fprintf(stderr, "segment arrives event is not yet implemented\n");
    exit(1);
  }
  case EVENT_STATUS: {
    fprintf(stderr, "status event is not yet implemented\n");
    exit(1);
  }
  case EVENT_TIMEOUT: {
    fprintf(stderr, "timeout event is not yet implemented\n");
    exit(1);
  }
  }
  free(evt);
}
