typedef enum {
  SYN,
  ACK,
  FIN,
} ip_datagram_type;

typedef struct {
  ip_datagram_type type;
} ip_datagram;

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

void handle_incoming_datagrams(connection *connections) {
  while (1) {
    ip_datagram datagram = take_ip_datagram();
    process_datagram(datagram, connections);
  }
}