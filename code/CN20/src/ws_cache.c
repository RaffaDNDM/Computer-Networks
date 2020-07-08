#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define __USE_XOPEN
#include <time.h>

#define LINE "__________________________________________________________"
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
    int i,s,t,s2,n,len,c,yes=1, head; 
    
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
        remote.sin_family=AF_INET;
        len = sizeof(struct sockaddr_in);
        memset(&remote, 0, sizeof(struct sockaddr_in));

        s2 = accept(s,(struct sockaddr *) &remote, &len);
        if (s2 == -1) {perror("Accept Failed\n"); return 1;}
        
        if (!fork())
        {
            int keep_alive= 0;

            f = NULL; // <<<<< BACO
            head=0;
            n=read(s2,request,1999);
            request[n]=0;
            printf("%s\n%s\n",LINE, request);
            method = request;
            
            for(i=0;(i<n) && (request[i]!=' ');i++); request[i]=0;
            path = request+i+1;
            for(   ;(i<n) && (request[i]!=' ');i++); request[i]=0;
            version = request+i+1;
            for(   ;(i<n) && (request[i]!='\r');i++); request[i]=0;
            
            printf("Method = %s, path = %s , version = %s\n",method,path,version);	
           
            i+=2;
            int k=0;
            h[k].name = request+i;
            while(i<n)
            {
                if(request[i]=='\n' && request[i-1]=='\r')
                {
                    request[i-1]=0;
                    
                    if(h[k].name[0]==0)
                        break;
                    
                    h[++k].name=request+i+1;
                }
                else if(request[i]==':' && h[k].value==0)
                {
                    request[i]=0;
                    h[k].value = request+i+1;
                }
                i++;
            }

            if(!strcmp(version, "HTTP/1.1"))
                keep_alive=1;
            else if(!strcmp(version, "HTTP/1.0"))
            {
                for(i=0; h[i].name[0]; i++)
                {
                    if(!strcmp(h[i].name, "Connection") && !strcmp(h[i].value, "keep_alive"))
                    {
                        keep_alive=1;
                        break;
                    }
                    printf("[%s] -----> %s\n",h[i].name, h[i].value);
                }
            }

            if(!strcmp("GET",method))
            { // it is a get
                if(!strncmp(path,"/cgi-bin/",9))
                { // CGI interface  
                    sprintf(command,"%s > results.txt",path+9);
                    printf("executing %s\n", command);
                    system(command);
                    
                    if((f=fopen("results.txt","r"))==NULL)
                    {
                        printf("cgi bin error\n");
                        return 1;
                    }
                    
                    sprintf(response,"HTTP/1.1 200 OK\r\nConnection:close\r\n\r\n");
                }
                else if((f=fopen(path+1,"r"))==NULL)
                    sprintf(response,"HTTP/1.1 404 Not Found\r\nConnection:close\r\n\r\n");
                else 
                    sprintf(response,"HTTP/1.1 200 OK\r\nConnection:close\r\n\r\n");
            }
            else if(!strcmp("HEAD", method))
            {
                head=1;
                if(strncmp(path,"/cgi-bin/", 9))
                {
                    if((f=fopen(path+1, "r"))!=NULL)
                    {
                        struct stat attr;
                        struct tm tm;
                        memset(&tm, 0, sizeof(tm));

                        char date[30];
                        stat(path+1, &attr);
                        tm = *(gmtime(&attr.st_mtime));
                        //strptime(gmtime(&attr.st_mtime), "%a %b  %d %H:%M:%S %Y", &tm);
                        strftime(date, 30, "%a, %d %b %Y %H:%M:%S %Z", &tm);
                        sprintf(response, "HTTP/1.1 200 OK\r\nConnection:keep-alive\r\nLast-Modified:%s\r\n\r\n", date); 
                    }
                    else
                        sprintf(response,"HTTP/1.1 404 Not Found\r\nConnection:close\r\n\r\n");
                }
            }
            else	
                sprintf(response, "HTTP/1.1 501 Not Implemented\r\n\r\n");
            
            write(s2,response,strlen(response)); // HTTP Headers
            
            if(f!=NULL)
            { 
                // if present, the Entity Body 
                
                if(!head)
                {
                    while((c=fgetc(f))!=EOF)
                        write(s2,&c,1);
                }

                fclose(f); 
            }

            printf("%s\n", LINE);
            shutdown(s2,SHUT_RDWR);
            close(s2);	
            exit(0);
	    }
	}
}
