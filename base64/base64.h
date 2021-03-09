#ifndef BASE64
#define BASE64
#include <stdint.h>

#define ENCODE 0
#define DECODE 1
#define LINE_SIZE 100
#define LINE "**********************************************"
#define OUTPUT "output.txt"

void base64(char* input, char** output, int code);
void encode(char* input, char** output);
void decode(char* input, char** output);
char encode_symbol(unsigned int num_symbol);
unsigned int decode_symbol(char base64_symbol);

#endif
