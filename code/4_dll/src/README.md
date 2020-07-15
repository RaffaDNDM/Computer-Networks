# Data Link Layer
  <details> <summary> arp.c </summary> 
  Web client that makes an HTTP/0.9 request to the server and reads the response of the server.
  </details>
  <details> <summary> ping.c </summary> 
  Web client that makes an HTTP/1.0 request and analyses the response of the server looking for Content-Length header.
  </details>
  <details> <summary> record\_route.c </summary>
  Web client that makes an HTTP/1.1 request and analyses the response of the server looking for either Content-Length or Transfer-Encoding header.
  </details>
  <details> <summary> statistics.c </summary> 
  Web client that makes an HTTP/1.0 request and analyses the response of the server looking for Content-Length header.
  </details>
  <details> <summary> tcp.c </summary> 
  Web Client with caching implemented using Last-Modified header and HEAD method.
  </details>
  <details> <summary> time\_exceeded.c </summary> 
  Web Client with caching implemented using If-Modified-Since header.
  </details>
- ***Web Server***
  <details> <summary> traceroute.c </summary> 
  Standard version with management of function calls.
  </details>
  <details> <summary> unreachable\_dest.c </summary> 
  Web Server with Transfer-Encoding:chunked header management.
  </details>
  <details> <summary> utility.c </summary> 
  Web Server with Content-Length header.
  </details>
