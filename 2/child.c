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

time_t start;
int id;
char gate_state;

void print_and_alarm(int signum){
	if(gate_state == 't')
		printf(GREEN"[ID=%d/PID=%d/TIME=%lds] The gates are open!\n", id, getpid(), time(NULL) - start);
	
	else
    	printf(RED"[ID=%d/PID=%d/TIME=%lds] The gates are closed!\n", id, getpid(), time(NULL) - start);
    alarm(15);
}

void print_state(int signum){
	if(gate_state == 't')
		printf(GREEN"[ID=%d/PID=%d/TIME=%lds] The gates are open!\n", id, getpid(), time(NULL) - start);
	
	else
    	printf(RED"[ID=%d/PID=%d/TIME=%lds] The gates are closed!\n", id, getpid(), time(NULL) - start);	
}

void flip_state(int signum){
	if(gate_state == 'f'){
		gate_state = 't';
		printf(GREEN"[ID=%d/PID=%d/TIME=%lds] The gates are open!\n", id, getpid(), time(NULL) - start);
	}

	else{
		gate_state = 'f';
    	printf(RED"[ID=%d/PID=%d/TIME=%lds] The gates are closed!\n", id, getpid(), time(NULL) - start);
    }
}

void terminate(int signum){
	exit(0);
}

int main(int argc, char **argv){
	if(argc != 2){
		perror(DEFAULT"Too many arguments in child process!");
		return -1;
	}
	struct sigaction print_and_alarm_action, print_state_action, flip_state_action, terminating_action;
	
	print_and_alarm_action.sa_handler = print_and_alarm;
	print_and_alarm_action.sa_flags = SA_RESTART;
	
	print_state_action.sa_handler = print_state;
	print_state_action.sa_flags = SA_RESTART;
	
	flip_state_action.sa_handler = flip_state;
	flip_state_action.sa_flags = SA_RESTART;
	
	terminating_action.sa_handler = terminate;
	terminating_action.sa_flags = SA_RESTART;
	
	id = argv[1][1];
	gate_state = argv[1][0];

	start = time(NULL);
    
    if(argv[1][0] == 't')
		printf(GREEN"[ID=%d/PID=%d/TIME=%ds] The gates are open!\n", id, getpid(), 0);
	
    else
    	printf(RED"[ID=%d/PID=%d/TIME=%ds] The gates are closed!\n", id, getpid(), 0);

    sigaction(SIGALRM, &print_and_alarm_action, NULL);
    sigaction(SIGUSR1, &print_state_action, NULL);
    sigaction(SIGUSR2, &flip_state_action, NULL);
	sigaction(SIGTERM, &terminating_action, NULL);
	
	alarm(15);

    while(1){
    	pause();
    }

    return 0;
}