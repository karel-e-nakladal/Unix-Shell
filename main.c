#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pthread.h>

#define INPUT_BUFFER_SIZE 255
#define PATH_BUFFER_SIZE 255

/*
TODO: CD command
TODO: Command history
TODO: Command completion
TODO: exit when typed "exit" 
TODO: remove ctrl+c behavior

TODO: Check if this is the correct way of making a shell
*/

struct inputTokens{
    int length;
    char command[INPUT_BUFFER_SIZE];
    char* tokens[INPUT_BUFFER_SIZE];
}typedef InputTokens;


// INPUT
    void getInput(char* input){
        fgets(input, sizeof(char)*INPUT_BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = 0;
    }

    int tokenizeInput(char* input, InputTokens* tokens){
        char* idk[INPUT_BUFFER_SIZE];
        char* dup = strdup(input);
        char* s = dup;
        char* t = NULL;
        int i = 0;
        do{
            t = strchr(s, ' ');
            if(t != NULL){
                t[0] = 0;
            }
            tokens->tokens[i] = malloc(sizeof(char) * INPUT_BUFFER_SIZE);
            memcpy(tokens->tokens[i], s, sizeof(char) * INPUT_BUFFER_SIZE);
            if(i == 0){
                strcpy(tokens->command, s);
            }
            s = t + 1;
            i++;

        }while(t != NULL);
        tokens->tokens[i] = NULL;
        tokens->length = i;
        free(dup);
        return 0;
    }

// DEBUG
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


// OUTPUT
    void newLine(){
        char path[PATH_BUFFER_SIZE];
        getcwd(path, sizeof(char) * PATH_BUFFER_SIZE);
        if(path == NULL){
            printf("$>");
        }else{
            printf("%s>", path);

        }
    }

int commandExists(char* command, char* path){
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

        commandPath[0] = 0;
        strcat(commandPath, s);
        strcat(commandPath, "/");
        strcat(commandPath, command);
        char* restrict fuck = commandPath;
        stat(fuck, idk);

        if(idk->st_mode != 0){
            strcpy(path, commandPath);
            return 1;
        }

        s = p + 1;
        idk->st_mode = 0;
    }while(p != NULL);
    free(idk);
    free(dup);
    return 0;
}

int execCommand(InputTokens* tokens){
    pid_t command_pid = fork();

    if(command_pid == -1){
        perror("error");
        return 1;
    }

    if(command_pid == 0){
        int status_code = execvp(tokens->command, tokens->tokens);
    }else{
        waitpid(command_pid, NULL, 0);
    }

    return 0;
}


void execInput(InputTokens* tokens){
    char path[100];
    if(strcmp(tokens->command, "debug") == 0){
        printDebug();
    }else if(strcmp(tokens->command, "cd") == 0){
        printf("The change directory (cd) command is not yet implemented\n");
    }else if(commandExists(tokens->command, path)){
        execCommand(tokens);
    }else{
        printf("Uknown command: %s", tokens->command);
    }

}


// MEMORY MANAGEMENT
    void clearTokens(InputTokens* tokens){
        int i = 0;
        while (tokens->tokens[i] != NULL){
            free(tokens->tokens[i]);
            i++;
        }
    }

int main(){
    while(1){
        char input[INPUT_BUFFER_SIZE];

        newLine();
        getInput(input);
        InputTokens* tokens = malloc(sizeof(InputTokens));
        tokenizeInput(input, tokens);

        execInput(tokens);

        clearTokens(tokens);
        free(tokens);
    }
    return 0;
}
