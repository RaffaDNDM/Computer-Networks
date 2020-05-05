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

void request_line(char* request, char** method, char** path, char** version);
void manage_request(char* method, char* path, char* version, char* response, FILE** f);
void send_body(int sd2, FILE* f);
void endDaemon(int sig);

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
    sd = socket(AF_INET, SOCK_STREAM, 0);

    if(sd == -1)
    {
        printf("Errno: %d\n", errno);
        perror("Socket failed");
        return 1;
    }

    local.sin_family=AF_INET;
    //local.sin_port = htons(80); no possible because port 80 already used
    local.sin_port = htons(8080); //we need to use a port not in use 
    local.sin_addr.s_addr = 0; //By default, it 

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    t = bind(sd, (struct sockaddr*) &local, sizeof(struct sockaddr_in));

    if(t==-1)
    {
        printf("Errno: %d\n", errno);
        perror("Bind failed");
        return 1;
    }

    //To prevent the connection to the server
    //Queue of pending clients that want to connect
    t = listen(sd, QUEUE_MAX);

    if(t==-1)
    {
        printf("Errno: %d\n", errno);
        perror("Listen Failed");
        return 1;
    }

    //The server can have more file descriptors mapped on the same port
    //only one socket = listening socket to extablish the connection (bind is unique)
    //then with accept a new socket is created (unique for each connection) that is bound to the same port
    //accept has all the info to disambiguate the connection
    //sd only to accept the connection, sd2 to read and write
    //sd1 = accept(socket);

    while(1)
    {
        f=NULL;
        remote.sin_family = AF_INET;
        len = sizeof(struct sockaddr_in);  
        sd2 = accept(sd, (struct sockaddr*) &remote, &len);

        if(sd2==-1)
        {
            perror("Accept failed\n");
            return 1;
        }

        if(!fork())
        {
            t = read(sd2, request, 1999);
            request[t]=0;
        
            request_line(request, &method, &path, &version);
            printf("Method:  %s\n", method);
            printf("Path:  %s\n", path);
            printf("Version:  %s\n", version);

            manage_request(method, path, version, response, &f);
            printf("%s", response);
            write(sd2, response, strlen(response));
            send_body(sd2, f);

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
            char command[40]; 
            sprintf(command, "cd %s & %s > %s", ROOT_PATH, path+9, CGI_RESULT);
            int status = system(command);
            
            if(!status)        
            {
                //Useless if because the file is always created, because of pipe implementation
                if(((*f)=fopen(CGI_RESULT, "r+"))==NULL)
                {
                    perror("Error with CGI");
                }
                else
                    sprintf(response,"HTTP/1.1 200 OK\r\nConnection:Close\r\n\r\n");
            }
            else if(status==-1)
            {       
                //Used to manage if a program doesn't exists
                sprintf(response,"HTTP/1.1 400 Not Found\r\nConnection:Close\r\n\r\n");
                *f=NULL;
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

