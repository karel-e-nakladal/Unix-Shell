#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define INPUT_BUFFER_SIZE 255
#define PATH_BUFFER_SIZE 255

void getInput(char* input){
    fgets(input, sizeof(char)*INPUT_BUFFER_SIZE, stdin);
    input[strcspn(input, "\n")] = 0;
}

void printPath(){

    printf("!___$PATH ENV Data___!\n");
    
    char* dup = strdup(getenv("PATH"));
    char* s = dup;
    char* p = NULL;
    printf(" %s\n", s);

    do{
        p = strchr(s, ':');
        if (p != NULL){
            p[0] = 0;
        }
        printf(" %s\n", s);
        s = p + 1;
    }while(p != NULL);
    free(dup);
    printf("!____________________!\n");
}

void printDebug(){
    printPath();
}

int programExists(char* command){

    char* dup = strdup(getenv("PATH"));
    char* s = dup;
    char* p = NULL;


    struct stat* restrict idk = malloc(sizeof(stat));
    
    char commandPath[100];
    
    do{
        p = strchr(s, ':');
        if (p != NULL){
            p[0] = 0;
        }
;
        strcat(commandPath, s);
        strcat(commandPath, "/");
        strcat(commandPath, command);
        char tmp[30];
        strcpy(tmp, commandPath);
        char* restrict fuck = commandPath;
        stat(fuck, idk);
        
        printf("PATH:%s ; MODE:%i\n", tmp, idk->st_mode);
        printf("=====================================\n");
        s = p + 1;
        
        idk->st_mode = 0;
        commandPath[0] = 0;
    }while(p != NULL);
    free(idk);
    free(dup);
    return 0;
}

void execInput(char* input){
    if(strcmp(input, "debug") == 0){
        printDebug();
    }else if(programExists(input)){
        printf("Program exists");
    }else{
        printf("Uknown command: %s", input);
    }

}

void newLine(){
    char path[PATH_BUFFER_SIZE];
    getcwd(path, sizeof(char) * PATH_BUFFER_SIZE);
    if(path == NULL){
        printf("\n$>");
    }else{
        printf("\n%s>", path);

    }
}


int main(){
    while(1){
        char input[INPUT_BUFFER_SIZE];
        newLine();
        getInput(input);
        execInput(input);
    }
    return 0;
}
