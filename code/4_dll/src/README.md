# Data Link Layer
  <details> <summary> arp.c </summary>
  </details>
  <details> <summary> ping.c </summary>
  </details>
  <details> <summary> record_route.c </summary>
  </details>
  <details> <summary> statistics.c </summary>
  </details>
  <details> <summary> tcp.c </summary>
  </details>
  <details> <summary> time_exceeded.c </summary>
  </details>
  <details> <summary> traceroute.c </summary>
  </details>
  <details> <summary> unreachable_dest.c </summary>
  </details>
  <details> <summary> utility.c </summary>
  </details>
  </details>
  <details> <summary> inverse_ping.c </summary>
  </details>
  <details> <summary> inverse_ping.c </summary>
  Program that analyses an ECHO request, sent by a remote node, and replies to it with an ECHO reply.
  The program follows these steps:
  1. It waits for an ECHO Request
  2. It receives the ECHO Request
  3. It sends the ECHO reply, following ICMP rules explained in RFC 791
  4. It ends the execution
  The program execute the testing phase, by calling the command ping on a remote machine called "lab".
  </details>
  <details><summary> split_ping.c </summary>
The program works as a ping but splitting the ECHO request in 2 different IP packets:
1. With payload size of 16 bytes
2. With payload of needed size
  </details>
