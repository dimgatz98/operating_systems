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
	return 0;
}