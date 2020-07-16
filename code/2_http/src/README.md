# HTTP protocol  
- ***Web Client***
  <details> <summary> <b><i>wc09.c</i></b> </summary> 
  Web client that makes an HTTP/0.9 request to the server and reads the response of the server.
  </details>
  <details> <summary> <b><i>wc10.c</i></b> </summary> 
  Web client that makes an HTTP/1.0 request and analyses the response of the server looking for Content-Length header.
  </details>
  <details> <summary> <b><i>wc11.c</i></b> </summary>
  Web client that makes an HTTP/1.1 request and analyses the response of the server looking for either Content-Length or Transfer-Encoding header.
  </details>
  <details> <summary> <b><i>wc10.c</i></b> </summary> 
  Web client that makes an HTTP/1.0 request and analyses the response of the server looking for Content-Length header.
  </details>
  <details> <summary> <b><i>wc_cache.c</i></b> </summary> 
  Web Client with caching implemented using Last-Modified header and HEAD method.
  </details>
  <details> <summary> <b><i>wc_cache2.c</i></b> </summary> 
  Web Client with caching implemented using If-Modified-Since header.
  </details>
  <details> <summary> <b><i>wc_trace.c</i></b> </summary> 
  Web Client that uses the method TRACE, defined in HTTP/1.1 and explained in  <a href="https://tools.ietf.org/html/rfc2616#section-9.8">Section 9.8 of the RFC 2616</a>, and understands if the request was modified by transparent proxies before reaching the server.<br> Web Client is going to be tested on following addresses:
  <ul>
    <li>184.168.221.96 (<a href="www.webtrace.com">www.webtrace.com</a>)</li>
    <li>46.37.17.205 (<a href="www.radioamatori.it">www.radioamatori.it</a>)</li>
  </ul>
  </details>

- ***Web Server***
  <details> <summary> <b><i>ws.c</i></b> </summary> 
  Standard version with management of function calls.
  </details>
  <details> <summary> <b><i>ws_chunked.c</i></b> </summary> 
  Web Server with Transfer-Encoding:chunked header management.
  </details>
  <details> <summary> <b><i>ws_content.c</i></b> </summary> 
  Web Server with Content-Length header.
  </details>
  <details> <summary> <b><i>ws_cache.c</i></b> </summary> 
  Web Server with both caching management.
  </details>
  <details> <summary> <b><i>ws_reflect.c</i></b> </summary> 
  Web Server that replies with message of client, with additional info about client
  </details>
- ***Web Proxy***
  <details> <summary> <b><i>wp.c</i></b> </summary> 
  HTTPS and HTTP management.
  </details>
  <details> <summary> <b><i>wp_2_connections.c</i></b> </summary> 
  Keep-alive and close connections together.
  </details>
  <details> <summary> <b><i>wp_blacklist.c</i></b> </summary> 
  Web Proxy that manages HTTP requests from client, such that if the client has an IP address that is the same of one of 4 IP addresses stored in the proxy (max 4 addresses), the proxy leave the transfering of only HTML or TEXT files.
  </details>
 
