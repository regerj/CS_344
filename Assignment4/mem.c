#include "mem.h"

void * threadOne()
{
    // Read in a line of the input and search for STOP substring
    char input[1000] = "";
    char * fgetsStatus = NULL;
    fgetsStatus = fgets(input, 1000, stdin);
    //printf("Read in line: %s", input);
    //scanf("%s", input);

    /*
    //! BEGINING TEST CODE

    char delim[2] = " \n"; 
    char * token = NULL;
    token = strtok(input, delim);
    bool end = false;
    while(token)
    {
        if(strncmp(token, "STOP", 4) == 0 && (*(token + 4) == ' ' || *(token + 4) == '\n'))
        {
            printf("Found end at beginning with line %s\n", token);
            end = true;
            break;
        }
        token = strtok(NULL, delim);
    }

    //! ENDING TEST CODE
    */
    
    char * status = strstr(input, "STOP");
    ////printf("input[0]: %c\n", input[0]);

    while(!status && input[0] != '\n' && indexToLoadOne < 49)
    //while(!end && input[0] != '\n' && indexToLoadOne < 49) //! TEST CODE
    {
        //printf("%s", input);
        //if(status)
            //pthread_exit(NULL);
        loadBufferOne(input);
        ////printf("Loaded buffer one\n");
        fgetsStatus = fgets(input, 1000, stdin);
        //scanf("%s", input);
        status = strstr(input, "STOP");
        /*
        //! BEGINING TEST CODE

        token = strtok(input, delim);
        while(token)
        {
            if(strncmp(token, "STOP", 4) == 0 && (*(token + 4) == ' ' || *(token + 4) == '\n'))
            {
                printf("Found end at mid with line %s\n", input);
                end = true;
                break;
            }
            token = strtok(NULL, delim);
        }

        //! ENDING TEST CODE
        */
        ////printf("input[0]: %c\n", input[0]);
    }
    char kill[1000];
    kill[0] = '\n';
    loadBufferOne(kill);
    ////printf("Thread one died\n");
    ////printf("indexToLoadOne: %d\n", indexToLoadOne);
    //pthread_exit(NULL);
}

void loadBufferOne(char payload[1000])
{
    ////printf("attempting to load buffer one\n");
    pthread_mutex_lock(&mutexOne);      // Lock the mutex
    ////printf("Locked mutex one\n");
    strcpy(bufferOne[indexToLoadOne], payload);
    ////printf("Loading this line into buffer one: %s\n", bufferOne[indexToLoadOne]);
    ////printf("Payload delivered\n");
    sizeOne[indexToLoadOne] = strlen(bufferOne[indexToLoadOne]);
    ////printf("bufferOne contains: %s\n", bufferOne[indexToLoadOne]);
    //bufferOne = payload;                // Load the payload
    pthread_cond_signal(&fullOne);      // Signal consumer
    pthread_mutex_unlock(&mutexOne);    // Unlock the mutex
    ++indexToLoadOne;
}

char * unloadBufferOne()
{
    ////printf("Waiting to unload buffer 1\n");
    pthread_mutex_lock(&mutexOne);
    if(indexToLoadOne == indexToUnloadOne)
        pthread_cond_wait(&fullOne, &mutexOne);
    ////printf("Done waiting\n");
    char data[1000] = "";
    strcpy(data, bufferOne[indexToUnloadOne]);
    ////bufferOne[indexToUnloadOne][0] = '\n';
    pthread_mutex_unlock(&mutexOne);
    ++indexToUnloadOne;

    return data;
}

void * threadTwo()
{
    //printf("Thread two initialized\n");
    char data[1000] = "";
    char * tempStr = unloadBufferOne();
    //printf("Unloaded buffer one with: %s\n", tempStr);
    strcpy(data, tempStr);
    ////printf("threadTwo reading data: %s\n", data);
    while(data[0] != '\n')
    {
        char * ptr;
        ptr = strchr(data, '\n');
        if(ptr)
            *ptr = ' ';
        sizeTwo[indexToLoadTwo] = sizeOne[indexToUnloadOne - 1];
        loadBufferTwo(data);
        tempStr = unloadBufferOne();
        strcpy(data, tempStr);
    }
    if(data[0] == '\n')
        loadBufferTwo(data);
    ////printf("Thread two died\n");
    return NULL;
}

