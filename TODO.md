# TODO

- check checksums using wireshark validation
- WHY am I not getting a SYN/ACK back?
- debug - not finding SYN ACK - try wireshark alongside lldb?...
- ...it seems that it's actually iptables that's the problem here??? it's blocking my incoming packets...I thought it wasn't meant to affect raw sockets though??
- maybe try using QUEUE/NFQUEUE as suggested in the comments here https://serverfault.com/questions/387263/disable-kernel-processing-of-tcp-packets-for-raw-socket - you then have to explicitly read from the queue in your code, but that's maybe not so bad? eventually we will move over to using AF_PACKET anyway, which hopefully will actually bypass iptables rules?
- implement process_incoming_segment
- get to the established state
- strerror is not threadsafe - come up with better error handling solution
- switch over to using lower-level packet sockets (https://linux.die.net/man/7/packet) (initially with SOCK_DGRAM, and then eventually even lower-level SOCK_RAW...) - requires implementing ARP myself, I think?
- eventually we'll be able to remove all the config from /etc/network/interfaces, because we'll be implementing it all ourselves (including arp etc)

### TCP

- implement TCP
- provide API for simply creating a single TCP connection, that follows the state machine flow
- success condition is a program that:
  - opens a single passive connection to serve my website
  - can be used to serve a single HTTP request/response
  - then the connection closes...

i.e. I want to be able to use it like this:

```c
int main(void) {
  // initialise the tcp stack. this will initialise some data structures and
  // spawn a couple of helper threads - one for receiving IP packets, and one for
  // handling timeouts.
  // NB if you call tcp_init a second time it should be an error.
  tcp_stack stack = tcp_init();
  // obtain a pointer to a new connection. NB the state of this connection gets
  // updated by another thread behind the scenes. Each connection will have
  // a mutex as a field on the struct.
  connection* conn = tcp_open_passive(&stack);
  // NB most of these methods will first need to obtain the connection
  // mutex, before checking the current state of the connection,
  // taking any actions, changing the state etc., and then releasing
  // the mutex again
  tcp_send(conn, /* */);
  // I guess this needs to be "callback style" - i.e. should take a function
  // pointer to be called with the results, instead of blocking.
  // How would you implement that though? I guess what it does internally
  // is actually just push that function pointer (or a struct containing it)
  // to an array so that it can be called when the data is actually received?
  // So this means your function pointer will actually end up getting called
  // within a different thread...which I guess is fine.
  tcp_receive(conn, /* */, handle_data);
  tcp_status(conn, /* */);
  tcp_close(conn, /* */);
  tcp_abort(conn, /* */);
}
```

### HTTP

- build http server on top of TCP implementation
- basically needs to ensure that at least one TCP connection is always in LISTEN state
- this can be acheived in a simple way?

  - calloc an array of TCBs, of length MAX_CONNECTIONS
  - ???

- should actually parse request and send appropriate response (not just send response immediately on connecting!)

### HTTPS

- generate self-signed certificate and implement TLS using that
- install certbot and use that to generate actual CA signed certs instead
