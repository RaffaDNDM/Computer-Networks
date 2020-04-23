#include "base64.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
    char input[100];
    char *output;
    scanf("%s", input);

    base64(input, &output, ENCODE);

    free(output);
    return 0;
}

void base64(char* input, char** output, int code)
{
    if(code)
        decode(input, output);
    else
        encode(input, output);
}

void encode(char* input, char** output)
{
    int length_in = strlen(input);
    int length_out;

    length_out = (length_in%3!=0)? ((length_in/3)*4+3) : ((length_in/3)*4);
    *output = malloc(sizeof(char)*length_out);

    int h=0;
    int k=0;
    for(; h<(length_in/3); h++)
    {
        int i=0;
        
        unsigned char mask1 = 252;
        unsigned char mask2 = 63;

        for(; i<3; i++)
        {
            int j=(i<1)? 0 : (i-1);

           
            char less_significant = input[h*3+j] & (mask1>>(2*i));
            char most_significant = input[h*3+i] & (mask2>>((3-i)*2));

            *output[k] = (less_significant>>(2*(i+1))) | (most_significant<<(3-i)*2);
            k++; 
        }
    }
}

void decode(char* input, char** output)
{
    int length = strlen(input);

    if(length%4!=0)
    {
        perror("No base64 encoded string\n");
        exit(1);
    }
}
