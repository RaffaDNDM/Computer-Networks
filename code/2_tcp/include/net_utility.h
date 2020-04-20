#ifndef NET_UTILITY
#define NET_UTILITY

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

typedef struct {
    char* name;
    char* value;
}header;

#define LINE "--------------------------------------------------------\n"

int my_strlen(char* string);
void control(int code, char* message);
char hex2dec(char c);
void print_body(char* entity, int size, int first);

#endif
