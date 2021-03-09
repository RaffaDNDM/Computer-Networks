#ifndef WEB_SERVER
#define WEB_SERVER

#include <stdio.h>
#define QUEUE_MAX 10
#define ROOT_PATH "../dat"
#define CGI_BIN "/cgi-bin/"
#define CGI_RESULT "../dat/result.txt"

void request_line(char* request, char** method, char** path, char** version);
void manage_request(char* method, char* path, char* version, char* response, FILE** f);
void send_body(int sd2, FILE* f);
void endDaemon(int sig);

#endif
