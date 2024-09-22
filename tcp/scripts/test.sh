#!/usr/bin/env bash

set -eu pipefail

cleanup() {
  kill "$test_server_pid"
  kill "$my_server_pid"
  rm "$tmp_file"
}

assert_eq() {
  if [ "$1" != "$2" ]; then
    echo "Assertion failed: $1 != $2"
    exit 1
  fi
}

assert_packet_field() {
  local packet
  packet=$1

  local field
  field=$2

  local expected_val
  expected_val=$3

  local actual_val

  actual_val=$(echo $packet | jq -r ."$field")
  assert_eq "$actual_val" "$expected_val"
}

assert_tcp_flag_is_set() {
  local packet
  packet=$1

  local flag_name
  flag_name=$2

  local flag_val
  flag_val=$(echo "$packet_1" | jq -r '.["tcp.flags_tree"]["tcp.flags.'"$flag_name"\"']')

  if [ "$flag_val" != "1" ]; then
    echo "$flag_name" flag was not set 2>&1
    exit 1
  fi
}

trap cleanup 'EXIT'

tmp_file=$(mktemp)

echo "starting sniffer"
tshark -i any -f 'tcp and (port 3000 or port 3001)' -a 'duration:5' -T json >"$tmp_file" &
sniffer_pid=$!

echo "starting test server"
node ./scripts/test-server.js &
test_server_pid=$!
sleep 1
echo "starting my server"
make serve &
my_server_pid=$!

wait "$sniffer_pid"

# First, we expect to see a SYN being sent to my server
packet_1=$(cat "$tmp_file" | jq '.[0]._source.layers.tcp')
assert_packet_field "$packet_1" "[\"tcp.srcport\"]" "3001"
assert_packet_field "$packet_1" "[\"tcp.dstport\"]" "3000"
assert_packet_field "$packet_1" "[\"tcp.flags_tree\"][\"tcp.flags.syn\"]" "1"

# Then we expect a SYN ACK in reply
packet_2=$(cat "$tmp_file" | jq '.[1]._source.layers.tcp')
assert_packet_field "$packet_2" "[\"tcp.srcport\"]" "3000"
assert_packet_field "$packet_2" "[\"tcp.dstport\"]" "3001"
assert_packet_field "$packet_2" "[\"tcp.flags_tree\"][\"tcp.flags.syn\"]" "1"
assert_packet_field "$packet_2" "[\"tcp.flags_tree\"][\"tcp.flags.ack\"]" "1"
