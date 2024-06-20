#pragma once

typedef enum {
  CWR = 128,
  ECE = 64,
  URG = 32,
  ACK = 16,
  PSH = 8,
  RST = 4,
  SYN = 2,
  FIN = 1
} ip_datagram_type;
