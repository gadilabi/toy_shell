#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "myArray.h"
#include "myString.h"

char * setEnvPath(char * command){
	
	int commandLength = (int) strlen(command);
	char * path = "";
	int pathCounter = 0;
	char * current;
	Array * pathArray = sepStr(command, ' ');
	for(int i=0; i<pathArray->length; i++){
		current = * (char **) pathArray->get(pathArray, i);
		path = concat(path, current);
		if(i<pathArray->length-1)
			path = concat(path, ":");
		
	}

	setenv("PATH", path, 1);
	
	return path;
	
}

int main(int argc, char * argv[], char ** envp){
	char * spacePath = " /bin /usr/sbin /home/vagrant/shell ";
	setEnvPath(spacePath);
	char * colonPath = getenv("PATH");
	printf("%s\n", colonPath);


	/*
	setenv("PATH", "/wird", 1);
	int i;
	for(i=0; envp[i]!=NULL; i++)
		printf("%s\n", envp[i]);
	*/
}
