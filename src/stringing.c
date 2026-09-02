#include "../lib/stringing.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int strsplt(char* string, char token, char** output){
    char* dup = strdup(string);
    char* s = dup;
    char* t = NULL;
    int i = 0;
    do{
        t = strchr(s, token);
        if(t != NULL){
            t[0] = 0;
        }
        strcpy(output[i], s);

        s = t + 1;
        i++;

    }while(t != NULL);

    free(dup);

    return i;
}

void strjoin(char** input, int inputSize, char* output){
    for (size_t i = 0; i < inputSize; i++)
    {
        strcpy(output, input[i]);
    }
    
}