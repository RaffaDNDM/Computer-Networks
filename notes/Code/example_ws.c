#include "net_utility.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#define QUEUE_MAX 10
#define ROOT_PATH "../dat"
#define CGI_BIN "/cgi-bin/"
#define CGI_RESULT "../dat/result.txt"

struct sockaddr_in local, remote;

int main()
{
    char request[2000], response[2000];
    char *method, *path, *version;
    int sd, sd2;
    int t;
    socklen_t len;
    int yes = 1;
    FILE *f;

    signal(SIGINT, endDaemon);

    //Initialization of TCP socket for IPv4 protocol
    sd = socket(AF_INET, SOCK_STREAM, 0);
    control(sd, "Socket failed\n");

    //Bind the server to a specific port
    local.sin_family=AF_INET;
    local.sin_port = htons(8080); //we need to use a port not in use
    local.sin_addr.s_addr = 0; //By default, it

    //Reuse the same IP already bind to other program
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    t = bind(sd, (struct sockaddr*) &local, sizeof(struct sockaddr_in));
    control(t, "Bind failed \n");

    //Queue of pending clients that want to connect
    t = listen(sd, QUEUE_MAX);
    control(t, "Listen failed \n");

    while(1)
    {
        f=NULL;
        remote.sin_family = AF_INET;
        len = sizeof(struct sockaddr_in);

        //Accept the new request and create its socket
        sd2 = accept(sd, (struct sockaddr*) &remote, &len);
        control(sd2, "Accept failed \n");

        //A child manages the single request
        if(!fork())
        {
            //Read the request of the client
            t = read(sd2, request, 1999);
            request[t]=0;

            //Parser of request line
            request_line(request, &method, &path, &version);
            printf("Method:  %s\n", method);
            printf("Path:  %s\n", path);
            printf("Version:  %s\n", version);

            //Manage the response to the request
            manage_request(method, path, version, response, &f);
            printf("%s", response);
            write(sd2, response, strlen(response));
            send_body(sd2, f);

            //Shutdown the socket created with the specific client
            shutdown(sd2, SHUT_RDWR);
            close(sd2);
            exit(0);
        }
    }
}


void request_line(char* request, char** method, char** path, char** version)
{
    int i;
    *method = request;

    for(i=1; request[i]!=' '; i++);

    request[i]=0;
    *path=request+i+1;

    for(; request[i]!=' '; i++);

    request[i]=0;
    *version=request+i+1;

    for(; (request[i]!='\n' || request[i-1]!='\r') ; i++);

    request[i-1]=0;
}

void manage_request(char* method, char* path, char* version, char* response, FILE** f)
{
    if(strcmp(method,"GET")) //it's not GET request
        sprintf(response, "HTTP/1.1 501 Not Implemented\r\n\r\n");
    /*
     * else if((*f=fopen(path+1,"r"))==NULL) //it's GET request for a file
        //path+1 is used to remove the / root directory
        sprintf(response,"HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
    else
        sprintf(response,"HTTP/1.1 200 Not Found\r\nConnection: close\r\n\r\n");
    */
    else
    {
        char file_name[40];
        sprintf(file_name,"%s%s",ROOT_PATH,path);

        if(!strncmp(path, CGI_BIN, 9))
        {
            int i=0;
            char* arguments[10];

            int size_path =strlen(path);
            for(i=9; i<size_path && path[i]!='?'; i++);

            printf("%d\n", i);
            path[i]=0;
            int j=0;
            for(i=i+1; i<size_path && j<10; i++)
            {
                if(path[i]=='=')
                    arguments[j++]=path+i+1;

                if(path[i]=='&')
                    path[i]=0;
            }

            char command[60];
            sprintf(command, "cd %s ; %s", ROOT_PATH, path+9);

            for(i=0; i<j; i++)
            {
                int size = strlen(command);
                sprintf(command+size, " %s", arguments[i]);

                printf("%s", arguments[i]);
            }

            int size = strlen(command);
            sprintf(command+size, " > %s", CGI_RESULT);
            printf("%s\n", command);

            int status = system(command);

            if(status==-1)
            {
                //Used to manage if a program doesn't exists
                sprintf(response,"HTTP/1.1 400 Not Found\r\nConnection:Close\r\n\r\n");
                *f=NULL;
            }
            else if(!status)
            {
                //Useless if because the file is always created, because of pipe implementation
                if(((*f)=fopen(CGI_RESULT, "r+"))==NULL)
                {
                    perror("Error with CGI");
                }
                else
                    sprintf(response,"HTTP/1.1 200 OK\r\nConnection:Close\r\n\r\n");
            }

        }
        else
        {
            printf("%s\n", file_name);

            //"r+" because in linux directory are file so we need to specify
            //also writing rights to be sure that fopen return NULL with also directory
            if(((*f)=fopen(file_name,"r+"))==NULL) //it's GET request for a file
                sprintf(response,"HTTP/1.1 404 Not Found\r\nConnection:Close\r\n\r\n");
            else
                sprintf(response,"HTTP/1.1 200 OK\r\nConnection:Close\r\n\r\n");
        }
    }
}

void send_body(int sd2, FILE* f)
{
    char c;
    if(f!=NULL)
    {
        while((c=fgetc(f))!=EOF)
            write(sd2, &c, 1);

        fclose(f);
    }
}

void endDaemon(int sig)
{
    FILE* f;

    if((f=fopen(CGI_RESULT,"r+"))!=NULL)
    {
        char command[40];
        sprintf(command, "rm %s", CGI_RESULT);
        system(command);
    }

    exit(0);
}
