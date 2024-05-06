
int main(void) {
  tcp_stack stack = tcp_init();
  // TODO - need to do endianness conversion for IP address & port
  tcp_connection *conn =
      tcp_open_passive(&stack, (tcp_socket){.ipv4_addr = 0, .port = 9898});
  tcp_send(conn, "hello there...");
}
