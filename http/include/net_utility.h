#ifndef NET_UTILITY
#define NET_UTILITY

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_BLUE "\033[1;34m"
#define BOLD_MAGENTA "\033[1;35m"
#define BOLD_CYAN "\033[1;36m"
#define DEFAULT "\033[0m"

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
