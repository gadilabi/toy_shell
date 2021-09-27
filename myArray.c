#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "myArray.h"

/*
   struct array{
   void * arr;
   int type;
   int size;
   int length;
   void (* printElem)(void * elem);
   void (* print)(struct array * a);
   };
 */
static void printArray(struct array * a);
static void addElement(struct array * a, void * elem);
static void * get(struct array * a, int index);

struct array * createArray(int type, int size, void printElem(void * elem)){
	struct array * a = malloc(sizeof(struct array));
	a->length = 0;
	a->type = type;
	a->arr = malloc(size*type);
	a->size = size;
	a->print = printArray;
	a->printElem = printElem;
	a->addElement = addElement;
	a->get = get;

	return a;

}

static void addElement(struct array * a, void * elem){
	int bytesOffset = a->type * a->length;
	void * dest = (char *) a->arr + bytesOffset;
	if(elem == NULL){
		void ** n = malloc(8);
		*n = NULL;
		memcpy(dest, n, 8);
		a->length++;
		return;
	
	}
	memcpy(dest, elem, a->type);

	a->length++;


}

static void printArray(struct array * a){

	int bytesOffset;
	char * dest;

	if(a->length == 0)
		return;

	if(a->printElem == NULL){
		for(int i=0; i<(a->length); i++){
			dest = a->get(a, i);
			if(a->type == 1)
				printf("%c", * (char *) dest);

			if(a->type == 4)
				printf("%d", * (int *) dest);

			if(a->type == 8)
				printf("%s", * (char **) dest);

			printf("\n");
		}
		return;

	}

	for(int i=0; i<a->length; i++){
		dest = a->get(a, i);
		a->printElem(dest);

	}

}

static void prettyPrint(void * s){
	printf("hello\n");

}

static void * get(struct array * a, int index){
	int bytesOffset = a->type * index;
	void * dest = (char *) a->arr + bytesOffset;

	return dest;

}


/*
int main(){
	struct array * a = createArray(sizeof(char *), 10, NULL);
	int nums[] = {1,2,3,4,5, 6, 7, 8}; 
	char * str[] = {"hello", "world"};
	for(int i=0; i<2; i++)
		addElement(a, str[i]);
	//printf("type: %d ; size: %d ; length: %d", a->type, a->size, a->length);

	//a->print(a);
	printf("%s\n", str[0]);

	for(int i=0; i<2; i++)
		printf("%s\n",  (char *) a->get(a, i));
}
*/


