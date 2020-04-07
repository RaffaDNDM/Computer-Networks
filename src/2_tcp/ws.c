#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#define QUEUE_MAX 10

struct sockaddr_in local;

int main()
{
    int backlog=10;
    int sd;
    int t;
    int sd1;    

    sd = socket(AF_INET, SOCK_STREAM, 0);

    if(sd == -1)
    {
        printf("Errno: %d\n", errno);
        perror("Socket failed");
        return 1;
    }

    local.sin_family=AF_INET;
    //local.sin_port = htons(80); no possible because port 80 already used
    local.sin_port = htons(88); //we need to use a port not in use 
    local.sin_addr.s_addr = 0; //By default, it 

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
    //sd1 = accept(socket);
}
