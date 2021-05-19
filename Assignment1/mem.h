#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct movie* cmdLineHandler(int argc, char* argv[]);
struct movie* sortMovieList(struct movie* head);
void clearMemory(struct movie* head);