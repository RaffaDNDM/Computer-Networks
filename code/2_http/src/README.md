# HTTP protocol  
- ***Web Client***
  <details> <summary> <b><i>wc09.c</i></b> </summary> 
  Web client that makes an HTTP/0.9 request to the server and reads the response of the server.
  </details>
  <details> <summary> <b><i>wc10.c</i></b> </summary> 
  Web client that makes an HTTP/1.0 request and analyses the response of the server looking for <i>Content-Length</i> header.
  </details>
  <details> <summary> <b><i>wc11.c</i></b> </summary>
  Web client that makes an HTTP/1.1 request and analyses the response of the server looking for either <i>Content-Length</i> or <i>Transfer-Encoding</i> header.
  </details>
  <details> <summary> <b><i>wc10.c</i></b> </summary> 
  Web client that makes an HTTP/1.0 request and analyses the response of the server looking for <i>Content-Length</i> header.
  </details>
  <details> <summary> <b><i>wc_cache.c</i></b> </summary> 
  Web Client with caching implemented using <i>Last-Modified</i> header and <i>HEAD</i> method.
  </details>
  <details> <summary> <b><i>wc_cache2.c</i></b> </summary> 
  Web Client with caching implemented using <i>If-Modified-Since</i> header.

- ***Web Server***
  <details> <summary> <b><i>ws.c</i></b> </summary> 
  Web Server with also management of function calls.
  </details>
  <details> <summary> <b><i>ws_cache.c</i></b> </summary> 
  Web Server with both caching management.
  </details>
  <details> <summary> <b><i>ws_chunked.c</i></b> </summary> 
  Web Server with generation of <i>Transfer-Encoding:chunked</i> header.
  </details>
  <details> <summary> <b><i>ws_content.c</i></b> </summary> 
  Web Server with generation of Content-Length header.
  </details>
  <details> <summary> <b><i>ws_reflect.c</i></b> </summary> 
  Web Server that, as soon as it receives a request from the client for the resource corresponding to the path <i>"/reflect"</i>, instead of looking for a resource file to open and send back, it sends to the client a response in which the entity body is composed in order by:
  <ol>
   <li>The text corresponding to the entire request, sent by the client to the server </li>
   <li>\<CRLF\> </li>
   <li>The IP address in dotted decimal notation from which the client sent its request </li>
   <li>\<CRLF\> </li>
   <li>Port number from which the client made its request</li>
  </ol>
  </details>

- ***Web Proxy***
  <details> <summary> <b><i>wp.c</i></b> </summary> 
  HTTPS and HTTP management.
  </details>
  <details> <summary> <b><i>wp_2_connections.c</i></b> </summary> 
  Keep-alive and close connections together.
  </details>
  <details> <summary> <b><i>wp_blacklist.c</i></b> </summary> 
  Web Proxy that blocks requests to some domain, stored in an array.
  </details>
  <details> <summary> <b><i>wp_cache.c</i></b> </summary> 
  </details>
  <details> <summary> <b><i>wp_filter_response.c</i></b> </summary>
  Web Proxy that manages HTTP requests from client, such that if the client has an IP address that is the same of one of 4 IP addresses stored in the proxy (max 4 addresses), the proxy leave the transfering of only HTML or TEXT files.
  To create this program, I analyse the Content-Type header value of HTTP response from the Server (<a href="https://tools.ietf.org/html/rfc1945#section-10.5">Section 10.5 of RFC 1945</a>). 
  </details>
  <details> <summary> <b><i>wp_limit_avg_bitrate.c</i></b> </summary> 
  Web Proxy that limits the bandwidth when it works as a Layer-4 gateway. It limits the average upload bitrate, from client to the server, to <i>1.0 Kbits per second</i> (maximum) and the average download to <i>10.0 Kbits per second</i> (maximum).<br>
  The program is implemented using <i>gettimeofday(3)</i>, <i>usleep(3)</i>, <i>sleep(3)</i> UNIX library functions to implement the mechanism.
  </details>
  <details> <summary> <b><i>wp_limit_avg_bitrate2.c</i></b> </summary> 
  Web Proxy like the one implemented in <b><i>wp_limit_avg_bitrate.c</i></b>, that does the same things in an alternative way.
  </details>
  <details> <summary> <b><i>wp_limit_bitrate.c</i></b> </summary>     
  Web Proxy that limits the bandwidth when it works as a Layer-4 gateway. It limits the upload bitrate, from client to server, to <i>1.0 Kbits per second</i> (maximum) and the average download bitrate, from the server to the client, to <i>10.0 Kbits per second</i> (maximum).<br>
  The program is implemented using <i>gettimeofday(3)</i>, <i>usleep(3)</i>, <i>sleep(3)</i> UNIX library functions to implement the mechanism.
  </details>
  <details> <summary> <b><i>wp_whitelist.c</i></b> </summary>   
  Web Proxy that allows requests only to some domain, stored in an array.
  </details>
