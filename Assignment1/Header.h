#pragma once
#include <string.h>
//#include <cstdlib>
#include <stdint.h>
#include <stdio.h>

#include "mem.h"
#include "userInterface.h"

struct movie
{
	char*			title;
	int				year;
	int				langCount;
	char			lang[5][20];
	double			rating;
	struct movie*	next;
};