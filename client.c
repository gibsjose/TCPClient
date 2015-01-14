//Example client application

#include <sys/socket.h> //Socket features
#include <netinet/in.h> //Internet-specific features of sockets
#include <arpa/inet.h>  //inet_addr()
#include <stdio.h>
#include <string.h>
#include <unistd.h>

//Define the port and address
#define PORT 8888
#define ADDRESS "127.0.0.1"

int main(int argc, char *argv[]) {

    FILE *file;

    //Create a socket:
    //socket() system call creates a socket, returning a socket descriptor
    //  AF_INET specifies the address family for internet
    //  SOCK_STREAM says we want TCP as our transport layer, by requesting stream-oriented communication
    //  0 is a parameter used for some options for certain types of sockets, unused for INET sockets
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0) {
        printf("Could not open socket\n");
        return -1;
    }

    //Specify the address for the socket
    //Create the socket address structure and populate it's fields
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;                        //Specify the family again (AF_INET = internet family)
    serveraddr.sin_port = htons(PORT);                      //Specify the port on which to send data (16-bit) (# < 1024 is off-limits)
    serveraddr.sin_addr.s_addr = inet_addr(ADDRESS);        //Specify the IP address of the server with which to communicate
                                                            //inet_addr() converts a string-address into the proper type

    //Open TCP "Connection"
    //Specify the socket descriptor, the server address structure, and the size of the socket address structure
    int e = connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(struct sockaddr_in));

    if(e < 0) {
        printf("Could not connect to server: %s:%d\n", ADDRESS, PORT);
        return -1;
    }

    //Can now send/receive data over the socket...

    //Ask for some text
    printf("Specify a remote filepath: ");
    char remote_filepath[1024];

    fgets(filepath, 1024, stdin);

    char response[1024];

    //Send the data over the socket:
    //  socket descriptor
    //  data (const void *)
    //  size of the data
    //  optional settings
    send(sockfd, filepath, strlen(s_line), 0);

    //Receive data over the socket:
    //  socket descriptor
    //  where to store data
    //  MAX_BYTES
    //  optional settings
    //Returns the number of bytes received...
    int n = recv(sockfd, r_line, 1024, 0);  //Receive is BLOCKING: Will wait for SOME data, but not necessarily until MAX_BYTES

    if(n < 0) {
        //Server has potentially closed the connection (check for specific error value)
        printf("Error receiving bytes from server...\n");
        return -1;
    } else if(n == 0) {
        printf("No bytes were received from server...\n");
    } else {
        printf("Received %d bytes from server: %s\n", n, r_line);
    }

    close(sockfd);

    return 0;
}
