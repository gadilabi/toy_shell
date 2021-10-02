#define _BSD_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "myArray.h"

char * stripChar(char * str, char del){
	int length = (int) strlen(str);
	char * stripped = malloc(1 + length*sizeof(char));
	int strippedLength = 0;

	for(int i=0; i<length; i++){
		if(str[i]==del)
			continue;
		stripped[strippedLength] = str[i];
		strippedLength++;
	}

	return stripped;
	
}

char * concat(char * head, char * tail ){
	int headLength = (int) strlen(head);
	int tailLength = (int) strlen(tail);
	int mergedLength = headLength + tailLength + 1;
	char * merged = malloc(sizeof(char)*mergedLength);

	for(int i=0; i<headLength; i++){
		merged[i] = head[i];
	}

	for(int i=0; i<tailLength; i++){
		merged[headLength + i] = tail[i];
	}

	merged[mergedLength - 1] = '\0';

	return merged;
}

char * firstWord(char * line){
	int lineLength = strlen(line);
	char * first = malloc(lineLength * sizeof(char));
	int counter = 0;

	for(int i=0; i<lineLength; i++){

		if(isalpha(line[i])){
			first[counter] = line[i];
			counter++;
			continue;
		}
		
		bool isSpace = line[i] == ' ';
		bool isNl = line[i] == '\n';
		bool isIndirection = (line[i] == '>' || line[i] == '<');
		bool isEnd = isSpace || isIndirection;

		// Spacing before command just continue
		if(line[i] == ' ' && counter == 0)
			continue;

		// End of word
		if(isEnd  && counter >0)
			break;
			
	}
	first[counter] = '\0';

	return first;

}

struct array * sepStr(char * str, char delim){
	// Create array to store seperated tokens
	struct array * sep = createArray(8, 20, NULL);

	// Iteration counter over str
	int strCounter = 0;

	// Counts number of elements in commandStr
	int commandCounter = 0;

	// Current character in str
	char c;

	// The current command
	char * commandStr = malloc(20);

	// Loop over string until null term
	while((c=str[strCounter])!='\0'){
		if(c==delim){
			// Terminate the string
			commandStr[strCounter] = '\0';

			if((int) strlen(commandStr)>0){
				// Add commandStr to the sep array
				sep->addElement(sep, &commandStr);

			}

			// Allocate new buffer
			commandStr = malloc(20);

			// restart commandStr counter
			commandCounter = 0;

			// Inc str counter
			strCounter++;

		}
		else{
			// Add current character into 
			commandStr[commandCounter] = c;

			commandCounter++;
			strCounter++;


		}

	}
	/* ADD THE LAST WORD */
	/* SEARCH HERE */
	if(commandCounter>0){
		// Terminate the string
		commandStr[commandCounter] = '\0';

		// Add commandStr to the sep array
		sep->addElement(sep, &commandStr);

	}

	return sep;
}



/*
int main(){
	char * str = "hello world!";
	char * strip = stripChar(str, '!');
	printf("%s\n", strip);
}
*/

