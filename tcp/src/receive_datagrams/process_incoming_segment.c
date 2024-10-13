#include "../constants.h"
#include "../send_segment/send_segment.h"
#include "../tcp_stack/tcp_stack.h"
#include "./segment.h"

// typedef struct {
//   ip_datagram_type type;
// } ip_datagram;

// // 1. main thread: handles user actions
// // 2. packet processing thread: accepts and processes incoming packets.
// //    - continually takes packet as they arrive (this will be blocking).
// //    - if the packet matches an open connection:
// //      - if there is a receive request waiting, send the payload to that
// //      - else, push it to a big buffer so be used if a receive action occurs
// //    - else, discard it? or whatever the spec says to do...
// // 3. timeout processing thread: handles timeouts
// // each one needs to hold the mutex for the connections array to do anything
// // with any of the connections...

// ip_datagram take_ip_datagram() {
//   // TODO - I guess here take any datagrams tagged as being for the TCP
//   // protocol?
// }

// void process_syn(ip_datagram datagram, connection *connection) {
//   // TODO
// }

// void process_ack(ip_datagram datagram, connection *connection) {
//   // TODO
// }

// void process_fin(ip_datagram datagram, connection *connection) {
//   // TODO
// }

// connection *find_datagram_connection(ip_datagram datagram) {
//   // TODO
//   // A natural way to think about processing incoming segments is to imagine
//   // that they are first tested for proper sequence number (i.e., that their
//   // contents lie in the range of the expected "receive window" in the
//   sequence
//   // number space) and then that they are generally queued and processed in
//   // sequence number order.
// }

void process_incoming_segment(tcp_stack *stack, uint32_t source_address, uint32_t dest_address, tcp_segment segment) {
  pthread_mutex_lock(stack->connection_pool.mutex);
  internal_tcp_socket local_socket = {
      .host_order_ipv4_addr = dest_address,
      .host_order_port = segment.dest_port,
  };

  internal_tcp_socket remote_socket = {
      .host_order_ipv4_addr = source_address,
      .host_order_port = segment.source_port,
  };

  tcp_connection *connection = tcp_connection_pool_find(stack->connection_pool, local_socket, remote_socket);

  if (connection != NULL) {
    // TODO - for now we're just assuming that we already have all the preceding
    // segments - i.e. there are no gaps.
    connection->receive_next = segment.sequence_number + 1;

    switch (connection->state) {
    case SYN_SENT: {
      if ((segment.flags & SYN) && (segment.flags & ACK)) {
        printf("received SYN/ACK\n");
        tcp_send_segment(stack, connection, NULL, 0, ACK);
        connection->state = ESTABLISHED;
      }
      break;
    }
    case ESTABLISHED: {
      char *data = (char *)segment.data.buffer;
      printf("omg actual data? %s\n", data);
      break;
    }
    default: {
      printf("unexpected segment...\n");
    }
    }
  }

  pthread_mutex_unlock(stack->connection_pool.mutex);
}
