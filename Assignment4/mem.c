#include "mem.h"

bool findSTOP(char * str)                                                   // Finds substring STOP in string passed in, only just STOP
{
    char str2[1000];                                                        // Declaration for safety
    strcpy(str2, str);                                                      // Copy for safety
    const char search[5] = "STOP";                                          // Search for
    const char search2[5] = "STOP\n";                                       // Search for alt
    const char delim[2] = " ";                                              // Delim

    char *token;                                                            // Token declaration

    bool end = false;                                                       // Bool for ret

    token = strtok(str2, delim);                                            // Tokenize the string with delim

    while (token != NULL)                                                   // While not end of string
    {                           
        if(strcmp(token, search) == 0 || strcmp(token, search2) == 0)       // Check for either search terms
        {
            end = true;                                                     // If so toggle end
            break;                                                          // And break
        }
        token = strtok(NULL, delim);                                        // Tokenize again
    }
    
    // DEBUG
    if(end)
    {
        //printf("ENDING\n");
    }

    return end;                                                             // Ret
}

void * threadOne()
{
    // Read in a line of the input and search for STOP substring
    char input[1000] = "";
    char * fgetsStatus = NULL;
    fgetsStatus = fgets(input, 1000, stdin);
    
    bool end = false;
    end = findSTOP(input);

    // Locate substring STOP
    char * status = strstr(input, "STOP");

    while(!end && input[0] != '\n' && indexToLoadOne < 49)
    {
        loadBufferOne(input);                                       // Load into the buffer
        fgetsStatus = fgets(input, 1000, stdin);                    // Grab the next line of input
        status = strstr(input, "STOP");                             // Locate substring STOP
        end = findSTOP(input);
    }
    char kill[1000];                                                // Kill code
    kill[0] = '\n';                                                 // Just an empty newline
    loadBufferOne(kill);                                            // Send killcode down the pipeline
}

void loadBufferOne(char payload[1000])
{
    pthread_mutex_lock(&mutexOne);                                  // Lock the mutex
    strcpy(bufferOne[indexToLoadOne], payload);                     // Deliver payload
    sizeOne[indexToLoadOne] = strlen(bufferOne[indexToLoadOne]);    // Move sizes
    pthread_cond_signal(&fullOne);                                  // Signal consumer
    pthread_mutex_unlock(&mutexOne);                                // Unlock the mutex
    ++indexToLoadOne;                                               // Incrememnt index
}

char * unloadBufferOne()
{
    pthread_mutex_lock(&mutexOne);                                  // Lock the mutex
    if(indexToLoadOne == indexToUnloadOne)                          // If buffer is empty
        pthread_cond_wait(&fullOne, &mutexOne);                     // Wait until its not
    char data[1000] = "";                                           // Data declaration
    strcpy(data, bufferOne[indexToUnloadOne]);                      // Unload data
    pthread_mutex_unlock(&mutexOne);                                // Unlock the mutex
    ++indexToUnloadOne;                                             // Increment index

    return data;                                                    // Return unloaded data
}

void * threadTwo()
{
    //printf("Thread two initialized\n");
    char data[1000] = "";                                           // Data declaration
    char * tempStr = unloadBufferOne();                             // Unload data from buffer
    //printf("Unloaded buffer one with: %s\n", tempStr);
    strcpy(data, tempStr);                                          // Move data
    ////printf("threadTwo reading data: %s\n", data);
    while(data[0] != '\n')                                          // While data is not killcode
    {
        char * ptr;                                                 
        ptr = strchr(data, '\n');                                   // Search for newline char
        if(ptr)                                                     // If you find
            *ptr = ' ';                                             // Replace with space char
        sizeTwo[indexToLoadTwo] = sizeOne[indexToUnloadOne - 1];    // Move size variable to next buffer
        loadBufferTwo(data);                                        // Load new data into buffer 2
        tempStr = unloadBufferOne();                                // Unload buffer 1
        strcpy(data, tempStr);                                      // Move data
    }
    if(data[0] == '\n')                                             // If killcode recieved
        loadBufferTwo(data);                                        // Send down pipeline
    return NULL;                                                    // Ret
}

void loadBufferTwo(char payload[1000])
{
    pthread_mutex_lock(&mutexTwo);                                  // Lock the mutex
    strcpy(bufferTwo[indexToLoadTwo], payload);                     // Deliver payload
    pthread_cond_signal(&fullTwo);                                  // Signal consumer
    pthread_mutex_unlock(&mutexTwo);                                // Unlock the mutex
    ++indexToLoadTwo;                                               // Increment index
    return;                                                         // Return control
}

