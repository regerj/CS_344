#ifndef MEM
#define MEM

// Includes
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

// Extern globals from main.c
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

// Find STOP function
bool findSTOP(char * str);

// Thread functions
void * threadOne();
void * threadTwo();
void * threadThree();
void * threadFour();

// Load buffer functions
void loadBufferOne(char payload[1000]);
void loadBufferTwo(char payload[1000]);
void loadBufferThree(char payload[1000]);

// Unload buffer functions
char * unloadBufferOne();
char * unloadBufferTwo();
char * unloadBufferThree();

// Nuke single char funciton
void nukeChar(int index, char * data);

#endif