#include <stdio.h>
#include <stdlib.h>

int main(void){
	char * c = "hello";
	char * str = malloc(10);
	int i=0;
	for(i=0; i<5; i++){
		str[i] = c[i];
	}
	str[6] = '0';

	printf("%s\n", str);
	free(str);
}
