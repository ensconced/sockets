#!/usr/bin/env bash

# Create a new IP address on the machine. Your network interface may have a different name.
# The chosen IP address should be within your local network, should have the same subnet mask
# configured on your local router, and should be outside of the range of addresses that your
# local DHCP server is configured to assign to devices on your network. You'll need to run this as root.
echo "source /etc/network/interfaces.d/*

# The loopback network interface
auto lo
iface lo inet loopback

# The primary network interface.
auto enp1s0
iface enp1s0 inet static
    address 192.168.178.202/24
    gateway 192.168.178.1

iface enp1s0 inet static
    address 192.168.178.201/24" > /etc/network/interfaces