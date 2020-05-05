#ifndef WC11
#define WC11

void parse_header(int sd, char* response, char** status_tokens, int* header_size);
void analysis_headers(char **status_tokens, header* h, int* body_length, char* website);
void body_acquire(int sd, int body_length, char* entity, int *size);

#endif
