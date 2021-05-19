//#define _SVID_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <linux/limits.h>
#include <fcntl.h>

#define BUFF_SIZE 2049

bool forgroundToggle = false;
int exitStatus = 0;
int garbage = 0;

void execBackgroundCmd(pid_t childPID, const char* inputFileName, const char* outputFileName, char* const command, char* const args[], bool backgroundProcess);
void execCmd(char* const inputFileName, char* const outputFileName, int* exitStatus, char* const command, char* const args[], bool backgroundProcess);
void sigHandler(int sig);
void sigIntHandler(int sig);
void getStatus();

int main()
{
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, sigHandler);
    pid_t currPID;                                  // Current process ID
    int childArray[10] = { 0 };
    int childArrayCounter = 0;
    int childPID = 0;

    currPID = getpid();                             // Pull current process ID

    do
    {
        char input[BUFF_SIZE];                      // Getting a string for the input
        char* args[512] = {NULL};                   // Setting up an array of 512 pointer to possible commands
        int argsCounter = 0;                        // Counter for args
        char inputFileName[256] = { 0 };            // Name of possible redirected input file
        char outputFileName[256] = { 0 };           // Name of possible redirected output file
        char command[256];                          // String to store command
        bool backgroundProcess = false;             // Bool to represent if the command should be executed in background
        char* token;                                // Pointer to token for parsing
        char cwd[PATH_MAX];

        for(int i = 0; i < 10; i++)
        {
            if(childArray[i] != 0 && waitpid(childArray[i], &garbage, WNOHANG) != 0)
            {
                printf("Child process with ID: %d has terminated with exit code: %d\n", childArray[i], exitStatus);
                fflush(stdout);
                for(int j = i; j < 9; j++)
                {
                    childArray[j] = childArray[j + 1];
                }
                childArray[9] = 0;
            }
        }

        printf(": ");                               // Basic shell prompt
        fflush(stdout);                             // Flush for timing reasons

        fgets(input, BUFF_SIZE, stdin);             // Grab user input

        if(input[0] == '#' || input[0] == '\n')     // If its a comment
            continue;                               // Prompt again
        else
        {
            token = strtok(input, " ");             // Pull token from input
            strcpy(command, token);
            args[argsCounter] = strdup(token);
            argsCounter++;
        }

        token = strtok(NULL, " ");

        while(token != NULL)                        // While not end of tokens
        {
            if(strcmp(token, ">") == 0)             // If user is denoting redirected output file
            {
                token = strtok(NULL, " ");          // Pull next token from input
                strcpy(outputFileName, token);      // Save token as output file name
                //args[argsCounter] = strdup(token);
                //argsCounter++;
            }
            else if(strcmp(token, "<") == 0)        // If user is denoting redirected input file
            {
                token = strtok(NULL, " ");          // Pull next token from input
                strcpy(inputFileName, token);       // Save token as input file name
                //args[argsCounter] = strdup(token);
                //argsCounter++;
            }

            else if(strcmp(token, "&") == 0 || strcmp(token, "&\n") == 0)        // If user is denoting execute as background process
            {
                if(!forgroundToggle)
                    backgroundProcess = true;           // Set background bool as true
                break;                              // Break because & should be last item               
            }
            else
            {
                args[argsCounter] = strdup(token); // If not one of the above special cases, just save it to the args
                ////strcat(args[argsCounter], "\n");
                argsCounter++;
            }
            token = strtok(NULL, " ");              // Pull next token from input before re-looping
        }

        if(args[argsCounter - 1] != NULL)
        {
            char * temp;
            temp = strrchr(args[argsCounter - 1], '\n');
            if(temp != NULL)
                *temp = 0;
        }

        for(int i = 0; i < argsCounter; i++)
        {
            char* tempStr = strdup(args[i]);
            //printf("entered loop\n");
            for(int j = 0; j < strlen(tempStr); j++)
            {
                if(tempStr[j] == '$' && tempStr[j + 1] == '$')
                {
                    tempStr[j] = '%';
                    tempStr[j + 1] = 'd';
                    j++;
                }
            }
            sprintf(args[i], tempStr, currPID);
        }
        //printf("command: %s", command);
        //fflush(stdout);
        
        argsCounter = 0;
        /*while(args[argsCounter] != NULL)
        {
            printf("Arg%d: %s", argsCounter, args[argsCounter]);
            fflush(stdout);
            argsCounter++;
        }*/

        //getcwd(cwd, sizeof(cwd));
        //printf("Current working directory: %s\n", cwd);
        //fflush(stdout);

        if(strcmp(command, "cd") == 0 || strcmp(command, "cd\n") == 0)
        {
            if(args[1] == NULL)
            {
                if(chdir(getenv("HOME")) == -1)
                {
                    printf("Failed to change directory\n");
                    fflush(stdout);
                }
            }
            else
            {
                if(chdir(args[1]) == -1)
                {
                    printf("Failed to change directory\n");
                    fflush(stdout);
                }
            }
            if(getcwd(cwd, sizeof(cwd)) != NULL)
            {
                printf("Current working directory: %s\n", cwd);
                fflush(stdout);
            }
        }
        else if(strcmp(command, "exit\n") == 0)
        {
            exitStatus = 0;
            exit(0);
        }
        else if(strcmp(command, "status\n") == 0)
        {
            getStatus();
        }
        else
        {
            char* temp;
            temp = strrchr(command, '\n');
            if(temp != NULL)
                *temp = 0;

            childPID = fork();
            if(backgroundProcess)
            {
                childArray[childArrayCounter] = childPID;
                childArrayCounter++;
            }
            execBackgroundCmd(childPID, inputFileName, outputFileName, command, args, backgroundProcess);
            /*if(backgroundProcess)
            {
                childPID = fork();
                childArray[childArrayCounter] = childPID;
                childArrayCounter++;
                execBackgroundCmd(childPID, inputFileName, outputFileName, command, args, backgroundProcess);
            }
            else
            {
                execCmd(inputFileName, outputFileName, &exitStatus, command, args, backgroundProcess);
            }*/
        }
        //printf("Made it out of execCmd\n");
        //fflush(stdout);
    }
    while(1);
}

