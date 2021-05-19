#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

struct movie* memHandler(char* fileName);
struct movie* sortMovieList(struct movie* head);
void processFile(struct movie* head);
void clearMemory(struct movie* head);