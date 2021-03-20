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

#define YELLOW "\033[33m"
#define RED "\033[31;1m"
#define GREEN "\033[32m"
#define WHITE "\033[37m"
#define N 2
#define BUFFER_SIZE 256

//function to convert integer to character array
char * toArray(int number){
    int n = log10(number) + 1;
    int i;
    char *numberArray = calloc(n, sizeof(char));
    for (i = n-1; i >= 0; --i, number /= 10)
    {
        numberArray[i] = (number % 10) + '0';
    }
    return numberArray;
}

int main(int argc, char **argv){
	pid_t pid[N];
	int child_no = 1;
	//read n and convert it to integer
	int n = atoi(argv[2]);
	
	//opening file descripto in order to read and write to the file
	int fd = open(argv[1], O_RDWR|O_APPEND|O_CREAT|O_EXCL, 0666);
	//if open failed
	if(fd == -1){
		perror("File already exists\n");
		return -1;	
	}

	//create N children
	for(int i = 0 ; i < N ; i++){
		pid[i] = fork();
		
		//if fork failed
		if(pid[i] < 0){
			perror("Child not created.");
			return -1;
		}

		//the child that just got created updates its child_no and prints
		if(pid[i] == 0){
			child_no = i + 1;
			//child_no == 1 print red
			if(child_no == 1)
				printf(RED"[Child%d] Started. PID=%d PPID=%d\n",child_no,getpid(),getppid());
			//else print green
			else
				printf(GREEN"[Child%d] Started. PID=%d PPID=%d\n",child_no,getpid(),getppid());	
			break;
		}
	}
	
	//child
	if(pid[child_no - 1] == 0){
		//create the message to write in file
		char buffer[BUFFER_SIZE] = "message from ";
		char *buffer1 = toArray(getpid());
		strcat(buffer, buffer1);
		strcat(buffer, "\n");
		
		//print to terminal and write in file
		for(int i = 0 ; i < n ; i++){
			//child_no == 1 print red
			if(child_no == 1)
				printf(RED"[Child%d] Heartbeat PID=%d Time=%ld x=%d\n",child_no,getpid(),time(NULL),i);
			//else print green
			else
				printf(GREEN"[Child%d] Heartbeat PID=%d Time=%ld x=%d\n",child_no,getpid(),time(NULL),i);
			write(fd, buffer, strlen(buffer));
			sleep(1);
		}
		//terminating
		if(child_no == 1)
			printf(RED"[Child%d] Terminating!\n", child_no);
		else
			printf(GREEN"[Child%d] Terminating!\n", child_no);
		//close file descriptor after write
		close(fd);
	}

	//parent
	else{
		char buffer[BUFFER_SIZE] = "message from ";
		char *buffer1 = toArray(getpid());
		strcat(buffer, buffer1);
		strcat(buffer, "\n");
		for(int i = 0 ; i < n ; i++){
			printf(WHITE"[Parent] Heartbeat PID=%d Time=%ld\n",getpid(), time(NULL));
			write(fd, buffer, strlen(buffer));
			sleep(1);
		}

		//wait for children
		pid_t pid_child_term;
		for(int i = 0 ; i < N ; i++){
			printf(WHITE"[Parent] waiting for child\n");
			pid_child_term = wait(NULL);
			printf(WHITE"[Parent] Child with PID=%d terminated\n", pid_child_term);
		}

		//moving cursor to the beginning of the file and print content
		char rd_buffer[BUFFER_SIZE*n*3];
		printf(YELLOW"[Parent] PID=%d Reading file\n", getpid());
		lseek(fd, 0 , SEEK_SET);
		read(fd, rd_buffer, BUFFER_SIZE*n*3);
		printf(YELLOW"%s", rd_buffer);

		//close the file descriptor
		close(fd);
	
	}
	
	return 0;

}