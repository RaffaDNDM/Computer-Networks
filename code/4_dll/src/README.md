# Data Link Layer
  <details> <summary> <b><i>arp.c</b></i> </summary>
  Implementation of ARP protocol for Address Resolution (ARP request + ARP reply).
  </details>
  <details> <summary> <b><i>inverse_ping.c</b></i> </summary>
  Program that analyses an ECHO request, sent by a remote node, and replies to it with an ECHO reply.<br>
  The program follows these steps:
  <ol>
  <li>It waits for an ECHO Request</li>
  <li>It receives the ECHO Request</li>
  <li>It sends the ECHO reply, following ICMP rules explained in <a href="https://tools.ietf.org/html/rfc792">RFC 792</a></li>
  <li>It ends the execution</li>
  </ol>
  The program execute the testing phase, by calling the command ping on a remote machine called <i>"lab"</i>.
  </details>
  <details> <summary> <b><i>ping.c</b></i> </summary>
  Implementation of PING application, using ICMP ECHO request and ECHO reply.
  </details>
  <details> <summary> <b><i>record_route.c</b></i> </summary>
  
  </details>
  <details><summary> <b><i>split_ping.c</b></i> </summary>
  The program works as a ping but splitting the ECHO request in 2 different IP packets:
  <ol>
  <li>With payload size of 16 bytes</li>
  <li>With payload of needed size</li>
  </ol>
  To program it, I used the fragmentation of IP datagrams (<a href="https://tools.ietf.org/html/rfc791#section-3.1">Section 3.1 of RFC 791</a>). 
  </details>
  <details> <summary> <b><i>statistics.c</b></i> </summary>
  
  </details>
  <details> <summary> <b><i>tcp.c</b></i> </summary>
  
  </details>
  <details> <summary> <b><i>time_exceeded.c</b></i> </summary>
  
  </details>
  <details> <summary> <b><i>traceroute.c</b></i> </summary>
  
  </details>
  <details> <summary> <b><i>unreachable_dest.c</b></i> </summary>
  
  </details>
  <details> <summary> <b><i>utility.c</b></i> </summary>
  
  </details>
