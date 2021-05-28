#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>

pid_t *pid;
int n;

char *toArray(int number){
    int n = log10(number) + 1;
    int i;
    char *numberArray = calloc(n, sizeof(char));
    for (i = n-1; i >= 0; --i, number /= 10)
    {
        numberArray[i] = (number % 10) + '0';
    }
    return numberArray;
}

void terminate_children(int signum){
	for(int i = 0 ; i < n ; i++){
		if(kill(pid[i], SIGKILL) < 0)
			perror("Couldn't send signal!");
	}
	exit(0);
}

int main(int argc, char **argv){
	//....................................................//
	if(argc > 3){
		perror("Too many arguments");
		return -1;
	}
	if(argc < 3){
		perror("Too few arguments");
		return -1;
	}
	int k = atoi(argv[2]);
	n = atoi(argv[1]);
	pid = (pid_t*) malloc(n);
	int pd[n][2], nbytes;
	char readbuffer[128], *writebuffer;
	//.....................................................//
	for(int i = 0; i < n; i++){
		pipe(pd[i]);
	}

	for(int i = 0; i < n; i++){
		pid[i]=fork();

		if(pid[i] < 0){
			perror("Child not created, killing children and terminating");
			terminate_children(1);
		}

		else if(pid[i] == 0){
			int res, temp = i + 1;

			if(i == n - 1){
				for(int j = 0; j < n; j++){
					if(j == i){
						close(pd[j][1]);
					}

					else if(j == 0){
						close(pd[j][0]);
					}
					
					else{
						close(pd[j][0]);
						close(pd[j][1]);
					}
				}
			}
			else{
				for(int j = 0; j < n; j++){
					if(j == i){
						close(pd[j][1]);
					}

					else if(j == i + 1){
						close(pd[j][0]);
					}
					
					else{
						close(pd[j][0]);
						close(pd[j][1]);
					}
				}
			}

			while(1){
				nbytes = read(pd[i][0], readbuffer, sizeof(readbuffer));
				
				res = atoi(readbuffer);
				
				res = res*temp;
				writebuffer = toArray(res);
				if(i == n - 1)
					write(pd[0][1], writebuffer, (strlen(writebuffer) + 1) );
				else
					write(pd[i + 1][1], writebuffer, (strlen(writebuffer) + 1) );
				
				if(temp == k){
					printf("%d\n", res);
					kill(getppid(), SIGINT);
				}
				
				temp += n;
			}
		}
	}

	struct sigaction terminate_children_action;
	terminate_children_action.sa_handler = terminate_children;
	sigaction(SIGINT, &terminate_children_action, NULL);
	
	write(pd[0][1], "1", (strlen("1") + 1) );
	
	for(int i = 0 ; i < n ; i++){
		close(pd[i][0]);
		close(pd[i][1]);
	}
	
	for(int i = 0 ; i < n ; i++){
		wait(NULL);
	}

	return 0;
}