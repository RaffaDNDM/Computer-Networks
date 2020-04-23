#include "base64.h"

int main(int argc, char** argv)
{
    

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
    int length = strlen(input);
    int num=length/3;
   
    *output = malloc(sizeof(char)*((num)*3+3));
    
    int i=0;
    for(; i<num; i++)
    {
        int j=0;
        unsigned char mask1 = 252;
        unsigned char mask2 = 63;
        int k=0;

        for(; j<3; j++)
        {
            unsigned char mask1_t = mask1<<(2*j);
            unsigned char mask2_t = mask2>>(2*(3-j));   
            *output[i*3+j] =  
        }
    }
    
    //Padding phase
    if((n=length%3) != 0)
    { 
        int i=0;
        for(; i<length; i++)
        {
            pad_input[i]=input[i];
        }
    }

    *output = malloc(sizeof(char)*((length/3)));

}

void decode(char* input, char** output)
{
    int length = strlen(input);
    int n=0;

    if((n=length%4) != 0)
    {
        perror("The input string is not base64");
        exit(1);
    }


}
