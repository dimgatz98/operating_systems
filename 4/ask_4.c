#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>   
#include <getopt.h>
#include <time.h>
#include <stdlib.h>
#define MAX_LIMIT 50

#define YELLOW "\033[33m"
#define RED "\033[31;1m"
#define GREEN "\033[32m"
#define WHITE "\033[37m"
#define MAGENTA "\033[34m"
#define CYAN "\033[36m"
#define GRAY "\033[38;1m"
#define DEFAULT "\033[30;1m"

void remove_front_zeros(char *arr){
    int i = 0;
    while(arr[0] == '0'){    
        while(arr[i] != '\0'){
            arr[i] = arr[i + 1];
            i++;
        }
        i = 0;
    }
}

void remove_front_and_back_whitespaces(char *arr){
    int i = 0;
    while(arr[0] == ' '){    
        while(arr[i] != '\0'){
            arr[i] = arr[i + 1];
            i++;
        }
        i = 0;
    }

    i = strlen(arr) - 1; 

    while(arr[i] == ' ' || arr[i] == 0){    
        i--;
    }
    arr[i + 1] = 0;
}

int main(int argc, char **argv){
    char *address, datetime[26];
    time_t timer;
    struct tm *tm_info;
    int PORT = 18080, opt, debug = 0, option_index = 0, sock = 0, valread;
    
    struct option long_opt[] = {
        {"host", required_argument, NULL, 0},
        {"port", required_argument, NULL, 1},
        {"debug", no_argument, NULL, 2}
    }; 
    
    struct hostent *hostnm;
    struct sockaddr_in serv_addr;
    char msg[MAX_LIMIT], temp1[4], temp2[6], buffer[1024];
    
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Socket creation error \n");
        exit(-1);
    }
   
    hostnm = gethostbyname("lab4-server.dslab.os.grnetcloud.net");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);
      
    while((opt = getopt_long(argc, argv, "",
                        long_opt, &option_index)) != -1){ 
        switch(opt){
            case 0:
                address = optarg;
                if(inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0){
                    hostnm = gethostbyname(address);
                    serv_addr.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);
                }
                break;
            case 1:
                PORT = atoi(optarg);
                serv_addr.sin_port = htons(PORT);
                break;
            case 2:
                debug = 1;
                break;
            case '?':
                perror("Unknown option");
                printf(" %s\n", optarg);
                close(sock);
                exit(-1);
        }
    }

    if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("Connection Failed \n");
        exit(-1);
    }

    while(1){
        fgets(msg, MAX_LIMIT, stdin);
        msg[strlen(msg) - 1] = 0;
        remove_front_and_back_whitespaces(msg);
        if(strcmp(msg, "exit") == 0){
            close(sock);
            exit(0);
        }
        if(strcmp(msg, "help") == 0){
            printf(GREEN"\nAssignment by Theofania Karabela (el17081) \
and Dimitris Gkatziouras (03116145) \
for the course Operating Systems of 6th semester.\n \
    usage: [options]\n \
        options:\n \
            exit: terminates program\n \
            get: prints Temperature/Light level/Timestamp\n \
            N name surname reason: examines the reason for move permission\n\n"WHITE);
            continue;
        }
        if(send(sock , msg , strlen(msg) , 0) < 0){//when no flags equivalent to write(2);
            perror("Couldn't write to socket\n");
            close(sock);
            exit(-1);
        }
        
        valread = read(sock , buffer, 1024);//equivalent to recv(2);
        if(valread < 0){
            perror("Couldn't read from socket\n");
            close(sock);
            exit(-1);
        }
        
        buffer[valread - 1] = 0;
        if(debug == 1){
            printf("[DEBUG] sent '%s'\n", msg);
            printf("[DEBUG] read '%s'\n", buffer);
        }
        if(strcmp(buffer, "try again") == 0){
            printf(CYAN"%s\n"WHITE, buffer);
        }
        else if(strcmp(msg, "help") == 0){
            printf(CYAN"%s\n"WHITE, buffer);
        }
        else if(strcmp(msg, "get") == 0){
            temp1[0] = buffer[2];
            temp1[1] = buffer[3];
            temp1[2] = buffer[4];
            temp1[3] = 0;
            remove_front_zeros(temp1);

            temp2[0] = buffer[6];
            temp2[1] = buffer[7];
            temp2[2] = '.';
            temp2[3] = buffer[8];
            temp2[4] = buffer[9];
            temp2[5] = 0;
            remove_front_zeros(temp2);

            timer = time(NULL);
            tm_info = localtime(&timer);
            strftime(datetime, 26, "%Y-%m-%d %H:%M:%S", tm_info);
            printf(CYAN"Latest event:\n");
            switch(buffer[0]){
                case '0':
                    printf("boot"); 
                    break;
                case '1':
                    printf("setup"); 
                    break;
                case '2':
                    printf("interval"); 
                    break;
                case '3':
                    printf("button"); 
                    break;
                case '4':
                    printf("motion"); 
                    break;
            }
            printf("(%c)\nTemperature is: %s\n", buffer[0], temp2);
            printf("Light level is: %s\n", temp1);
            printf("Timestamp is: %s\n"WHITE, datetime);
            memset(temp1, 0, strlen(temp1));
            memset(temp2, 0, strlen(temp2));   
        }
        else if(('0' <= msg[0] && msg[0] <= '9') && msg[1] == ' '){
            printf(CYAN"Send verification code: %s\n"WHITE, buffer);
        }
        else{
            printf(CYAN"Response:'%s'\n"WHITE, buffer);
        }
        memset(buffer, 0, strlen(buffer));
    }

    return 0;
}
