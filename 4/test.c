#include <stdio.h> 
#include <unistd.h> 
#include <getopt.h>

int main(int argc, char *argv[]){
    int opt;
    int option_index = 0;
    struct option long_opt[] = {
        {"host", required_argument, NULL, 0},
        {"port", required_argument, NULL, 1},
        {"debug", no_argument, NULL, 2}
    };

    while((opt = getopt_long(argc, argv, "",
                        long_opt, &option_index)) != -1){ 
        switch(opt){
            case 0:
                printf("host: %s\n", optarg);
                break;
            case 1:
                printf("port: %s\n", optarg);
                break;
            case 2:
                printf("debug: %s\n", optarg);
                break;
        }
    } 
}