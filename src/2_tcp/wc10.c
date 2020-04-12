#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define LINE "--------------------------------------------------------\n"
struct sockaddr_in server;

struct header{
    char* name;
    char* value;  
}h[30];

int main(int argc, char ** argv)
{
    int sd; //Socket Descriptor
    int t; //Control value returned by connect, write and read
    int i;
    int j;
    int k;
    int status_length;
    int size;
    int code;
    int body_length;
    char request[100];
    char response[1000000];
    char *website;
    /*
    char *version;
    char *code;
    char *phrase;
    */
    char *status_tokens[3];
    unsigned char ipaddr[4] = {216,58,211, 163};

    sd = socket(AF_INET, SOCK_STREAM, 0);

    if(argc>3)
    {
        perror("Too many arguments");
        return 1;
    }

    if(sd==-1)
    {   
        printf("Errno = %d \n", errno);
        perror("Socket failed");
        return 1;
    }

    server.sin_family=AF_INET;
    server.sin_port = htons(80); //HTTP port number
    
    if(argc>1)
    {
        server.sin_addr.s_addr=inet_addr(argv[1]);
        //or inet_aton(argv[1], &server.sin_addr);
        
        if(argc==3)
            server.sin_port = htons(atoi(argv[2])); //HTTP port number
    }
    else
    {
        server.sin_addr.s_addr = *(uint32_t *) ipaddr;
        server.sin_port = htons(80); //HTTP port number
    }
    t = connect(sd, (struct sockaddr *)&server, sizeof(server));

    if(t==-1)
    {
        printf("Errno = %d \n", errno);
        perror("Connection failed \n");
        return 1;
    }
    
    //sprintf(request, "GET / HTTP/1.0\r\n\r\n");
    sprintf(request, "GET / HTTP/1.0\r\nHost: www.repubblica.it\r\n\r\n");

    for(size=0; request[size]; size++);

    t = write(sd, request, size);
    
    if(t == -1)
    {
        printf("Errno = %d\n", errno);
        perror("Write failed");
        return 1;
    }

    j = 0;
    k = 0;
    h[k].name= response;

    while(read(sd, response+j, 1))
    {
        if((response[j]=='\n') && (response[j-1]=='\r'))
        {
            response[j-1]=0;
            
            if(h[k].name[0]==0)
                break;

            h[++k].name = response+j+1;
        }

        if(response[j]==':' && h[k].value==0)
        {
            response[j]=0;
            h[k].value=response+j+1;
        }
        j++;
    }

    //Print content of Status line + HTTP headers 
    
    /* 
    printf("Status line: %s\n", h[0].name);
    version = strtok(h[0].name, " ");
    code = strtok(NULL, " ");
    phrase = strtok(NULL, " ");
    */
    
    
    for(status_length=0; h[0].name[status_length]; status_length++);

    status_tokens[0]=h[0].name;
    i=1;
    k=1;
    for(i=0; i<status_length && k<3; i++)
    {
        if(h[0].name[i]==' ')
        {
            h[0].name[i]=0;
            status_tokens[k]=h[0].name+i+1;
            k++;
        }
    }
    
    printf(LINE);
    printf("Status line:\n");
    printf(LINE);
    /*
    printf("HTTP version: %30s\n", version);
    printf("HTTP code:    %30s\n", code);
    printf("HTTP version: %30s\n", phrase);
    */
    printf("HTTP version: %30s\n", status_tokens[0]);
    code = atoi(status_tokens[1]);
    printf("HTTP code:    %30d\n", code);
    printf("HTTP version: %30s\n", status_tokens[2]);
    printf(LINE);


    
    website=NULL;
    for(i=1; h[i].name[0]; i++)
    {
        if(!strcmp(h[i].name, "Content-Length"))
            body_length = atoi(h[i].value);

        if(!strcmp(h[i].name, "Location") && code>300 && code<303)
            website=h[i].value;

        printf("Name= %s -----> Value= %s\n",h[i].name, h[i].value);
    }

    
    if(body_length)
        for(size=0; (t=read(sd, response+j+size, body_length-size))>0; size+=t);
    else
        for(size=0; (t=read(sd, response+j+size, 1000000-size))>0; size+=t);
    

    if(t==-1)
    {
        printf("Errno = %d\n", errno);
        perror("Read failed");
        return 1;
    }

    //if(website!=NULL)
        printf("\nRedirection:       %s \n\n", website);
        
    for(i=j; i<size+j; i++)
        printf("%c", response[i]);

    return 0;
}
