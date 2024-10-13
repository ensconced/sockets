void open() {
  /*
  3.10.1. OPEN Call
CLOSED STATE (i.e., TCB does not exist)

Create a new transmission control block (TCB) to hold connection state information. Fill in local socket identifier,
remote socket, Diffserv field, security/compartment, and user timeout information. Note that some parts of the remote
socket may be unspecified in a passive OPEN and are to be filled in by the parameters of the incoming SYN segment.
Verify the security and Diffserv value requested are allowed for this user, if not, return "error: Diffserv value not
allowed" or "error: security/compartment not allowed". If passive, enter the LISTEN state and return. If active and the
remote socket is unspecified, return "error: remote socket unspecified"; if active and the remote socket is specified,
issue a SYN segment. An initial send sequence number (ISS) is selected. A SYN segment of the form <SEQ=ISS><CTL=SYN> is
sent. Set SND.UNA to ISS, SND.NXT to ISS+1, enter SYN-SENT state, and return. If the caller does not have access to the
local socket specified, return "error: connection illegal for this process". If there is no room to create a new
connection, return "error: insufficient resources". LISTEN STATE

If the OPEN call is active and the remote socket is specified, then change the connection from passive to active, select
an ISS. Send a SYN segment, set SND.UNA to ISS, SND.NXT to ISS+1. Enter SYN-SENT state. Data associated with SEND may be
sent with SYN segment or queued for transmission after entering ESTABLISHED state. The urgent bit if requested in the
command must be sent with the data segments sent as a result of this command. If there is no room to queue the request,
respond with "error: insufficient resources". If the remote socket was not specified, then return "error: remote socket
unspecified". SYN-SENT STATE

SYN-RECEIVED STATE

ESTABLISHED STATE

FIN-WAIT-1 STATE

FIN-WAIT-2 STATE

CLOSE-WAIT STATE

CLOSING STATE

LAST-ACK STATE

TIME-WAIT STATE

Return "error: connection already exists".


  */
}
