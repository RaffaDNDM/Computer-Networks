#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdio.h>
#include <arpa/inet.h>

//Identifies the address we want to connect to
struct sockaddr_in server;

int main()
{
    //Creation of socket = number of index in ufdt
    int s = socket(AF_INET, SOCK_STREAM, 0);
    char request[100];

    if(s == -1)
    {
        perror("Socket Failed");
        return 1;
    }
    
    //Extablish the connection
    unsigned char ip_addr[4] = {216, 58, 208, 163};
    server.sin_family = AF_INET; 

    //Ex. my_htons write a function that change order of bytes (from LE to BE)
    
    //http service (connection to http service)
    server.sin_port = htons(80); 
    
    //http service (connection to http service)
    server.sin_addr.s_addr = *(unsigned int*) ip_addr; 
    int t = connect(s, (struct sockaddr *) &server, sizeof(server)); 

    if(t==-1)
    {
        perror("Connection error");
        return 1;
    }

    //Send a request
    sprintf(request, "GET /\r\n");
}
