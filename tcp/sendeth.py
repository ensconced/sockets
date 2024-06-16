from socket import *
def sendeth(src, dst, eth_type, payload, interface = "enp1s0"):
  s = socket(AF_PACKET, SOCK_RAW)
  s.bind((interface, 0))
  return s.send((dst + src + eth_type + payload))

if __name__ == "__main__":
  print("Sent %d-byte Ethernet packet on enp1s0" %
  sendeth(b'\x30\x9C\x23\x0B\x03\xAB',
          b'\xE4\x5F\x01\x77\x76\x38',
          b'\x08\x00',
          b'hello'))