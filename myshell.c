// Tuan Quoc Nguyen - tqn170130
// CS 3377.0W1
// Programming Assignment 3 - abc

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "version.h"

#define READ 	0
#define WRITE 	1

void shell();
char* readLine();
char** parseLine(char* line);
int analyze(char** args);
int run(char** args);
int exCommands(char** args);
int cdCmd(char** args);
int pwdCmd(char** args);
int helpCmd(char** args);
int exitCmd(char** args);

int main() {
    displayVersion();
  	shell();
    return 0;
}

// Shell execs
void shell() {
    char* lineInput;
    char** args;
    int status;
    while (1) {
        printf("MyShell $  ");
        lineInput = readLine();
        args = parseLine(lineInput);
        status = analyze(args);
        if (status == 0) {
            continue;
        }
        else {
            printf("Error: Shell.\n");
            continue;
        }
        // free inLine and args for next loop
        free(lineInput);
        free(args);
    }
    return;
}

// read line
char* readLine() {
    char *line = NULL;
    size_t buffer = 0;
    getline(&line, &buffer, stdin);
    return line;
}

// parse line
char** parseLine(char* line) {
    int bufferSize = 64;
    int count = 0;
    char** tokenArr = malloc(sizeof(char*) * bufferSize);
    char* token;

    if (!tokenArr) {
        perror("Error: Parse.\n");
        exit(EXIT_FAILURE);
    }
    token = strtok(line, " \t\r\n\a");
    while (token != NULL) {
        tokenArr[count] = token;
        count++;
        if (count == bufferSize) {
            bufferSize += 64;
            tokenArr = realloc(tokenArr, sizeof(char*) * bufferSize);
            if (!tokenArr) {
                perror("Error: Parse.\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, " \t\r\n\a");
    }
    tokenArr[count] = NULL;
    return tokenArr;
}

// analyze input
int analyze(char** args) {
    if (args[0] == NULL) { 
    	return 1; 
    }
    char* exCmd[] = {"cd", "exit", "pwd"};
    for (int i = 0; i < sizeof(exCmd)/sizeof(char*); i++) {
        if (strcmp(args[0], exCmd[i]) == 0) { 
        	return exCommands(args); 
        }
    }
    return run(args);
}

// run command
int run(char** args) {
    pid_t pid, wpid;
    int status;
    pid = fork();
    // Child
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            perror("Error: Child.");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    // Parent
    else if (pid > 0) {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    // Error
    else {
        perror("Error: Fork.");
        return 1;
    }
    return 0;
}

// handles cd pwd exit commands
int exCommands(char** args) { 
    char* exCmd[] = {"cd","exit","pwd"};
    int sta = 0;
    for (int i = 0; i < sizeof(exCmd) / sizeof(char*); i++) { 
        if (strcmp(args[0], exCmd[i]) == 0) { 
            sta = i + 1; 
            break; 
        } 
    }
    switch (sta) { 
    	case 1:
        	sta = cdCmd(args);
        	return sta;
    	case 2:
        	sta = exitCmd(args);
        	return sta;
    	case 3:
        	sta = pwdCmd(args);
        	return sta;
    	default:
        	return 1;
    }
}

// cd command
int cdCmd(char** args) {
    if (args[1] == NULL) {
        perror("Error: Argument.\n");
        return 1;
    }
    else {
        if (chdir(args[1]) != 0) {
            perror("Error: Shell.");
            return 1;
        }
    }
    return 0;
}

// pwd command
int pwdCmd(char** args) {
    char cwd[1024]; 
    getcwd(cwd, sizeof(cwd)); 
    printf("Dir: %s\n", cwd); 
    return 0;
}

// exit command
int exitCmd(char** args) {
    exit(EXIT_SUCCESS);
    return 0;
}

