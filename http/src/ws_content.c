#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define LINE "_________________________________________________________"
struct sockaddr_in local,remote;
char request[2000],response[2000];
char * method, *path, *version;

struct header{
    char* name;
    char* value;
}h[30];

int main()
{
    FILE *f;
    char command[100];
    int i,s,t,s2,n,len,c,yes=1, j; 
    unsigned int count;
    char response_length[10];

    s = socket(AF_INET, SOCK_STREAM, 0);
    if ( s == -1) { perror("Socket Failed\n"); return 1;}

    local.sin_family=AF_INET;
    local.sin_port = htons(8083);
    local.sin_addr.s_addr = 0;
    
    setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
    t = bind(s,(struct sockaddr *) &local, sizeof(struct sockaddr_in));
    if ( t == -1) { perror("Bind Failed \n"); return 1;}
    
    t = listen(s,10);
    if ( t == -1) { perror("Listen Failed \n"); return 1;}
    
    while( 1 )
    {
        f = NULL; // <<<<< BACO
        remote.sin_family=AF_INET;
        len = sizeof(struct sockaddr_in);
        s2 = accept(s,(struct sockaddr *) &remote, &len);
        if (s2 == -1) {perror("Accept Failed\n"); return 1;}
        
        if (!fork())
        {
            n=read(s2,request,1999);
            request[n]=0;
            method = request;
            for(i=0;(i<2000) && (request[i]!=' ');i++); 
            request[i]=0;
            path = request+i+1;
            
            for(   ;(i<2000) && (request[i]!=' ');i++); 
            request[i]=0;
            version = request+i+1;
            
            for(   ;(i<2000) && (request[i]!='\r');i++); 
            request[i]=0;
            
            printf("%s\nMethod = %s, path = %s , version = %s\n",LINE,method,path,version);	

            i+=2;
            j=0;
            h[j].name = request+i;

            while(i<n)
            {
                if(request[i]=='\r' && request[i+1]=='\n')
                {
                    request[i]=0;
                    h[++j].name=request+i+2;
                    i++;
                }
                else if(request[i]==':' && h[j].value==0)
                {
                    request[i]=0;
                    h[j].value=request+i+1;
                }

                i++;
            }

            
            for(i=0; h[i].name[0]; i++)
            {
                printf("[%s] %s\n", h[i].name, h[i].value);

                if((!strcmp(h[i].name, "Connection") && !strcmp(h[i].value, "keep-alive")) 
                   || !strcmp(version, "HTTP/1.1"))
                    break;
            }

            printf("%s\n", LINE);
 
            if(!h[i].name[0] && !strcmp(version, "HTTP/1.0"))
                sprintf(response, "%s 400 Bad Request\r\n\r\n", version); 
            else if(strcmp("GET",method)) // it is not a GET
                sprintf(response, "%s 501 Not Implemented\r\n\r\n", version);
            else 
            { // it is a get
                if(!strncmp(path,"/cgi-bin/",9))
                { 
                    // CGI interface  
                    sprintf(command,"%s > results.txt",path+9);
                    printf("executing %s\n", command);
                    system(command);
                    
                    if((f=fopen("results.txt","r"))==NULL)
                    {
                        printf("cgi bin error\n");
                        return 1;
                    }
                    sprintf(response,"%s 200 OK\r\nContent-Length", version);
                }
                else  if((f=fopen(path+1,"r"))==NULL)
                    sprintf(response,"%s 404 Not Found\r\nConnection:Close\r\n\r\n", version);
                else 
                    sprintf(response,"%s 200 OK\r\nContent-Length:", version);
            }

            write(s2,response,strlen(response)); // HTTP Headers
            
            if(f!=NULL)
            { 
                // if present, the Entity Body 
                count=0;
                while((c=fgetc(f))!=EOF)
                {
                    sprintf(response+count, "%c", c);
                    count++;
                }

                sprintf(response_length, "%d\r\n\r\n", count);
                write(s2, response_length, strlen(response_length));
                write(s2, response, count);

                fclose(f); 
            }

            shutdown(s2,SHUT_RDWR);
            close(s2);	
            exit(0);
        }
	}
}
