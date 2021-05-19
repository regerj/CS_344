#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "mem.h"

// TODO FIX KILL CODE
// TODO Make size ints into an array to avoid overwriting values
// TODO Make a num of lines in buffer variable and implement it to avoid pthread_cond_wait();

// GLOBAL VARIABLE DEFS
char bufferOne[50][1000];                                   // Buffer
pthread_mutex_t mutexOne = PTHREAD_MUTEX_INITIALIZER;       // Mutex
pthread_cond_t fullOne = PTHREAD_COND_INITIALIZER;          // Signal
int sizeOne[50] = { 0 };                                    // Size array
int indexToLoadOne = 0;                                     // Index to load
int indexToUnloadOne = 0;                                   // Index to unload

// See above for comments
char bufferTwo[50][1000];
pthread_mutex_t mutexTwo = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fullTwo = PTHREAD_COND_INITIALIZER;
int sizeTwo[50] = { 0 };
int indexToLoadTwo = 0;
int indexToUnloadTwo = 0;

// See above for comments
char bufferThree[50][1000];
pthread_mutex_t mutexThree = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fullThree = PTHREAD_COND_INITIALIZER;
int sizeThree[50] = { 0 };
int indexToLoadThree = 0;
int indexToUnloadThree = 0;

int main(int argc, char* argv)
{
    ////srand(time(0));
    pthread_t threadOneHandle, threadTwoHandle, threadThreeHandle, threadFourHandle;    // Initiate handles to threads
    pthread_create(&threadOneHandle, NULL, threadOne, NULL);                            // Create thread one
    pthread_create(&threadTwoHandle, NULL, threadTwo, NULL);                            // Create thread two
    pthread_create(&threadThreeHandle, NULL, threadThree, NULL);                        // Create thread three
    pthread_create(&threadFourHandle, NULL, threadFour, NULL);                          // Create thread four

    pthread_join(threadOneHandle, NULL);                                                // Waits for specified thread then terminates
    pthread_join(threadTwoHandle, NULL);                                                // Waits for specified thread then terminates
    pthread_join(threadThreeHandle, NULL);                                              // Waits for specified thread then terminates
    pthread_join(threadFourHandle, NULL);                                               // Waits for specified thread then terminates
    return 0;
}