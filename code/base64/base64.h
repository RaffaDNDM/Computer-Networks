#ifndef BASE64
#define BASE64

#define ENCODE 0
#define DECODE 1

void base64(char* input, char** output, int code);
void encode(char* input, char** output);
void decode(char* input, char** output);

#endif
