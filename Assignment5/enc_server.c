#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

// TODO Code encryption function and add key stuff

char * encrypt(char * msg, char * key)
{
    char encMsg[70000];
    strcpy(encMsg, msg);
    //printf("encMsg before encoding: %s\n", encMsg);

    for(int i = 0; i < strlen(encMsg); i++)
    {
        if(encMsg[i] == 32)
            encMsg[i] = 91;
        if(key[i] == 32)
            key[i] = 91;
        int encChar = (encMsg[i]) + (key[i] - 65);
        if(encChar > 91)
            encChar = encChar - 27;
        if(encChar == 91)
            encChar = 32;
        encMsg[i] = (char)encChar;
    }

    return encMsg;
}

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
    int charsWritten;
    char buffer[150000];
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);

    if(argc < 2)                                                // If too few arguments
    {
        fprintf(stderr, "USAGE: %s port\n", argv[0]);           // Output errors
        fflush( stdout );
        exit(1);                                                // Exit with code 1
    }

    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);         // Instantiate the socket to listen for connections
    if(listenSocket < 0)                                        // If it didn't work
    {
        error("ERROR opening socket");                          // Output errors
    }

    setupAddressStruct(&serverAddress, atoi(argv[1]));           // Set up the address struct for the server socket

    // Bind the socket to the port
    if(bind(listenSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
    {
        error("ERROR on binding");                              // Output error if it failed
    }

    listen(listenSocket, 5);                                    // Begin listening for connections, and allow up to 5

    while(1)
    {
        // Accept the connection and grab client address info
        connectionSocket = accept(listenSocket, (struct sockaddr *) &clientAddress, &sizeOfClientInfo);
        if(connectionSocket < 0)                                // If it failed to accept
        {
            error("ERROR on accept");                           // Output error
        }

        pid_t childPID = -1;
        int childExitStatus = -1;
        childPID = fork();

        switch(childPID)
        {
            case -1: 
                error("Invalid child!\n");
                break;
            case 0:
                // Debug output 
                printf("SERVER: Connected to client running at host %d port %d\n", ntohs(clientAddress.sin_addr.s_addr), ntohs(clientAddress.sin_port));
                fflush( stdout );
                memset(buffer, '\0', 150000);                              // Clear buffer

                charsRead = recv(connectionSocket, buffer, 149999, 0);     // Recieve communication into the buffer of max size 255
                if(charsRead < 0)                                       // If the recieve failed
                {
                    error("ERROR reading from socket");                 // Output error
                }

                //printf("Recieved %d chars\n", charsRead);
                //fflush( stdout );

                // Output the recieved bytes
                //printf("SERVER: I recieved this from the client : \"%s\"\n", buffer);
                //fflush( stdout );

                char msg[70000];
                char key[80000];
                char ID[2];
                char * token = strtok(buffer, "!");
                //printf("Made it past first tokenize: %s\n", token);
                //fflush( stdout );
                strcpy(msg, token);
                token = strtok(NULL, "!");
                //printf("Made it past the second tokenize: %s\n", token);
                //fflush( stdout );
                strcpy(key, token);
                token = strtok(NULL, "!");
                strcpy(ID, token);
                if(strncmp(ID, "E", 1) != 0)
                {
                    close(connectionSocket);
                    error("SERVER: INVALID client connecting, terminated connection\n");
                }

                if(strlen(msg) > strlen(key))
                {
                    error("ENC_SERVER: ERROR msg is longer than key\n");
                }

                //printf("Made it past tokenizing\n");
                //fflush( stdout );

                char * encMsg = encrypt(msg, key);

                //printf("encMsg: %s\n", encMsg);
                //fflush( stdout );

                // Send back an ACK
                //printf("Size of encMsg: %d\n", strlen(encMsg));
                charsWritten = send(connectionSocket, encMsg, strlen(encMsg), 0);
                //printf("Sent encMsg\n");
                fflush( stdout );
                if(charsWritten < 0)                                       // If the message failed to send
                {
                    error("ERROR writing to socket");                   // Output error
                }
                exit(0);
                break;
            default:
                close(connectionSocket);
                pid_t temp = waitpid(childPID, &childExitStatus, WNOHANG);
                break;
        }

        

        //close(connectionSocket);                                // Hang up
    }

    close(listenSocket);
    return 0;
}