# Computer-Networks
Application of Computer Networks protocols in C, C++, Python and Java and [notes](https://github.com/RaffaDNDM/Computer-Networks/blob/master/Computer_networks.pdf) of attended course or [notes](https://github.com/RaffaDNDM/Computer-Networks/blob/master/Telecommunication_networks.pdf) about telecommunication networks.

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
  - Web Proxy
    1. HTTPS (4-Level Gateway) and HTTP (7-Level Gateway) management
    2. Keep-alive and close connections together
    3. Blacklist mangement for some domains
    4. Filter of type of content of the response
    5. Limit of the average bitrate in HTTPS connection
    6. Limit of the average bitrate in HTTPS connection (alternative version)
    7. Limit the maximum bitrate in HTTPS connection
    8. Whitelist management of some domains
  - Web Server
    1. Standard version with also management of function calls
    2. Web Server with both versions of caching management
    2. Web Server with Transfer-Encoding:chunked header
    3. Web Server with Content-Length header
    5. Web Server that replies with message of client, with additional info about client
- [Base64 implementation.](https://github.com/RaffaDNDM/Computer-Networks/tree/master/code/3_base64)
- [Implementation of Ethernet Packets using socket.](https://github.com/RaffaDNDM/Computer-Networks/tree/master/code/4_dll)
  1. ARP (Address Resolution Protocol)
  2. Inverse ping
  3. Ping
  4. Record route option of IP header
  5. Split ICMP ECHO REQUEST in two packets
  6. Statistics about number of received packets
  7. TCP request and response
  8. ICMP Time exceeded message management
  9. Traceroute
  10. ICMP Unreachable destination message management
