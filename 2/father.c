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
#define MAGENTA "\033[34m"
#define CYAN "\033[36m"
#define GRAY "\033[38;1m"
#define DEFAULT "\033[30;1m"

pid_t *pid, killed_child;
int length;

void terminate_children_before_parent(){
	for(int i = 0 ; i < length ; i++){
		printf(CYAN"[PARENT/PID=%d] Waiting for %d children to exit\n", getpid(), length - i);
		if(kill(pid[i], SIGTERM) < 0){
			perror(DEFAULT"Counldn't send signal\n");
			exit(-1);
		}
		printf(YELLOW"[PARENT/PID=%d] Child with PID=%d terminated successfully with exit status code 0!\n", getpid(), pid[i]);
	}
	printf(YELLOW"[PARENT/PID=%d] All children exited, terminating as well\n", getpid());
}

void terminate_children(int signum){
	for(int i = 0 ; i < length ; i++){
		printf(CYAN"[PARENT/PID=%d] Waiting for %d children to exit\n", getpid(), length - i);
		if(kill(pid[i], SIGTERM) < 0){
			perror(DEFAULT"Counldn't send signal\n");
			exit(-1);
		}
		printf(YELLOW"[PARENT/PID=%d] Child with PID=%d terminated successfully with exit status code 0!\n", getpid(), pid[i]);
	}
	printf(YELLOW"[PARENT/PID=%d] All children exited, terminating as well\n", getpid());
	exit(0);
}

void make_children_print(int signum){
	for(int i = 0 ; i < length ; i++){
		if(kill(pid[i], SIGUSR1) < 0){
			perror(DEFAULT"Counldn't send signal\n");
			terminate_children_before_parent();
			exit(-1);
		}
	}
}

int main(int argc, char **argv){
	if(argc != 2){
		perror(DEFAULT"Too many arguments in father process!\n");
		terminate_children_before_parent();
		return -1;
	}
	int code;
	struct sigaction terminate_children_action, make_children_print_action, remake_child_action;
	
	terminate_children_action.sa_handler = terminate_children;
	terminate_children_action.sa_flags = SA_RESTART;
	
	make_children_print_action.sa_handler = make_children_print;
	make_children_print_action.sa_flags = SA_RESTART;
	
	length = strlen(argv[1]);
	pid = (pid_t *) malloc(strlen(argv[1]));
	
	for(int i = 0 ; i < strlen(argv[1]) ; i++){
		char flag[] = {argv[1][i], i};
		char *const parmList[] = {"child", flag, NULL, NULL};
		pid[i] = fork();
		if(pid[i] < 0){
			perror(DEFAULT"child not created\n");
			terminate_children_before_parent();
			return -1;
		}
		else if(pid[i] == 0){
			if(execve("child", parmList, NULL) < 0){
				perror(DEFAULT"Counldn't send signal\n");
				terminate_children_before_parent();
				exit(-1);
			}
		}
		else{
			printf(MAGENTA"[PARENT/PID=%d] Created child %d (PID=%d) and initial state '%c'\n", getpid(), i, pid[i], flag[0]);
		}
	}

	sigaction(SIGTERM, &terminate_children_action, NULL);
	sigaction(SIGUSR1, &make_children_print_action, NULL);
	
	while(1){
		killed_child = waitpid(-1, &code, WUNTRACED);

		if(WIFEXITED(code)){
			int child_no;
			for(int i = 0 ; i < length ; i++){
				if(pid[i] == killed_child){
					child_no = i;
					break;
				}
			}
			printf("[PARENT/PID=%d] Child %d with PID=%d exited\n", getpid(), child_no, pid[child_no]);

			char flag[] = {argv[1][child_no], child_no};
			char *const parmList[] = {"child", flag, NULL, NULL};
			pid[child_no] = fork();
			if(pid[child_no] < 0){
				perror(DEFAULT"child not created\n");
				terminate_children_before_parent();
				exit(-1);
			}
			else if(pid[child_no] == 0){
				if(execve("child", parmList, NULL) < 0){
					perror(DEFAULT"Counldn't send signal\n");
					terminate_children_before_parent();
					exit(-1);
				}
			}
			else{
				printf(MAGENTA"[PARENT/PID=%d] Created child %d (PID=%d) and initial state '%c'\n", getpid(), child_no, pid[child_no], flag[0]);
			}
		}

		else if(WIFSTOPPED(code))
			kill(killed_child, SIGCONT);
	}
}