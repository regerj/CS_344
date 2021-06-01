#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Error function used for reporting issues
void error(const char * msg)
{
    perror(msg);
    exit(1);
}

// Set up the address struct for the server socket

void setupAddressStruct(struct sockaddr_in * address, int portNumber)
{
    memset((char *) address, '\0', sizeof(*address));           // Clear address struct
    address->sin_family = AF_INET;                              // Make sure its network capable
    address->sin_port = htons(portNumber);                      // Store the port number
    address->sin_addr.s_addr = INADDR_ANY;                      // Allow clients of any address to connect
}

int main(int argc, char * argv[])
{
    int connectionSocket;
    int charsRead;
    char buffer[256];
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);

    if(argc < 2)                                                // If too few arguments
    {
        fprintf(stderr, "USAGE: %s port\n", argv[0]);           // Output errors
        exit(1);                                                // Exit with code 1
    }

    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);         // Instantiate the socket to listen for connections
    if(listenSocket < 0)                                        // If it didn't work
    {
        error("ERROR opening socket");                          // Output errors
    }

    setupAddresStruct(&serverAddress, atoi(argv[1]));           // Set up the address struct for the server socket

    // Bind the socket to the port
    if(bind(listenSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
    {
        error("ERROR on binding");                              // Output error if it failed
    }

    listen(listenSocket, 5);                                    // Begin listening for connections, and allow up to 5

    while(1)
    {
        connectionSocket = accept(listenSocket, (struct sockaddr *) &clientAddress, &sizeOfClientInfo);
        if(connectionSocket < 0)
        {
            error("ERROR on accept");
        }

        printf("SERVER: Connected to client running at host %d port %d\n", ntohs(clientAddress.sin_addr.s_addr), ntohs(clientAddress.sin_port));

        memset(buffer, '\0', 256);

        charsRead = recv(connectionSocket, buffer, 255, 0);
        if(charsRead < 0)
        {
            error("ERROR reading from socket");
        }
        printf("SERVER: I recieved this from the client : \"%s\"\n", buffer);

        charsRead = send(connectionSocket, "I am the server, and I got your message", 39, 0);
        if(charsRead < 0)
        {
            error("ERROR writing to socket");
        }

        close(connectionSocket);
    }

    close(listenSocket);
    return 0;
}