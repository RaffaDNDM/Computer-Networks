# Computer-Networks
Application of Computer Networks protocols in C, C++, Python and Java and [notes](https://github.com/RaffaDNDM/Computer-Networks/blob/master/notes/Computer_networks.pdf) about attended course.

### Code
Implementation of network applications, using Linux socket.
- [Endianness used in Operating Systems and Network Byte Order.](https://github.com/RaffaDNDM/Computer-Networks/tree/master/code/1_endianness)
  1. Evaluation of endianness of the system.
  2. Convertion of number from Host to Network Byte Order.
- [Implementation of HTTP protocol using TCP socket.](https://github.com/RaffaDNDM/Computer-Networks/tree/master/code/2_http)
  - Web Client
    1. Web Client HTTP/0.9
    2. Web Client HTTP/1.0
    3. Web Client HTTP/1.1
    4. Web Client with caching implemented using Last-Modified header and HEAD method
    5. Web Client with caching implemented using If-Modified-Since header
  - Web Server
    1. Standard version with management of function calls
    2. Web Server with Transfer-Encoding:chunked header
    3. Web Server with Content-Length header
    4. Web Server with both caching management
    5. Web Server that replies with message of client, with additional info about client
  - Web Proxy
    1. HTTPS and HTTP management
    2. Keep-alive and close connections together
- [Base64 implementation.](https://github.com/RaffaDNDM/Computer-Networks/tree/master/code/3_base64)
- [Implementation of Ethernet Packets using socket.](https://github.com/RaffaDNDM/Computer-Networks/tree/master/code/4_dll)
  1. ARP (Address Resolution Protocol)
  2. ping
  3. traceroute
  4. statistics about received packets
  5. record route oprion of IP header
  6. time exceeded
  7. unreachable destination ICMP
