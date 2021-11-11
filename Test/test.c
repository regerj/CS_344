#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

int main(void) {
    int counter = 0;
    int i = 5;
    for(i = 1; i < 10; i++){
        counter++;
        if(i == 5){
            continue;
        }
    }
    printf("%d\n", counter);
}