void getStatus()
{
    if(exitStatus == 2)
    {
        printf("Process terminated by signal with status: %i\n", exitStatus);
        fflush(stdout);
    }
    else
    {
        printf("Process terminated with status: %i\n", WEXITSTATUS(exitStatus));
        fflush(stdout);
    }
}

void sigHandler(int sig)
{
    if(sig == SIGTSTP)
    {
        forgroundToggle = !forgroundToggle;
        if(forgroundToggle)
        {
            printf("\nEntering forground only mode (& is ignored)\n: ");
            fflush(stdout);
        }
        else
        {
            printf("\nLeaving forground only mode (& is now allowed)\n: ");
            fflush(stdout);
        }
    }
    return;
}

void sigIntHandler(int sig)
{
    if(sig == SIGINT)
    {
        exitStatus = 2;
        printf("\n");
        exit(2);
    }
}

void execBackgroundCmd(pid_t childPID, const char* inputFileName, const char* outputFileName, char* const command, char* const args[], bool backgroundProcess)
{
    int inputFileDescriptor;
    int outputFileDescriptor;

    switch(childPID)
    {
        case -1:
            printf("Child creation unsuccessful!\n");
            fflush(stdout);
            perror("Error ");
            fflush(stdout);
            exitStatus = 1;
            exit(1);
            break;
        
        case 0:
            if(!backgroundProcess || forgroundToggle)
                signal(SIGINT, sigIntHandler);
            else
                signal(SIGINT, SIG_IGN);
            if(strcmp(inputFileName, "")!= 0)
            {
                char* temp;
                temp = strrchr(inputFileName, '\n');
                if(temp != NULL)
                    *temp = 0;

                inputFileDescriptor = open(inputFileName, O_RDONLY, 0644);
                if(inputFileDescriptor == -1)
                {
                    perror("Failed to open input file. Error ");
                    fflush(stdout);
                    exitStatus = 1;
                    exit(1);
                }

                if(dup2(inputFileDescriptor, STDIN_FILENO) == -1)
                {
                    perror("Failed to redirect input file. Error ");
                    fflush(stdout);
                    exitStatus = 1;
                    exit(1);
                }
                
                fcntl(inputFileDescriptor, F_SETFD, FD_CLOEXEC);
                //close(inputFileDescriptor);
                //free(inputFileDescriptor);
            }

            if(strcmp(outputFileName, "") != 0)
            {
                char* temp;
                temp = strrchr(outputFileName, '\n');
                if(temp != NULL)
                    *temp = 0;

                //printf("%s", outputFileName);
                fflush(stdout);
                outputFileDescriptor = open(outputFileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if(outputFileDescriptor == -1)
                {
                    perror("Failed to open output file. Error ");
                    fflush(stdout);
                    exitStatus = 1;
                    exit(1);
                }

                if(dup2(outputFileDescriptor, STDOUT_FILENO) == -1)
                {
                    perror("Failed to redirect output file. Error ");
                    fflush(stdout);
                    exitStatus = 1;
                    exit(1);
                }
                
                fcntl(outputFileDescriptor, F_SETFD, FD_CLOEXEC);
                //close(outputFileDescriptor);
                //free(outputFileDescriptor);
            }
            exitStatus = 0;
            if(execvp(command, args) == -1)
            {
                perror("Attempted to execute an invalid command! Error ");
                fflush(stdout);
                exitStatus = 1;
                exit(1);
            }
            exit(0);
            break;
        default:
            if(backgroundProcess)
            {
                printf("Child Process ID: %d\n", childPID);
                fflush(stdout);
            }
            else
            {
                waitpid(childPID, &exitStatus, 0);
            }
            break;
        
        return;
    }
}

void execCmd(char* const inputFileName, char* const outputFileName, int* exitStatus, char* const command, char* const args[], bool backgroundProcess)
{
    int inputFileDescriptor;
    int outputFileDescriptor;

    if(strcmp(inputFileName, "") != 0)
    {
        inputFileDescriptor = open(inputFileName, O_RDONLY);
        if(inputFileDescriptor == -1)
        {
            perror("Failed to open input file. Error ");
            *exitStatus = 1;
        }

        if(dup2(inputFileDescriptor, 0) == -1)
        {
            perror("Failed to redirect input file. Error ");
            *exitStatus = 1;
        }
        
        fcntl(inputFileDescriptor, F_SETFD, FD_CLOEXEC);
    }

    if(strcmp(outputFileName, "") != 0)
    {
        outputFileDescriptor = open(outputFileName, O_RDONLY);
        if(outputFileDescriptor == -1)
        {
            perror("Failed to open output file. Error ");
            *exitStatus = 1;
        }

        if(dup2(outputFileDescriptor, 0) == -1)
        {
            perror("Failed to redirect output file. Error ");
            *exitStatus = 1;
        }
        
        fcntl(outputFileDescriptor, F_SETFD, FD_CLOEXEC);
    }
    int tempChildPID = fork();
    //execBackgroundCmd(tempChildPID, inputFileName, outputFileName, command, args, backgroundProcess);
    //waitpid(tempChildPID, exitStatus, 0);
    /*if()
    {
        perror("Attempted to execute an invalid command! Error ");
        *exitStatus = 1;
    }
    */
    printf("End of execCmd\n");
    fflush(stdout);
}           