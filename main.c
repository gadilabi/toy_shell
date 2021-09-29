#define _BSD_SOURCE
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include "myString.h"
#include "myArray.h"

Array * PATH;

enum CommandType{
	BUILT_IN, EXTERNAL
};

enum Redirection{
	INPUT_REDIRECTION, OUTPUT_REDIRECTION_W, OUTPUT_REDIRECTION_A, NO_REDIRECTION
};

int exec_built_in(char * command, char * first);
void exec_external(char * command);
int setPath(char * command);
int changeDir(char * command);
int exec_output_indirection(char * command, enum Redirection type);
int exec_input_indirection(char * command);
int exec_straight(char * command);
enum Redirection checkRedirection(char * command);
char * createPrompt(void);
enum CommandType checkType(char * command);

int main(int argc, char * argv[]){
	char * commandPrompt = createPrompt();
	char ** buffer = malloc(sizeof(char *));
	int n=9;
	*buffer = malloc(n);
	printf("\n%s", commandPrompt);
	
	while(getline(buffer,(size_t *) &n, stdin)){
		
		/*
		*****************************
		*   Test checkRedirection   *  
		*****************************

		enum Redirection type = checkRedirection(*buffer);
		switch(type){
			case INPUT_REDIRECTION:
				printf("input");
				break;
			case OUTPUT_REDIRECTION_A:
				printf("output_a");
				break;
			case OUTPUT_REDIRECTION_W:
				printf("output_w");
				break;
			case NO_REDIRECTION:
				printf("no");
		}
		*/
		
		   char * first = firstWord(*buffer);
		   enum CommandType type = checkType(first);
		   int commandLength = (int) strlen(*buffer);
		   (*buffer)[commandLength-1] = '\0';

		   if(type == BUILT_IN)
		   exec_built_in(*buffer, first);

		   if(type == EXTERNAL)
		   exec_external(*buffer);

		   commandPrompt = createPrompt();
		   printf("\n%s", commandPrompt);

		   free(commandPrompt);
		   free(first);
		   commandPrompt = NULL;
		   first = NULL;

		 
	}
}

int exec_built_in(char * command, char * first){
	if(strcmp(first, "cd") == 0){
		int status = changeDir(command);
		if(status==0)
			return 0;
		else 
			return 1;

	}

	if(strcmp(first, "path") == 0){
		int status = setPath(command);
		if(status!=0)
			return 2;
		else
			return 0;
	}

	if(strcmp(first, "exit") == 0){
		exit(0);
	}

	return -1;
}

int changeDir(char * command){
	Array * sepCommad = sepStr(command, ' ');
	if(sepCommad->length != 2 )
		return -1;
	char * newDir = *(char **) sepCommad->get(sepCommad, 1);
	return chdir(newDir);
}

int setPath(char * command){
	Array * newPath = createArray(sizeof(char *), 15, NULL);
	Array * sepCommad = sepStr(command, ' ');
	if(sepCommad->length == 0){
		PATH = newPath;
		return 0;
	}

	for(int i=1; i<sepCommad->length; i++){
		char * current = sepCommad->get(sepCommad, i);
		newPath->addElement(newPath, current);
	}

	PATH = newPath;
	return 0;
}

int exec_input_indirection(char * command){

}

int exec_output_indirection(char * command, enum Redirection type){

}

int exec_straight(char * completeCommand){

	// Split the argument into array of commands
	struct array * commandList = sepStr(completeCommand, '|');

	// Pipe file descriptors
	int pfd1[2];
	int pfd2[2];

	// Create a pipe
	pipe(pfd1);
	pipe(pfd2);

	// Set the parent reader and writer end of each pipe
	int parentWrite = pfd1[1];
	int parentRead = pfd2[0];


	// Set the child reader and writer end of each pipe
	int childWrite = pfd2[1];
	int childRead = pfd1[0];

	// Buffer size
	int bufferSize = 5000;

	// stdin buffr
	char * inBuffer = malloc(bufferSize);

	int outputSize = 0;

	for(int i=0; i < commandList->length; i++){
		// Create a child process 
		int cpid = fork();

		// If fork failed
		if(cpid == -1)
			return -1;

		if(cpid == 0){
			// Close parent side of the pipe
			close(parentWrite);
			close(parentRead);

			// Error code for exec call
			int exStatus;

			// Make copies of the stdout and stdin
			// since they are going to be redirected
			int out = dup(STDOUT_FILENO);
			int in = dup(STDIN_FILENO);

			// Redirect the stdout to the write side of the pipe
			dup2(childWrite, STDOUT_FILENO);
			close(childWrite);

			// Redirect the stdin to the read side of the pipe
			dup2(childRead, STDIN_FILENO);
			close(childRead);

			// Split the command by spaces
			struct array * args = sepStr(*(char **)commandList->get(commandList, i), ' ');

			// First 
			// NULL terminate the argument list for process
			// as required by exec
			args->addElement(args, NULL);
			exStatus = execvp(((char **) args->arr)[0],((char **)args->arr));

			// Handle errors in exec
			if(exStatus==-1){
				dup2(out, STDOUT_FILENO);
				printf("fail\n");
				printf("%s\n", strerror(errno));
				return -1;

			}

	}else{
		// Close the sides that belong to the child
		close(childWrite);
		close(childRead);

		// Close parent write as there is no
		// data to pass to child
		close(parentWrite);
		
		// Wait for child to finish
		wait(NULL);
		
		outputSize = 0;

		// Read the output of the child
		while(read(parentRead, inBuffer+outputSize, 1))
			outputSize++;

		// Close remaining pipes of parent
		close(parentRead);

		// Create new pipes for parent and new child
		pipe(pfd1);
		pipe(pfd2);

		// Set the parent reader and writer end of each pipe
		parentWrite = pfd1[1];
		parentRead = pfd2[0];

		// Set the child reader and writer end of each pipe
		childWrite = pfd2[1];
		childRead = pfd1[0];

		// Write the output of the former process
		// into the stdin of the new child process
		write(parentWrite, inBuffer, outputSize);
	}


}

// Read from the pipe into inBuffer
write(STDOUT_FILENO, inBuffer, outputSize);


}

void exec_external(char * command){
	enum Redirection redirectionType = checkRedirection(command);
	switch(redirectionType){
		case INPUT_REDIRECTION:
			exec_input_indirection(command);
			break;
		case OUTPUT_REDIRECTION_A:
			exec_output_indirection(command, OUTPUT_REDIRECTION_A);
			break;
		case OUTPUT_REDIRECTION_W:
			exec_output_indirection(command, OUTPUT_REDIRECTION_W);
			break;
		case NO_REDIRECTION:
			exec_straight(command);
	}
}

enum Redirection checkRedirection(char * command){
	int length = (int) strlen(command);
	for(int i=0; i<length; i++){
		if(command[i]=='<')
			return INPUT_REDIRECTION;

		if(command[i]=='>'){
			if(command[i+1]=='>')
				return OUTPUT_REDIRECTION_A;
			else
				return OUTPUT_REDIRECTION_W;
		}
	}

	return NO_REDIRECTION;
}

enum CommandType checkType(char * command){
	if(strcmp(command, "cd") == 0)
		return BUILT_IN;
	if(strcmp(command, "path") == 0)
		return BUILT_IN;
	if(strcmp(command, "exit") == 0)
		return BUILT_IN;

	return EXTERNAL;
}

char * createPrompt(void){

	char * currentDir = getcwd(NULL, 0);
	char * commandPrompt =concat(concat("wish|", currentDir), "> ") ; 
	free(currentDir);
	currentDir = NULL;

	return commandPrompt;
}
