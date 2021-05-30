#include <stdio.h>
#include <string.h>
#define MAX_LIMIT 20

int main(){
	char msg[MAX_LIMIT];
	fgets(msg, MAX_LIMIT, stdin);
	if(strcmp(msg, "123\n") == 0)
		printf("%s", msg);
	else
		printf("Nope\n");
	
	if(msg[4] == '\0')
		printf("hello1\n");

	if(msg[4] == 0)
		printf("hello2\n");

	return 0;
}