#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX_RESPONSE_SIZE 1024

const int PORT = 9898;
const char *BIND_ADDR_STR = "0.0.0.0";
const char *RESPONSE_BODY =
    "<!DOCTYPE html>"
    "<head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />"
    "</head>"
    "<body>"
    "<h1>Joe's home page</h1>"
    "<p>Hi!</p>"
    "<p>This is my web page.</p>"
    "<p>It is served by a very very simple server that I wrote in plain C.</p>"
    "<p>You can see the source code on <a "
    "href=\"https://github.com/ensconced/sockets/tree/main\" "
    "target=\"_blank\">github</a>.</p>"
    "</body>";
char response[MAX_RESPONSE_SIZE] = "";

int main(void) {
  snprintf(response, MAX_RESPONSE_SIZE,
           "HTTP/1.1 200 OK\n"
           "Content-Type: text/html\n"
           "Content-Length: %ld\n"
           "\n"
           "%s\r\n",
           strlen(RESPONSE_BODY), RESPONSE_BODY);

  int socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_fd == -1) {
    fprintf(stderr, "Failed to create socket. %s", strerror(errno));
    exit(1);
  }

  struct in_addr bind_addr;
  int addr_err = inet_pton(AF_INET, BIND_ADDR_STR, &bind_addr);
  if (addr_err == 0) {
    fprintf(stderr, "Failed to parse bind address.");
    exit(1);
  }
  if (addr_err == -1) {
    fprintf(stderr, "System error while parsing bind address. %s",
            strerror(errno));
    exit(1);
  }

  struct sockaddr_in addr = {
      .sin_addr = bind_addr,
      .sin_family = AF_INET,
      .sin_port = htons(PORT),
  };

  int bind_err = bind(socket_fd, (struct sockaddr *)(&addr), sizeof(addr));
  if (bind_err) {
    fprintf(stderr, "Failed to bind socket. %s", strerror(errno));
    exit(1);
  }

  int listen_err = listen(socket_fd, SOMAXCONN);
  if (listen_err) {
    fprintf(stderr, "Failed to listen on socket. %s", strerror(errno));
    exit(1);
  }

  while (1) {
    struct sockaddr remote_addr;
    socklen_t remote_addr_len = sizeof(remote_addr);
    int acc_sock_fd = accept(socket_fd, &remote_addr, &remote_addr_len);
    if (acc_sock_fd == -1) {
      fprintf(stderr, "Failed to accept connection. %s", strerror(errno));
      continue;
    }

    // TODO - I'm actually sending the response here as soon as the TCP
    // connection is established. i.e. I'm not even waiting to receive an HTTP
    // request. If you examine the packets in wireshark you'll see that the
    // server actually sends the response before it received the request, which
    // is pretty weird. Presumably this would also break if the HTTP client
    // re-used the connection to send a second HTTP request - the second request
    // would never get a response.
    // Anyway, I should probably wait until I have a full request until I send
    // the response.
    send(acc_sock_fd, response, strlen(response), 0);
  }
}
