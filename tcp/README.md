This is my custom implementation of TCP.


# Configuration

My custom implementation of TCP is written for linux. Of course, linux already has its own built-in TCP implementation. This means there could be confusion regarding whether incoming IP packets should be handled by the built-in TCP implementation of the operating system, or by the custom TCP implementation.

This could be resolved by limiting the custom TCP implementation to a subset of the available ports on the system and selectively blocking the operating system from handling incoming traffic on those ports with an `iptables` rule. But I would prefer to be able to use the full range of ports in my custom TCP implementation, so instead I create a distinct IP address on the machine for the sole use of the custom TCP implementation. The steps for this on debian are as follows:

```sh
# Create a new IP address on the machine. Your network interface may have a different name.
# The chosen IP address should be within your local network, should have the same subnet mask
# configured on your local router, and should be outside of the range of addresses that your
# local DHCP server is configured to assign to devices on your network. You'll need to run this as root.
echo 'source /etc/network/interfaces.d/*

# The loopback network interface
auto lo
iface lo inet loopback

# The primary network interface.
auto enp2s0
iface enp2s0 inet static
    address 192.168.178.201
    netmask 255.255.255.0
    gateway 192.168.178.1

# Secondary network interface - used by my custom TCP implementation. We'll set up iptables
# to block any traffic incoming on this address, so that it's exclusively handled by our
# custom TCP implementation.
auto enp2s0:0
iface enp2s0:0 inet static
    address 192.168.178.202
    netmask 255.255.255.0
    gateway 192.168.178.1' > /etc/network/interfaces
```

Normally, your primary address would probably be configured to be set up by DHCP, but in my experience this does not combine well with the secondary address being static; if one address is configured by DHCP and one is static, then most tools (e.g. curl, ping, and probably most other programs that involve any networking) will prefer to use the static one. In our case though, that would be the opposite of what we want; we want everything except our custom TCP implementation to use the primary address. Using static addresses in both cases simplifies this.

```
# Apply the changes from /etc/network/interfaces
systemctl restart networking

# Block the operating system from handling any traffic on the secondary IP address.
# We want to handle that traffic with our custom TCP implemenation instead!
iptables -A INPUT -d 192.168.178.202 -j DROP

# The iptables rules are just kept in memory. To get them to persist over reboots, we'll create a systemd service.
# First, write your rules to a file.
iptables-save > /etc/iptables.rules
# and then create a new service file for systemd...
echo '[Unit]
Description=Restore iptables rules

[Service]
Type=oneshot
ExecStart=/sbin/iptables-restore /etc/iptables.rules

[Install]
WantedBy=multi-user.target' > /etc/systemd/system/iptables-restore.service

systemctl enable iptables-restore
```
