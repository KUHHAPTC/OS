#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[], char *env[]) {
    
    int N = 4;
    pid_t pid = getpid();
    pid_t ppid = getppid();
    printf("ENVIROMENT:  %s\n", env[0]);
    printf("Child start it`s work:\n");
    for (int i = 0;  i < 5; i++){
        printf("Parent`s ID: %d,\t Child`s ID: %d\n", ppid, pid);
        sleep(1);
    }

    exit(N);
};