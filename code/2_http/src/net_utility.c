#include "net_utility.h"

int my_strlen(char* string)
{
    int size;
    for(size=0; string[size]; size++);

    return size;
}

void control(int code, char* message)
{
    if(code==-1)
    {
        printf("Errno = %d \n", errno);
        printf("%s \n", message);
        exit(0);
    }
}

char hex2dec(char c)
{
    switch(c)
    {
         case '0' ... '9':
             printf("%c", c);
             c = c - '0';
             break;

         case 'A' ... 'F':
             printf("%c", c);
             c = c - 'A' +10;
             break;

         case 'a' ... 'f':
             printf("%c", c);
             c = c - 'a' +10;
             break;

         default:
             control(-1,"Error in chunk size format");
    }

    return c;
}

void print_body(char* entity, int size, int first)
{
    printf(LINE);
    printf(LINE);
    printf("                    BODY\n");
    printf(LINE);

    int i;
    for(i=first; i<(first+size); i++)
        printf("%c", entity[i]);

    printf("\n");
    printf(LINE);
    printf("\n");
}
