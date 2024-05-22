This is my implementation of TCP. It runs on linux. Of course, linux already has its own built-in TCP implementation. This means there could be confusion regarding whether incoming IP packets should be handled by the built-in TCP implementation of the operating system, or by the custom TCP implementation.

This could be resolved by limiting the custom TCP implementation to a subset of the available ports on the system, and selectively blocking the operating system from handling incoming traffic on those ports with an `iptables` rule. But I would prefer to be able to use the full range of ports in my custom TCP implementation, and I acheive this by creating a distinct IP address on the machine for use by the custom TCP implementation:

```sh
# Create a new IP address on the machine. Your network interface may have a different name.
# The chosen IP address should be within your local network, should have the same subnet mask
# configured on your local router, and should be outside of the range of addresses that your
# local DHCP server is configured to assign to devices on your network. You'll need to run this as root.
echo '
# Secondary network interface - used by my custom TCP implementation
auto enp2s0:0
iface enp2s0:0 inet static
    address 192.168.178.201
    netmask 255.255.255.0
    gateway 192.168.178.1' >> /etc/network/interfaces


systemctl restart networking

# Block the operating system from handling any traffic on the new IP address.
# We want to handle that traffic with our custom TCP implemenation instead!
iptables -A INPUT -d 192.168.178.201 -j DROP

# iptables rules are just kept in memory, so for them to persist over reboots, there are some more steps.
# First, write your rules to a file.
iptables-save > /etc/iptables.rules
# then create a new service file for systemd
echo '[Unit]
Description=Restore iptables rules

[Service]
Type=oneshot
ExecStart=/sbin/iptables-restore /etc/iptables.rules

[Install]
WantedBy=multi-user.target' > /etc/systemd/system/iptables-restore.service

systemctl enable iptables-restore
```


When starting wireshark, use

interface: any
capture filter: tcp and (port 3000 or port 3001)
