#define _BSD_SOURCE
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "myString.h"
#include "myArray.h"

Array * PATH;

enum CommandType{
	BUILT_IN, EXTERNAL
};

int exec_built_in(char * command, char * first);
void exec_external(char * command);
int setPath(char * command);
int changeDir(char * command);
char * createPrompt(void);
enum CommandType checkType(char * command);

int main(int argc, char * argv[]){
	
	char * commandPrompt = createPrompt();
	char ** buffer = malloc(sizeof(char *));
	int n=9;
	*buffer = malloc(n);
	printf("\n%s", commandPrompt);
	
	while(getline(buffer,(size_t *) &n, stdin)){
		
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

void exec_external(char * command){
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
