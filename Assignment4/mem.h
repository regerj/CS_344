#ifndef MEM
#define MEM

#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

extern char bufferOne[50][1000];
extern pthread_mutex_t mutexOne;
extern pthread_cond_t fullOne;
extern int sizeOne[50];
extern int indexToLoadOne;
extern int indexToUnloadOne;

extern char bufferTwo[50][1000];
extern pthread_mutex_t mutexTwo;
extern pthread_cond_t fullTwo;
extern int sizeTwo[50];
extern int indexToLoadTwo;
extern int indexToUnloadTwo;

extern char bufferThree[50][1000];
extern pthread_mutex_t mutexThree;
extern pthread_cond_t fullThree;
extern int sizeThree[50];
extern int indexToLoadThree;
extern int indexToUnloadThree;

void * threadOne();
void * threadTwo();
void * threadThree();
void * threadFour();

void loadBufferOne(char payload[1000]);
void loadBufferTwo(char payload[1000]);
void loadBufferThree(char payload[1000]);

char * unloadBufferOne();
char * unloadBufferTwo();
char * unloadBufferThree();

void nukeChar(int index, char * data);

#endif