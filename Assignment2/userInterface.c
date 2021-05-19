#include "userInterface.h"
#pragma warning(disable : 4996)

int choiceLoop()
{
	while (1)
	{
		if (choiceOne())
		{
			choiceTwo();
		}
		else
			return 0;
	}
}
bool choiceOne()
{
	int choice;
	do
	{
		printf("What would you like to do?\n\t1. Slect file to process\n\t2. Exit the program\nEnter a choice 1 or 2: ");
		scanf("%d", &choice);
		if (choice < 1 || choice > 2)
			printf("Invalid choice! Please try again\n");

	} while (choice < 1 || choice > 2);

	if (choice == 1)
		return true;
	else
		return false;
}

void choiceTwo()
{
	int choice;
	char* name = NULL;
	do
	{
		printf("\nWhich file do you want to process?\n");
		printf("\t1. to pick the largest file\n");
		printf("\t2. to pick the smallest file\n");
		printf("\t3. to specify the name of the file\n");
		printf("Enter a choice from 1 to 3: ");
		scanf("%d", &choice);

		if (choice < 1 || choice > 3)
			printf("Invalid choice! Please try again\n");

	} while (choice < 1 || choice > 3);

	printf("\n");

	switch (choice)
	{
	case 1:
	{
		DIR* dir;
		struct dirent* ent = NULL;
		long int size = 0;
		long int sizeTemp;
		char * ext;
		char * dot;
		char tempName[40];

		if ((dir = opendir("./")) != NULL)												// Open curr directory
		{
			while ((ent = readdir(dir)) != NULL)										// While loop to iterate the ents in dir
			{
				dot = strrchr(ent->d_name, '.');
				ext = dot + 1;

				if (strncmp(ent->d_name, "movies_by_year", 13) != 0 && !strncmp(ent->d_name, "movies_", 7) && strcmp(ext, "csv") == 0)	// If the ent is a regular file
				// if(!strcmp(ext, "csv"))
				{
					FILE* fp = fopen(ent->d_name, "r");									// Open it
					fseek(fp, 0, SEEK_END);												// Iterate to EoF
					sizeTemp = ftell(fp);												// Pull size of file
					if (sizeTemp > size)												// If its bigger
					{
						size = sizeTemp;
						strcpy(tempName, ent->d_name);										// Save ent name

					}
					if(fp)
						fclose(fp);															// Close
				}
			}
		}
		else
		{
			printf("No working directory found!\n");
		}
		
		name = (char*)malloc(strlen(tempName) * sizeof(char) + 1);						// Dynamically allocated name string with perfect length
		strcpy(name, tempName);
		// name = tempName;															// Save name of largest file
		closedir(dir);
		break;
	}
	case 2:
	{
		DIR* dir;
		struct dirent* ent = NULL;
		long int size = 2147483647;
		long int sizeTemp;
		char* ext;
		char* dot;
		char tempName[40];

		if ((dir = opendir("./")) != NULL)												// Open curr directory
		{
			while ((ent = readdir(dir)) != NULL)										// While loop to iterate the ents in dir
			{
				dot = strrchr(ent->d_name, '.');
				ext = dot + 1;

				if (strncmp(ent->d_name, "movies_by_year", 13) != 0 && !strncmp(ent->d_name, "movies_", 7) && strcmp(ext, "csv") == 0)			// If the ent is a regular file
				{
					FILE* fp = fopen(ent->d_name, "r");									// Open it
					fseek(fp, 0, SEEK_END);												// Iterate to EoF
					sizeTemp = ftell(fp);												// Pull size of file
					if (sizeTemp < size)												// If its bigger
					{
						size = sizeTemp;
						strcpy(tempName, ent->d_name);										// Save ent name

					}
					if(fp)
						fclose(fp);															// Close
				}
			}
		}
		else
		{
			printf("No working directory found!\n");
		}

		name = (char*)malloc(strlen(tempName) * sizeof(char) + 1);					// Dynamically allocated name string with perfect length
		// name = tempName;															// Save name of largest file
		strcpy(name, tempName);
		closedir(dir);
		break;
	}
	case 3:
	{

		char temp[50];
		
		printf("Enter the complete file name: ");
		scanf("%s", temp);
		FILE* fp;
		fp = fopen(temp, "r");
		if (fp == NULL)
		{
			printf("The file %s was not found. Try again\n", temp);
			choiceTwo();
			return;
		}

		name = (char*)malloc(strlen(temp) * sizeof(char) + 1);
		strcpy(name, temp);
		// name = temp;
		if(fp)
			fclose(fp);
		
		break;
	}
	}

	struct movie* head = NULL;
	head = memHandler(name);
	printf("Now processing the chosen file named %s\n", name);
	processFile(head);
	clearMemory(head);
	free(name);
	return;
}
