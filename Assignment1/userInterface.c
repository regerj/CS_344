#pragma warning(disable : 4996)
#include "userInterface.h"

int dataAnalysisOptions()
{
	int choice;

	printf("\nChoose an option below:\n");
	printf("\t1.\tShow movies released in the specified year\n");
	printf("\t2.\tShow highest rated movie for each year\n");
	printf("\t3.\tShow the title and year of release of all movies in a specific language\n");
	printf("\t4.\tExit the program\n\n");

	do
	{
		printf("Enter a choice from 1 to 4: ");
		scanf("%d", &choice);

		if (choice < 1 || choice > 4)
			printf("You entered an incorrect choice. Try again.\n");

	} 	while (choice < 1 || choice > 4);

	return choice;
}

void choiceHandler(int choice, struct movie* head)
{
	struct movie* iter = head;
	bool found = false;

	switch (choice)
	{
	case 1:
	{
		int year;
		printf("Enter the year for which you want to see movies: ");
		scanf("%d", &year);
		printf("\n");

		while (iter != NULL)
		{
			if (iter->year == year)
			{
				printf("\t%s\n", iter->title);
				found = true;
			}

			iter = iter->next;
		}

		if (!found)
			printf("No data about movies released in the year %d", year);
		break;
	}

	// TODO FINISH THIS OPTION, CANT BE BOTHERED TO THINK ABOUT IT RN
	case 2:
	{
		printf("\n");
		//struct movie* tempIter = NULL;
		struct movie* yearHighScore = NULL;

		//tempIter = iter->next;
		//int yearIndex = 0;

		yearHighScore = iter;

		while (iter != NULL)
		{
			if (iter->rating > yearHighScore->rating && iter->year == yearHighScore->year)
				yearHighScore = iter;
			if (iter->next != NULL && iter->year != iter->next->year)
			{
				printf("\t%d, %.1f, %s\n", yearHighScore->year, yearHighScore->rating, yearHighScore->title);
				yearHighScore = iter->next;
			}
			if (iter->next == NULL)
				printf("\t%d, %.1f, %s\n", yearHighScore->year, yearHighScore->rating, yearHighScore->title);
			iter = iter->next;
		}

		break;
	}

	case 3:
	{
		char language[20];
		printf("Enter the language for which you want to see movies: ");
		scanf("%s", language);
		printf("\n");

		while (iter != NULL)
		{
			if (!strcmp(iter->lang[0], language) || !strcmp(iter->lang[1], language) || !strcmp(iter->lang[2], language) || !strcmp(iter->lang[3], language) || !strcmp(iter->lang[4], language))
			{
				printf("\t%d %s\n", iter->year, iter->title);
				found = true;
			}

			iter = iter->next;
		}

		if (!found)
			printf("No data about movies released in %s", language);
		break;
	}

	case 4:
	{
		clearMemory(head);
		exit(0);
	}
	}
}