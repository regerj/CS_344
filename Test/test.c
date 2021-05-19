#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool findSTOP(char * str)
{
    //char str[] = "We like to ALLSTOP talking sometimes";

    char str2[1000];
    strcpy(str2, str);
    printf("String is: %S\n", str2);
    const char search[5] = "STOP";
    const char search2[5] = "STOP\n";
    const char delim[2] = " ";

    char *token;

    bool end = false;

    token = strtok(str2, delim);

    while (token != NULL) 
    {
        printf(" %s\n", token);
        if(strcmp(token, search) == 0 || strcmp(token, search2) == 0)
        {
            end = true;
            break;
        }
        token = strtok(NULL, delim);
    }
    
    if(end)
    {
        printf("ENDING\n");
    }
    return end;
}

int main() {

    char str[] = " STOP\n";

    bool end = false;
    end = findSTOP(str);
    if(end)
        printf("Ended\n");

    return 0;
}