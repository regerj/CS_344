#pragma warning(disable : 4996)
#include "Header.h"
#include "mem.h"

struct movie* memHandler(char* fileName)
{
	FILE* fp = fopen(fileName, "r");
	char buff[255];						// Initialize buffer
	char* EoF = NULL;
	EoF = fgets(buff, 255, fp);	// Read in first line garbage
	EoF = fgets(buff, 255, fp);
	char* token;
	char* tokenLang;
	char* subtoken;
	const char delimComma[2] = ",";
	const char delimSemiColon[2] = ";";
	const char delimBracket1[2] = "[";
	const char delimBracket2[2] = "]";

	struct movie* head = NULL;
	struct movie* iter = NULL;
	struct movie* tempIter = NULL;

	head = (struct movie*)malloc(sizeof(struct movie));
	iter = head;
	int count = 0;

	while (EoF != NULL)
	{
		count++;
		if (count != 1)
		{
			tempIter = iter;
			iter = (struct movie*)malloc(sizeof(struct movie));
			tempIter->next = iter;
		}

		// Pull title
		token = strtok(EoF, delimComma);
		int temp = strlen(token);
		iter->title = (char*)malloc(temp * sizeof(char) + 1);
		strcpy(iter->title, token);

		// Pull year
		token = strtok(NULL, delimComma);
		iter->year = atoi(token);

		// Pull but do not parse languages, done later because using strtok
		token = strtok(NULL, delimBracket2);
		tokenLang = token;

		// Pull rating
		token = strtok(NULL, delimComma);
		iter->rating = strtod(token, NULL);

		// Now we parse languages
		subtoken = strtok(tokenLang, delimSemiColon);
		int i = 0;
		while (subtoken != NULL)
		{
			++i;
			iter->langCount = i;
			if (subtoken[0] == '[')
			{
				memmove(&subtoken[0], &subtoken[1], strlen(subtoken));
			}
			strcpy(iter->lang[i - 1], subtoken);
			subtoken = strtok(NULL, delimSemiColon);
		}

		// Finally grab next movie line for while loop
		EoF = fgets(buff, 255, fp);
	}

	printf("Processed file %s and parsed data for %d movies\n", fileName, count);

	iter->next = NULL;

	head = sortMovieList(head);

	fclose(fp);

	return head;
}

struct movie* sortMovieList(struct movie* head)
{
	struct movie* prev = NULL;
	struct movie* iter = NULL;
	struct movie* temp1 = NULL;
	struct movie* temp2 = NULL;
	struct movie* output = NULL;
	bool swapped = false;

	iter = head;
	temp1 = iter->next;

	while (swapped != false || temp1 != NULL)
	{
		if (temp1 == NULL)
		{
			iter = head;
			temp1 = iter->next;
			prev = NULL;
			swapped = false;
		}
		else
		{

			if (temp1->year < iter->year)
			{
				if (iter == head)
					head = temp1;
				if (prev != NULL)
				{
					prev->next = temp1;
					temp2 = temp1->next;
					temp1->next = iter;
					iter->next = temp2;
					temp1 = iter->next;
					swapped = true;
					prev = prev->next;
				}
				else
				{
					prev = temp1;
					temp2 = temp1->next;
					temp1->next = iter;
					iter->next = temp2;
					temp1 = iter->next;
					swapped = true;
				}
			}
			else
			{
				if (prev != NULL)
				{
					prev = prev->next;
					iter = iter->next;
					temp1 = temp1->next;
				}
				else
				{
					prev = iter;
					iter = iter->next;
					temp1 = temp1->next;
				}
			}
		}
	}

	return head;
}

void processFile(struct movie* head)
{
	srand(time(0));
	int randomNumber = rand() % 100000;
	char temp[30];
	sprintf(temp, "regerj.movies.%d", randomNumber);
	printf("Created directory with name: %s\n", temp);
	mkdir(temp, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP);

	struct movie* iter = head;
	int year = iter->year;

	// Create file for first year
	// Write first movie info to file
	FILE* fp;
	char tempFileName[40];
	sprintf(tempFileName, "%s/%d.txt", temp, iter->year);

	fp = fopen(tempFileName, "w+");
	
	int status;
	status = chmod(tempFileName, S_IRUSR | S_IWUSR | S_IRGRP);
	/*
	if (status == 0)
		printf("Changed %s permissions to read and write usr and read grp\n", tempFileName);
	else
		printf("Failed to change %s permissions\n", tempFileName);
	*/

	if (fp == NULL)
		printf("Failed to create file at %s", tempFileName);

	while (iter != NULL)
	{
		while (iter->next != NULL && iter->year == iter->next->year)
		{
			fputs(iter->title, fp);
			fputc('\n', fp);
			iter = iter->next;
		}
		fputs(iter->title, fp);
		fputc('\n', fp);
		if (iter->next != NULL && iter->next->year != iter->year)
		{
			fclose(fp);
			sprintf(tempFileName, "%s/%d.txt", temp, iter->next->year);
			fp = fopen(tempFileName, "w");
			status = chmod(tempFileName, S_IRUSR | S_IWUSR | S_IRGRP);
			/*
			if (status == 0)
				printf("Changed %s permissions to read and write usr and read grp\n", tempFileName);
			else
				printf("Failed to change %s permissions\n", tempFileName);
			*/
		}
		iter = iter->next;
	}
	if (fp != NULL)
		fclose(fp);

	return;
}

void clearMemory(struct movie* head)
{
	struct movie* iter = head->next;
	while (head->next != NULL)
	{
		free(head->title);
		free(head);
		head = iter;
		iter = head->next;
	}
	free(head->title);
	free(head);
}