char * unloadBufferTwo()
{
    pthread_mutex_lock(&mutexTwo);                                  // Lock the mutex
    if(indexToLoadTwo == indexToUnloadTwo)                          // If buffer is empty
        pthread_cond_wait(&fullTwo, &mutexTwo);                     // Wait until its not
    char data[1000] = "";                                           // Data declaration
    strcpy(data, bufferTwo[indexToUnloadTwo]);                      // Unload data
    pthread_mutex_unlock(&mutexTwo);                                // Unlock the mutex
    ++indexToUnloadTwo;                                             // Increment index

    return data;                                                    // Return unloaded data
}

void nukeChar(int index, char * data)
{
    memmove(&data[index], &data[index + 1], strlen(data) - index);  // Overwrite the char
    sizeThree[indexToLoadThree]--;                                  // Decrement size
    return;                                                         // Ret
}

void * threadThree()
{
    char data[1000] = "";                                           // Declaration
    char * tempStr;                                                 // Declaration
    tempStr = unloadBufferTwo();                                    // Unload from buffer 2
    sizeThree[indexToLoadThree] = sizeTwo[indexToUnloadTwo - 1];    // Move sizes
    strcpy(data, tempStr);                                          // Move data

    while(data[0] != '\n')                                          // While not killcode
    {
        for(int i = 0; i < 1000; i++)                               // Parse data
        {
            if(data[i] == '+' && data[i + 1] == '+')                // If double plus found
            {
                data[i] = '^';                                      // Replace first plus with caret
                nukeChar(i + 1, data);                              // Nuke second plus
            }
            else if(data[i] == '\n')                                // If find a newline
                break;                                              // Break
        }
        loadBufferThree(data);                                      // Load data into buffer 3
        tempStr = unloadBufferTwo();                                // Unload next piece
        sizeThree[indexToLoadThree] = sizeTwo[indexToUnloadTwo - 1];// Move sizes
        strcpy(data, tempStr);                                      // Move data
    }
    if(data[0] == '\n')                                             // If killcode
        loadBufferThree(data);                                      // Load killcode
    return NULL;                                                    // Ret
}

void loadBufferThree(char payload[1000])
{
    pthread_mutex_lock(&mutexThree);                                // Lock the mutex
    strcpy(bufferThree[indexToLoadThree], payload);                 // Deliver payload
    pthread_cond_signal(&fullThree);                                // Signal consumer
    pthread_mutex_unlock(&mutexThree);                              // Unlock the mutex
    ++indexToLoadThree;                                             // Increment index
    return;                                                         // Return control
}

char * unloadBufferThree()
{
    pthread_mutex_lock(&mutexThree);                                // Lock mutex
    if(indexToLoadThree == indexToUnloadThree)                      // If buffer empty
        pthread_cond_wait(&fullThree, &mutexThree);                 // Wait for buffer to fill
    char data[1000] = "";                                           // Declaration
    strcpy(data, bufferThree[indexToUnloadThree]);                  // Unload data
    pthread_mutex_unlock(&mutexThree);                              // Unlock mutex
    ++indexToUnloadThree;                                           // Incrememnt index

    return data;                                                    // Ret
}

void * threadFour()
{
    char data[1080] = "";                                           // Declaration
    char * tempStr = unloadBufferThree();                           // Unload data from buffer 3
    int localSize = sizeThree[indexToUnloadThree - 1];              // Grab size
    strcpy(data, tempStr);                                          // Move data

    while(tempStr[0] != '\n')                                       // While not killcode
    {
        while(localSize < 80)                                       // While size is not enough to display a line
        {
            char temp[1000];                                        // Declaration
            tempStr = unloadBufferThree();                          // Unload another line
            if(tempStr[0] == '\n')                                  // If killcode
                break;                                              // Break
            strcpy(temp, tempStr);                                  // Otherwise move to a new location
            strcat(data, temp);                                     // Concatonate strings
            localSize = localSize + sizeThree[indexToUnloadThree - 1];  // Change size appropriately
        }
        if(tempStr[0] == '\n')                                      // If killcode
            break;                                                  // Break
        fwrite(data, sizeof(char), 80, stdout);                     // Write 80 chars to stdout
        fwrite("\n", sizeof(char), 1, stdout);                      // Write newline to stdout
        fflush(stdout);                                             // Flush
        char overwrite[1080] = "";                                  // Declaration
        
        memmove(&overwrite[0], &data[80], localSize - 80);          // Nuke 80 chars just written 
        strcpy(data, overwrite);                                    // Push nuked string
        localSize = localSize - 80;                                 // Adjust size accordingly
    }

    return NULL;                                                    // Ret
}