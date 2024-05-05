#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_CONNECTIONS 256

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

typedef enum { OPEN, CLOSE } user_action_type;

typedef struct {
  user_action_type type;
} user_action;

// threads:
// 1. main thread: accept and process incoming packets
// 2. handle timeouts
// 3. handle user actions
// each one needs to hold the mutex for the connections array to do anything
// with any of the connections

ip_datagram take_ip_datagram(void) {
  // TODO - I guess here take any datagrams tagged as being for the TCP
  // protocol?
}

user_action take_user_action(void) {
  // TODO
}

void process_syn(ip_datagram datagram, connection *connections) {
  // TODO
}

void process_ack(ip_datagram datagram, connection *connections) {
  // TODO
}

void process_fin(ip_datagram datagram, connection *connections) {
  // TODO
}

void process_datagram(ip_datagram datagram, connection *connections) {
  switch (datagram.type) {
  case SYN: {
    process_syn(datagram, connections);
    break;
  }
  case ACK: {
    process_ack(datagram, connections);
    break;
  }
  case FIN: {
    process_fin(datagram, connections);
    break;
  }
  }
}

void process_open(user_action action, connection *connections) {
  // TODO
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

int main(void) {
  connection *connections = malloc(sizeof(connection) * MAX_CONNECTIONS);
  if (connections == 0) {
    fprintf(stderr, "Failed to malloc connections\n");
    exit(1);
  }

  // TODO - create user action handling thread
  // TODO - create timeout handling thread

  handle_incoming_datagrams(connections);
}
