#pragma once

#define CWR 128
#define ECE 64
#define URG 32
#define ACK 16
#define PSH 8
#define RST 4
#define SYN 2
#define FIN 1

void tcp_send_segment(tcp_stack *stack, tcp_connection *conn, uint8_t *payload,
                      size_t payload_len, uint8_t flags, uint32_t seq_number,
                      uint32_t ack_number);
