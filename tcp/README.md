# Design Notes

This is an implementation of TCP/IP.

##  Threading

Main thread

- listens for user actions are called e.g. OPEN, SEND, RECEIVE, CLOSE, STATUS, ABORT, FLUSH
  (for now the user actions are simply function calls within a main fn, but eventually this will become a daemon, accepting input from a cli?)

Receive thread

- raw sockets are read from...

Timeout thread

- tracks timeouts

Note that the state machines of the individual connections can be mutated by all three of the threads. The handling of certain actions/events may also result in the connection pool hashmap being mutated. We have a mutex on the connection pool which must be held while using (reading or writing) the connection pool in any way, including the individual connection state machines, or the connection pool hashmap.
There is also a mutex on the raw socket.

# Configuration

My custom implementation of TCP is written for linux. Of course, linux already has its own built-in TCP implementation. This means there could be confusion regarding whether incoming IP packets should be handled by the built-in TCP implementation of the operating system, or by the custom TCP implementation.

This could be resolved by limiting the custom TCP implementation to a subset of the available ports on the system and selectively blocking the operating system from handling incoming traffic on those ports with an `iptables` rule. But I would prefer to be able to use the full range of ports in my custom TCP implementation, so instead I create a distinct IP address on the machine for the sole use of the custom TCP implementation. The steps for this on debian are as follows:

```sh
# Create a new IP address on the machine. Your network interface may have a different name.
# The chosen IP address should be within your local network, should have the same subnet mask
# configured on your local router, and should be outside of the range of addresses that your
# local DHCP server is configured to assign to devices on your network. You'll need to run this as root.
echo 'source /etc/network/interfaces.d/*

source /etc/network/interfaces.d/*

# The loopback network interface
auto lo
iface lo inet loopback

# The primary network interface.
auto enp1s0
iface enp1s0 inet static
    address 192.168.111.222/24
    gateway 192.168.111.1

# Secondary network interface - used by my custom TCP implementation. We'll set up iptables
# to block any traffic incoming on this address, so that it's exclusively handled by our
# custom TCP implementation.
iface enp1s0 inet static
    address 192.168.111.221/24
    gateway 192.168.111.1
```

Normally, your primary address would probably be configured to be set up by DHCP, but in my experience this does not combine well with the secondary address being static; if one address is configured by DHCP and one is static, then most tools (e.g. curl, ping, and probably most other programs that involve any networking) will prefer to use the static one. In our case though, that would be the opposite of what we want; we want everything except our custom TCP implementation to use the primary address. Using static addresses in both cases simplifies this.

```
# Apply the changes from /etc/network/interfaces
systemctl restart networking

# Block the operating system from handling any traffic on the secondary IP address.
# We want to handle that traffic with our custom TCP implemenation instead!
iptables -A INPUT -p tcp -d 192.168.111.221 -j DROP

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


#  Running


To build the main `sockets` binary, run:

```
make bin/sockets
```

You can then run the binary. You'll need to be root, because this uses raw sockets.

```
./bin/sockets
```

To make life easier, you'll probably add the bin directory to your PATH by adding something like this to your preferred shell startup script (e.g. `~/.bsahrc` or `~/.zshrc`)

```
# Don't forget to replace /path/to/repo with the actual path to the repo on your machine!
export PATH="/path/to/repo/tcp/bin:$PATH"
```