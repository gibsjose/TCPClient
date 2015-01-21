//CIS 452 Lab 2 - TCP Client
//Joe Gibson
//Seth Hilaski
//Adam Luckenbaugh
//Raleigh Mumford

#include <sys/socket.h> //Socket features
#include <netinet/in.h> //Internet-specific features of sockets
#include <arpa/inet.h>  //inet_addr()
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

//Define the default port and address
#define PORT 8888
#define ADDRESS "127.0.0.1"

#define MAX_FILE_SIZE (50 * 1024 * 1024)

#define ERR_FILE_NOT_FOUND  0xE0
#define ERR_FILE_TOO_LARGE  0xE1

void ntrim(char *str);

int main(int argc, char *argv[]) {

    char port_str[16];
    char address[64];
    unsigned int port;
    FILE *local_file;

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

    //Prompt user for server address and port
    printf("Server address: ");
    fgets(address, 64, stdin);
    ntrim(address);

    printf("Server port: ");
    fgets(port_str, 16, stdin);
    ntrim(port_str);
    port = atoi(port_str);

    printf("===========================================\n");
    printf("Connecting to %s:%d\n", address, port);
    printf("===========================================\n\n");

    //Specify the address for the socket
    //Create the socket address structure and populate it's fields
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;                        //Specify the family again (AF_INET = internet family)
    serveraddr.sin_port = htons(port);                      //Specify the port on which to send data (16-bit) (# < 1024 is off-limits)
    serveraddr.sin_addr.s_addr = inet_addr(address);        //Specify the IP address of the server with which to communicate
                                                            //inet_addr() converts a string-address into the proper type

    //Open TCP "Connection"
    //Specify the socket descriptor, the server address structure, and the size of the socket address structure
    int e = connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(struct sockaddr_in));

    if(e < 0) {
        printf("Could not connect to server: %s:%d\n", address, port);
        return -1;
    }

    //Can now send/receive data over the socket...

    //Ask for the remote/local file
    printf("Specify a remote filepath: ");
    char remote_filepath[1024];
    fgets(remote_filepath, 1024, stdin);
    ntrim(remote_filepath);

    //Obviously this is not an efficient way to do this...
    char * response = malloc(MAX_FILE_SIZE);

    //Send the data over the socket:
    //  socket descriptor
    //  data (const void *)
    //  size of the data
    //  optional settings
    send(sockfd, remote_filepath, strlen(remote_filepath), 0);

    //Receive data over the socket:
    //  socket descriptor
    //  where to store data
    //  MAX_BYTES
    //  optional settings
    //Returns the number of bytes received...
    int n = recv(sockfd, response, MAX_FILE_SIZE, 0);  //Receive is BLOCKING: Will wait for SOME data, but not necessarily until MAX_BYTES

    //Error
    if(n <= 0) {
        //Server has potentially closed the connection (check for specific error value)
        printf("Error receiving bytes from server...\n");
        return -1;
    }

    //If one byte was received, check to see if it was an error response
    if(n == 1) {
        if((unsigned char) *response == ERR_FILE_NOT_FOUND) {
            printf("Error: Remote file does not exist\n");
            return -1;
        }

        else if((unsigned char) *response == ERR_FILE_TOO_LARGE) {
            printf("Error: The file requested exceeds the file limit of %d bytes", MAX_FILE_SIZE);
            return -1;
        }
    }

    printf("Received %d bytes from server\n", n);

    //Close the socket
    close(sockfd);

    //Save the buffer to a file in the current directory
    char remote_filename[1024];
    char local_filepath[1024] = "./";

    //Get the NAME of the remote file requested
    strcpy(remote_filename, basename(remote_filepath));

    //Form a local filepath with the remote file name
    strcat(local_filepath, remote_filename);

    printf("Successfully obtained remote file: %s\n", remote_filename);
    printf("Saving locally to: %s\n", local_filepath);

    //Write the local file
    local_file = fopen(local_filepath, "wb");
    fwrite(response, sizeof(char), n, local_file);

    free(response);

    return 0;
}

//Trim the first newline character from the string
void ntrim(char *str) {
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
}
