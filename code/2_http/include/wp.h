#ifndef WEB_PROXY
#define WEB_PROXY

#define QUEUE_MAX 10

void request_line(char* request, char** method, char** path, char** version);
void manage_request(char* method, char* path, char* version, int sd2);
void parser_path(char* path, char** scheme, char** host, char** resource);
void parser_connect(char* path, char** host, char** port);
int connect2server(char* host, char* port);

#endif
