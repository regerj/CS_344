#include "Header.h"
#include "mem.h"
#include "userInterface.h"

int main(int argc, char* argv[])
{
	struct movie* head;
	struct movie* iter;
	head = cmdLineHandler(argc, argv);
	iter = head;

	int choice;
	while (1)
	{
		choice = dataAnalysisOptions();
		choiceHandler(choice, head);
	}

	return 0;
}