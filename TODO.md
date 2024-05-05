# TODO

### TCP

- implement TCP
- success condition is a program that:
  - opens a single passive connection to serve my website
  - can be used to serve a single HTTP request/response
  - then the connection closes...

### HTTP

- build http server on top of TCP implementation
- basically needs to ensure that at least one TCP connection is always in LISTEN state
- should actually parse request and send appropriate response (not just send response immediately on connecting!)

### HTTPS

- generate self-signed certificate and implement TLS using that
- install certbot and use that to generate actual CA signed certs instead
