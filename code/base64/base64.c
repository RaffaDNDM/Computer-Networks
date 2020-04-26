#include "base64.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
    char* input;

    int code=-1;
    int input_file=0;
    FILE *f_in, *f_out;

    if(argc<2)
    {
        perror("You need to specify -d or -e\n");
        return 1;
    }

    int i=1;
    for(; i<argc; i++)
    {
        if(!strcmp(argv[i],"-d"))
        { 
            if(code!=-1)
            {
                perror("Too many arguments");
                return 1;
            }

            code = DECODE;
            continue;
        }

        else if(!strcmp(argv[i],"-e"))
        {
            if(code!=-1)
            {
                perror("Too many arguments");
                return 1;
            }

            code = ENCODE;
            continue;
        }

        else
        {
            if((f_in=fopen(argv[i], "r+"))==NULL)
            {
                printf("Invalid argument\n");
                return 1;
            }
            else
            {
                input_file=1;
                continue;
            }
        }
    }

    input = malloc(sizeof(char)*LINE_SIZE);
    char *output;
    
    if(!input_file)
    {
        fgets(input, LINE_SIZE, stdin);        
        int length = strlen(input);
        input[length-1]=0; //remove \n

        base64(input, &output, code);

        printf("\n\n%s\n",LINE);
        printf("%s", output);
        printf("\n%s\n\n", LINE);
    }
    else
    {
        f_out=fopen(OUTPUT, "w");
        while(fgets(input, LINE_SIZE, f_in)!=NULL)
        {
            if(code==DECODE)
            {
                int length = strlen(input);
                input[length-1]=0; //remove \n
            }

            base64(input, &output, code);

            fprintf(f_out, "%s", output);
        }
    }

    free(output);
    return 0;
}

void base64(char* input, char** output, int code)
{
    switch(code)
    {
        case ENCODE:
            encode(input, output);
            break;

        case DECODE:
            decode(input, output);
            break;
    }
}

void encode(char* input, char** output)
{
    int length_in = strlen(input);
    int length_out;
    int i=0;
    int k=0;
    unsigned int num = 0;
    char* p = (char*) (&num);
    unsigned int mask = 0;

    length_out = (length_in%3!=0)? ((length_in/3)*4+5) : ((length_in/3)*4+1);
    *output = malloc(sizeof(char)*length_out);
    printf("length_in: %d\n", length_in);

    int count = length_in/3;
    printf("count: %d", count);

    for(; i<count; i++)
    {
        int j=0;
        
        mask = (unsigned int) 252*256*256*256;

        p[3]=input[i*3];
        p[2]=input[i*3+1];
        p[1]=input[i*3+2];
        
        printf("num: %u\n", num);

        for(; j<4; j++)
        {
            unsigned int num_base = (unsigned int) (num & mask>>(6*j));
            (*output)[k++] = encode_symbol(num_base>>((3-j)*6+8));
        }
    }

    num=0;
    mask = (unsigned int) 252*256*256*256;
    printf("k: %d", k);
    
    switch(length_in%3)
    {
        case(1):
        {
            p[3]=input[i*3];
            unsigned int num_base = num & (mask);
            (*output)[k++] = encode_symbol(num_base >> ((3*6)+8));
            num_base = num & (mask>>6);
            (*output)[k++] = encode_symbol(num_base >> ((2*6)+8));
            (*output)[k++]='=';
            (*output)[k++]='=';
            break;
        }

        case(2):
        {
            p[3]=input[i*3];
            p[2]=input[i*3+1];
            unsigned int num_base = num & (mask);
            (*output)[k++] = encode_symbol(num_base >> ((3*6)+8));
            num_base = num & (mask>>6);
            (*output)[k++] = encode_symbol(num_base >> ((2*6)+8));
            num_base = num & (mask>>2*6);
            (*output)[k++] = encode_symbol(num_base >> ((1*6)+8));
            (*output)[k++]='=';
 
            break;
        }
    }

    (*output)[k]=0;
}

void decode(char* input, char** output)
{
    int length_in = strlen(input);
    int length_out;

    if(length_in%4!=0)
    {
        perror("No base64 encoded string\n");
        exit(1);
    }

    if(input[length_in-2]=='=')
    {
        if(input[length_in-3]=='=')
            length_out = (length_in/4)*3-1;
        else
            length_out = (length_in/4)*3;
    }
    else
        length_out = (length_in/4)*3+1;

    *output = malloc(sizeof(char)*length_out);


    int i=0;
    int k=0;

    for(; i<(length_in/4); i++)
    {
        int j=0;
        unsigned int num_base=0;
        char* p = (char*) &num_base;

        for(; j<4; j++)
        {
            unsigned int num = decode_symbol(input[i*4+j]);
 
            printf("   %d\n", num);
            num_base = num_base | (num<<((3-j)*6));
        }

        int min=0;

        if(i==(length_in/4-1) && length_out==((length_in/4)*3-1))
            min = 2;
        
        if(i==(length_in/4-1) && length_out==((length_in/4)*3))
            min=1;

        for(j=2; j>=min; j--)
           (*output)[k++]=p[j];
        
    }

    (*output)[k]=0;
}

char encode_symbol(unsigned int num_symbol)
{
    char base64_sym;

    printf("num: %d\n", num_symbol);
    switch(num_symbol)
    {
        case 0 ... 25:
            base64_sym = 'A'+ (char) num_symbol;       
            break;

        case 26 ... 51:
            base64_sym = 'a'+(char) (num_symbol-26);
            break;

        case 52 ... 61:
            base64_sym = '0'+ (char) (num_symbol-52);
            break;

        case 62:
            base64_sym = '+';
            break;

        case 63:
            base64_sym = '/';
            break;
    
        default:
            printf("Not a valid number\n");
            exit(1);
    }

    return base64_sym;
}

unsigned int decode_symbol(char base64_symbol)
{
    unsigned char num_symbol;

    printf("%c", base64_symbol);

    switch(base64_symbol)
    {
        case 'A' ... 'Z':
            num_symbol = (base64_symbol-'A');
            break;

        case 'a' ... 'z':
            num_symbol = 26 + (base64_symbol-'a');
            break;

        case '0' ... '9':
            num_symbol = 52 + (base64_symbol-'0');
            break;

        case '+':
            num_symbol = 62;
            break;

        case '/':
            num_symbol = 63;
            break;

        case '=':
            num_symbol = 0;
            break;
    }

    return num_symbol;
}
