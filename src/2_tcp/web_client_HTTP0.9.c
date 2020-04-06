#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

//Identifies the address we want to connect to
struct sockaddr_in server;

int main()
{
    /*
     * Creation of socket = file descriptor for the Socket
     *                      (number of index in ufdt)
     */
    int size;
    int s = socket(AF_INET, SOCK_STREAM, 0);
    char request[100], response[1000000];

    if(s == -1)
    {
        perror("Socket Failed\n");
        return 1;
    }

    /*
     * Extablish the connection to www.google.it
     */

    //Family of addresses (IPv4 addresses)
    server.sin_family = AF_INET;

    //http service port = 80
    server.sin_port = htons(80);

    //Definition of IP address of google
    unsigned char ip_addr[4] = {216, 58, 208, 163};
    server.sin_addr.s_addr = *(unsigned int*) ip_addr;
    int t = connect(s, (struct sockaddr *) &server, sizeof(server));

    if(t==-1)
    {
        perror("Connection error\n");
        return 1;
    }

    /*
     * Send a Request (Application Layer = HTTPS)
     */
    sprintf(request, "GET /\r\n");
    t = write(s, request, 7);

    if(t==-1)
    {
        perror("Write failed\n");
        return 1;
    }

    /*
     * Receive the response (HTML page)
     * 1000000=MAX length
     * 1000000-size ----> guarantees that the max amount of characters read is 1000000
     */
    for(size=0; (t=read(s, &response[size], 1000000-size))>0; size=size+t);

    //Print the value of the response message
    int i;
    for(i=0; i<size; i++)
        printf("%c", response[i]);
}
