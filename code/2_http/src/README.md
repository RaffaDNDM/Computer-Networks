# HTTP protocol  
- ***Web Client***
  <details> <summary> wc09.c </summary> 
  Web client that makes an HTTP/0.9 request to the server and reads the response of the server.
  </details>
  <details> <summary> wc10.c </summary> 
  Web client that makes an HTTP/1.0 request and analyses the response of the server looking for Content-Length header.
  </details>
  <details> <summary> wc11.c </summary>
  Web client that makes an HTTP/1.1 request and analyses the response of the server looking for either Content-Length or Transfer-Encoding header.
  </details>
  <details> <summary> wc10.c </summary> 
  Web client that makes an HTTP/1.0 request and analyses the response of the server looking for Content-Length header.
  </details>
  <details> <summary> wc_cache.c </summary> 
  Web Client with caching implemented using Last-Modified header and HEAD method.
  </details>
  <details> <summary> wc_cache2.c </summary> 
  Web Client with caching implemented using If-Modified-Since header.
  </details>
- ***Web Server***
  <details> <summary> ws.c </summary> 
  Standard version with management of function calls.
  </details>
  <details> <summary> ws_chunked.c </summary> 
  Web Server with Transfer-Encoding:chunked header management.
  </details>
  <details> <summary> ws_content.c </summary> 
  Web Server with Content-Length header.
  </details>
  <details> <summary> ws_cache.c </summary> 
  Web Server with both caching management.
  </details>
  <details> <summary> ws_reflect.c </summary> 
  Web Server that replies with message of client, with additional info about client
  </details>
- ***Web Proxy***
  <details> <summary> wp.c </summary> 
  HTTPS and HTTP management.
  </details>
  <details> <summary> wp_2_connections.c </summary> 
  Keep-alive and close connections together.
  </details>
