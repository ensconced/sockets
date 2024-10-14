#!/usr/bin/env bash

# perhaps an e2e test could look something like this eventually...

set -eu

sockets start --detach
sockets open --local 192.168.111.222:1234 --remote 8.8.8.8:80
printf "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n" | sockets send --local 192.168.111.222:1234 --remote 8.8.8.8:80
data=$(sockets receive --local 192.168.111.222:1234 --remote 8.8.8.8:80)
# now make some assertion on the data...
sockets stop
