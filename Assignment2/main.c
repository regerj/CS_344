#include "Header.h"
// #include "dirent.h"
#include <dirent.h>
#include "Header.h"
#pragma warning(disable : 4996)

int main()
{
	while (1)
	{
		if (choiceLoop() == 0)
			break;
	}
	return 0;
}