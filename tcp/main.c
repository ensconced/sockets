#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_CONNECTIONS 256

typedef struct {
  connection *connections;
} tcp_stack;

typedef enum {
  LISTEN,
  SYN_RECEIVED,
  SYN_SENT,
  ESTABLISHED,
  FIN_WAIT_1,
  FIN_WAIT_2,
  CLOSE_WAIT,
  CLOSING,
  LAST_ACK,
  TIME_WAIT
} connection_state;
typedef enum { PASSIVE, ACTIVE } connection_mode;
typedef struct {
  connection_state state;
  connection_mode mode;
} connection;

typedef enum {
  SYN,
  ACK,
  FIN,
} ip_datagram_type;

typedef struct {
  ip_datagram_type type;
} ip_datagram;

typedef enum { OPEN, SEND, RECEIVE, CLOSE, ABORT, STATUS } user_action_type;

typedef struct {
  user_action_type type;
} user_action;

typedef enum {
  USER_TIMEOUT,
  RETRANSMISSION_TIMEOUT,
  TIME_WAIT_TIMEOUT
} timeout_type;

// 1. main thread: handles user actions
// 2. packet processing thread: accepts and processes incoming packets.
//    - continually takes packet as they arrive (this will be blocking).
//    - if the packet matches an open connection:
//      - if there is a receive request waiting, send the payload to that
//      - else, push it to a big buffer so be used if a receive action occurs
//    - else, discard it? or whatever the spec says to do...
// 3. timeout processing thread: handles timeouts
// each one needs to hold the mutex for the connections array to do anything
// with any of the connections...

ip_datagram take_ip_datagram(void) {
  // TODO - I guess here take any datagrams tagged as being for the TCP
  // protocol?
}

user_action take_user_action(void) {
  // TODO
}

void process_syn(ip_datagram datagram, connection *connection) {
  // TODO
}

void process_ack(ip_datagram datagram, connection *connection) {
  // TODO
}

void process_fin(ip_datagram datagram, connection *connection) {
  // TODO
}

connection *find_datagram_connection(ip_datagram datagram) {
  // TODO
  // A natural way to think about processing incoming segments is to imagine
  // that they are first tested for proper sequence number (i.e., that their
  // contents lie in the range of the expected "receive window" in the sequence
  // number space) and then that they are generally queued and processed in
  // sequence number order.
}

void process_datagram(ip_datagram datagram, connection *connections) {
  connection *conn = find_datagram_connection();
  switch (datagram.type) {
  case SYN: {
    process_syn(datagram, conn);
    break;
  }
  case ACK: {
    process_ack(datagram, conn);
    break;
  }
  case FIN: {
    process_fin(datagram, conn);
    break;
  }
  }
}

void process_open(user_action action, connection *connections) {
  // 1. create new connection
  // 2. fill in fields on connection - some will be left blank until later if
  //    mode is passive
  // 3. obtain mutex
  // 4. add connection to connections array
  // 5. release mutex
}

void process_close(user_action action, connection *connections) {
  // TODO
}

void process_user_action(user_action action, connection *connections) {
  switch (action.type) {
  case OPEN: {
    process_open(action, connections);
    break;
  }
  case CLOSE: {
    process_close(action, connections);
    break;
  }
  }
}

void handle_user_actions(connection *connections) {
  while (1) {
    user_action action = take_user_action();
    process_user_action(action, connections);
  }
}

void handle_incoming_datagrams(connection *connections) {
  while (1) {
    ip_datagram datagram = take_ip_datagram();
    process_datagram(datagram, connections);
  }
}

void handle_timeouts(connection *connections) {
  while (1) {
    // TODO -
  }
}

tcp_stack tcp_init(void) {
  connection *connections = malloc(sizeof(connection) * MAX_CONNECTIONS);
  if (connections == 0) {
    fprintf(stderr, "Failed to malloc connections\n");
    exit(1);
  }

  pthread_t incoming_datagram_handler_thread_id;
  if (pthread_create(&incoming_datagram_handler_thread_id, NULL,
                     handle_incoming_datagrams, connections) != 0) {
    fprintf(stderr, "Failed to create datagram handling thread\n");
    exit(1);
  };

  pthread_t timeout_handler_thread_id;
  if (pthread_create(&timeout_handler_thread_id, NULL, handle_timeouts,
                     connections) != 0) {
    fprintf(stderr, "Failed to create timeout handling thread\n");
    exit(1);
  };

  return (tcp_stack){.connections = connections};
}

int main(void) {
  tcp_stack stack = tcp_init();
  connection *conn = open_passive_connection(&stack);
}
