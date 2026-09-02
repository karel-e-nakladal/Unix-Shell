#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdbool.h>

#include "../lib/stringing.h"

#define INPUT_BUFFER_SIZE 1024
#define PATH_BUFFER_SIZE 255

/*
TODO: Add a list of built in commands and loop thru them (if there will be more than one)
TODO: Built in commands: [cd;help;debug]
TODO: CD command
        TODO: Command history
TODO: Command completion
TODO: exit when typed "exit" 
TODO: remove ctrl+c behavior & other?

TODO: Check if this is the correct way of making a shell
*/

struct inputTokens{
    int length;
    char command[INPUT_BUFFER_SIZE];
    char* tokens[INPUT_BUFFER_SIZE];
}typedef InputTokens;

struct configuration{
    char* splashImage;
    char* runOnStart;
}typedef Config;

struct historyNode{
    char* command;
    struct historyNode* prev;
}typedef HistoryNode;

struct commandHistory{
    HistoryNode* latest;
}typedef CommandHistory;

CommandHistory* _LatestCommand_;

Config* _Config_;

char* _CurrentDirectory_;


// MEMORY MANAGEMENT
    /*frees all memory taken by tokens*/
    void freeTokens(InputTokens* tokens){
        int i = 0;
        while (tokens->tokens[i] != NULL){
            free(tokens->tokens[i]);
            i++;
        }
        free(tokens);
    }

    // WHY DID I DO THIS??????????????
    /*
    void freeHistory(HistoryNode* removed){
        HistoryNode* curr = _LatestCommand_->latest;
        if(curr == removed){
            HistoryNode* tmp = curr->prev;
            free(curr);
            _LatestCommand_->latest = tmp;
            return;
        }
        while((curr->prev != removed) || (curr->prev != NULL)){
            curr = curr->prev;
        }

        if(curr->prev == NULL) return;

        HistoryNode* tmp = curr->prev->prev;
        free(curr->prev);
        curr->prev = tmp;
    }
    */

    /*adds the inputed command in to history for reuse*/
    void pushCommandToHistory(char* command){
        HistoryNode* latest = _LatestCommand_->latest;
        HistoryNode* new = malloc(sizeof(HistoryNode));
        
        //new->command = command;
    

        if(latest == NULL){
            latest = new;
        }else{
            new->prev = latest;
            _LatestCommand_->latest = new;
        }
    }
    

// INPUT
    void getInput(char* input){
        fgets(input, sizeof(char)*INPUT_BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = 0;
        pushCommandToHistory(input);
    }

    /*Tokenizes input in to (command), (command+args)*/
    int tokenizeInput(char* input, InputTokens* tokens){
        char* tokenizedInput[INPUT_BUFFER_SIZE];
        int size = strsplt(input, ' ', tokenizedInput);
        int i = 0;

        strcpy(tokens->command, tokenizedInput[0]);
        do{
            tokens->tokens[i] = malloc(sizeof(char) * INPUT_BUFFER_SIZE);
            memcpy(tokens->tokens[i], tokenizedInput[i], sizeof(char) * INPUT_BUFFER_SIZE);
            i++;
        }while(i != size);
        tokens->tokens[i] = NULL;
        tokens->length = i;
        return 0;
    }

// BUILT IN COMMANDS
    // DEBUG
        /*Prints current directory*/
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

        /*Prints debug info*/
        void printDebug(){
            printPath();
        }

    void commandCd(InputTokens* tokens){
        char* destinationTokens[PATH_BUFFER_SIZE];
        int destinationTokensSize = strsplt(tokens->tokens[1], '/', destinationTokens);

        char* currentPath = strdup(getenv("PATH"));
        char* currentPathTokens[PATH_BUFFER_SIZE];
        int currentPathSize = strsplt(currentPath, '/', currentPathTokens);

        char* newPath;
        // if first char is / then go from root
        
        // if first 2 chars are . then go up
        // if first char is neither go down
        if(*destinationTokens[0] == '/'){
            strjoin(destinationTokens, destinationTokensSize, newPath);
            chdir(newPath);
            return;
        }

        int i = 0;
        
        for (size_t i = 0; i < destinationTokensSize; i++)
        {
            if(*destinationTokens[0] == '.' && *destinationTokens[1] == '.'){
                if(currentPathSize > 0){
                    currentPathTokens[--currentPathSize] = NULL;
                }
            }else{
                strcpy(currentPathTokens[currentPathSize++], destinationTokens[i]);
            }
        }
        
        strjoin(destinationTokens, destinationTokensSize, newPath);
        
        free(currentPath);

        chdir(newPath);
    }

    int isBuiltIn(InputTokens* tokens){
        char* command = tokens->command;
        if(strcmp(command, "cd")){
            commandCd(tokens);
            return true;
        }
        return false;
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
                return true;
            }

            s = p + 1;
            idk->st_mode = 0;
        }while(p != NULL);
        free(idk);
        free(dup);
        return false;
    }

    int runProgram(InputTokens* tokens){
        return execvp(tokens->command, tokens->tokens);
    }

    int execCommand(InputTokens* tokens){
        pid_t command_pid = fork();

        if(command_pid == -1){
            perror("error");
            return 1;
        }

        if(command_pid == 0){
            int status_code = runProgram(tokens);
        }else{
            waitpid(command_pid, NULL, 0);
        }

        return 0;
    }


    void execInput(InputTokens* tokens){
        char path[100];
        if(strcmp(tokens->command, "debug") == 0){
            printDebug();
        }else if(isBuiltIn(tokens)){
            printf("The change directory (cd) command is not yet implemented\n");
        }else if(commandExists(tokens->command, path)){
            execCommand(tokens);
        }else{
            printf("Uknown command: %s", tokens->command);
        }

    }


// RUNTIME
    void commandLoop(){
        while(1){
            char input[INPUT_BUFFER_SIZE];

            newLine();
            getInput(input);
            InputTokens* tokens = malloc(sizeof(InputTokens));
            tokenizeInput(input, tokens);

            execInput(tokens);

            freeTokens(tokens);
        }
    }

    // INIT
    /*Loading configuration files*/
    void loadConfig(){
        _Config_ = malloc(sizeof(struct configuration));
        _Config_->splashImage = "|\\---/|\n| o_o |\n \\_^_/";
        //_Config_->runOnStart = "ls";
    }

    void printSplash(){
        if(_Config_->splashImage != NULL){
            printf("%s\n", _Config_->splashImage);
        }else{
            printf("SPLASH ART NO SET!\n");
        }
    }

    void runOnStart(){
        if(_Config_->runOnStart != NULL){
            InputTokens* startupCommand = malloc(sizeof(InputTokens));

            tokenizeInput(_Config_->runOnStart, startupCommand);
            execInput(startupCommand);
            freeTokens(startupCommand);
        }
    }

    void init(){
        loadConfig();
        printSplash();
        runOnStart();
    }

int main(){
    
    init();
    commandLoop();
    return EXIT_SUCCESS;
}
