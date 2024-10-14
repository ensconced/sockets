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