void loadBufferTwo(char payload[1000])
{
    ////printf("Loading buffer two\n");
    pthread_mutex_lock(&mutexTwo);      // Lock the mutex
    strcpy(bufferTwo[indexToLoadTwo], payload);
    //bufferTwo = payload;                // Load the payload
    pthread_cond_signal(&fullTwo);      // Signal consumer
    pthread_mutex_unlock(&mutexTwo);    // Unlock the mutex
    ++indexToLoadTwo;
    return;                             // Return control
}

char * unloadBufferTwo()
{
    ////printf("unloading buffer 2\n");
    pthread_mutex_lock(&mutexTwo);
    if(indexToLoadTwo == indexToUnloadTwo)
        pthread_cond_wait(&fullTwo, &mutexTwo);
    char data[1000] = "";
    strcpy(data, bufferTwo[indexToUnloadTwo]);
    ////bufferTwo[indexToUnloadTwo][0] = '\n';
    pthread_mutex_unlock(&mutexTwo);
    ++indexToUnloadTwo;

    return data;
}

void nukeChar(int index, char * data)
{
    // TODO FIX THIS THIRD OPERATOR, NOT A NULL TERMINATED STRING
    memmove(&data[index], &data[index + 1], strlen(data) - index);
    sizeThree[indexToLoadThree]--;
    return;
}

void * threadThree()
{
    ////printf("ThreadThree init\n");
    char data[1000] = "";
    char * tempStr;
    tempStr = unloadBufferTwo();
    sizeThree[indexToLoadThree] = sizeTwo[indexToUnloadTwo - 1];
    ////printf("Unloaded buffer 2\n");
    strcpy(data, tempStr);

    while(data[0] != '\n')
    {
        for(int i = 0; i < 1000; i++)
        {
            if(data[i] == '+' && data[i + 1] == '+')
            {
                data[i] = '^';
                nukeChar(i + 1, data);
            }
            else if(data[i] == '\n')
                break;
        }
        loadBufferThree(data);
        tempStr = unloadBufferTwo();
        sizeThree[indexToLoadThree] = sizeTwo[indexToUnloadTwo - 1];
        strcpy(data, tempStr);
    }
    if(data[0] == '\n')
        loadBufferThree(data);
    ////printf("Thread three died\n");
    return NULL;
}

void loadBufferThree(char payload[1000])
{
    ////printf("Loading buffer three with %s\n", payload);
    pthread_mutex_lock(&mutexThree);    // Lock the mutex
    strcpy(bufferThree[indexToLoadThree], payload);
    
    //bufferThree = payload;              // Load the payload
    pthread_cond_signal(&fullThree);    // Signal consumer
    pthread_mutex_unlock(&mutexThree);  // Unlock the mutex
    ++indexToLoadThree;
    return;                             // Return control
}

char * unloadBufferThree()
{
    ////printf("unloading buffer 3\n");
    pthread_mutex_lock(&mutexThree);
    if(indexToLoadThree == indexToUnloadThree)
        pthread_cond_wait(&fullThree, &mutexThree);
    char data[1000] = "";
    strcpy(data, bufferThree[indexToUnloadThree]);
    ////bufferThree[indexToUnloadThree][0] = '\n';
    pthread_mutex_unlock(&mutexThree);
    ++indexToUnloadThree;

    return data;
}

void * threadFour()
{
    ////printf("ThreadFour init\n");
    char data[1080] = "";
    char * tempStr = unloadBufferThree();
    int localSize = sizeThree[indexToUnloadThree - 1];
    strcpy(data, tempStr);

    while(tempStr[0] != '\n')
    {
        ////printf("In thread 4 while loop\n");
        while(localSize < 80)
        {
            ////printf("In nested while loop\n");
            char temp[1000];
            tempStr = unloadBufferThree();
            if(tempStr[0] == '\n')
                break;
            ////printf("Past unload in nested while loop\n");
            strcpy(temp, tempStr);
            strcat(data, temp);
            ////printf("New string after cat is: %s\n", data);
            localSize = localSize + sizeThree[indexToUnloadThree - 1];
        }
        if(tempStr[0] == '\n')
            break;
        ////printf("Past nested while loop\n");
        fwrite(data, sizeof(char), 80, stdout);
        fwrite("\n", sizeof(char), 1, stdout);
        fflush(stdout);
        char overwrite[1080] = "";
        
        memmove(&overwrite[0], &data[80], localSize - 80);
        strcpy(data, overwrite);
        localSize = localSize - 80;
    }

    ////printf("Thread four died\n");
    return NULL;
}