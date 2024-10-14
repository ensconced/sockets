/*
3.10.1. OPEN Call
CLOSED STATE (i.e., TCB does not exist)

Create a new transmission control block (TCB) to hold connection state information. Fill in local socket identifier,
remote socket, user timeout information. Note that some parts of the remote
socket may be unspecified in a passive OPEN and are to be filled in by the parameters of the incoming SYN segment.

If passive, enter the LISTEN state and return. If active and the
remote socket is unspecified, return "error: remote socket unspecified"; if active and the remote socket is specified,
issue a SYN segment. An initial send sequence number (ISS) is selected. A SYN segment of the form <SEQ=ISS><CTL=SYN> is
sent. Set SND.UNA to ISS, SND.NXT to ISS+1, enter SYN-SENT state, and return. If the caller does not have access to the
local socket specified, return "error: connection illegal for this process". If there is no room to create a new
connection, return "error: insufficient resources". LISTEN STATE

If the OPEN call is active and the remote socket is specified, then change the connection from passive to active, select
an ISS. Send a SYN segment, set SND.UNA to ISS, SND.NXT to ISS+1. Enter SYN-SENT state. Data associated with SEND may be
sent with SYN segment or queued for transmission after entering ESTABLISHED state. The urgent bit if requested in the
command must be sent with the data segments sent as a result of this command. If there is no room to queue the request,
respond with "error: insufficient resources". If the remote socket was not specified, then return "error: remote socket
unspecified". SYN-SENT STATE

SYN-RECEIVED STATE

ESTABLISHED STATE

FIN-WAIT-1 STATE

FIN-WAIT-2 STATE

CLOSE-WAIT STATE

CLOSING STATE

LAST-ACK STATE

TIME-WAIT STATE

Return "error: connection already exists".


*/

#include "../../error_handling/error_handling.h"
#include "../../tcp_socket.h"
#include "../../tcp_stack/tcp_stack.h"
#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct parsed_options {
  internal_tcp_socket local;
  internal_tcp_socket remote;
} parsed_options;

static const char optstring[] = "l:r:";

static const struct option longopts[] = {
    {
        .name = "local",
        .has_arg = required_argument,
        .flag = NULL,
        .val = 'l',
    },
    {
        .name = "remote",
        .has_arg = required_argument,
        .flag = NULL,
        .val = 'r',
    },
    // sentinel value required by getopt_long to signify end of array
    {},
};

static uint32_t host_order_ipv4_addr(char *addr) {
  uint32_t network_order_addr;
  switch (inet_pton(AF_INET, addr, &network_order_addr)) {
  case 1: {
    return ntohl(network_order_addr);
  }
  case 0: {
    fprintf(stderr, "Address is invalid: %s\n", addr);
    exit(1);
  }
  case -1: {
    fprintf(stderr, "Failed to process provided address %s: %s\n", addr, strerror(errno));
    exit(1);
  }
  default: {
    fprintf(stderr, "Unexpected error processing provided address: %s\n", addr);
    exit(1);
  }
  }
}

static uint16_t host_order_port(char *port) {
  unsigned long ul = strtoul(port, NULL, 10);
  return (uint16_t)(ul);
}

static internal_tcp_socket parse_tcp_socket(char *option) {
  char *arg = checked_malloc((strlen(option) + 1) * sizeof(char), "socket option");
  strcpy(arg, option);
  char *delimiter = strchr(arg, ':');
  if (delimiter == NULL) {
    fprintf(stderr, "Expected socket option to be of format <addr>:<port>\n");
    exit(1);
  }
  *delimiter = '\0';

  internal_tcp_socket result = {
      .host_order_ipv4_addr = host_order_ipv4_addr(arg),
      .host_order_port = host_order_port(delimiter + 1),
  };
  free(arg);
  return result;
}

static parsed_options parse_options(int argc, char **argv) {
  parsed_options options = {};
  int opt = 0;
  while ((opt = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
    switch (opt) {
    case 'l': {
      options.local = parse_tcp_socket(optarg);
      break;
    }
    case 'r': {
      options.remote = parse_tcp_socket(optarg);
      break;
    }
    default: {
      fprintf(stderr, "Unrecognized option\n");
      exit(1);
    }
    }
  }

  return options;
}

int handle_open_command(int argc, char **argv) {
  parsed_options options = parse_options(argc, argv);

  printf("local addr: %u, local port: %hu, remote addr: %u, remote port: %hu\n", options.local.host_order_ipv4_addr,
         options.local.host_order_port, options.remote.host_order_ipv4_addr, options.remote.host_order_port);

  return 0;
}
