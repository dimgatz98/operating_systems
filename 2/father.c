#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char **argv){
	pid_t pid;
	for(int i = 0 ; i < strlen(argv[1]) ; i++){
		char flag[] = {argv[1][i]};
		//printf("flag: %c\n", argv[1][i]);
		char *const parmList[] = {"child", flag, NULL, NULL};
		pid = fork();
		if(pid == 0)
			execve("child", parmList, NULL);
	}
}