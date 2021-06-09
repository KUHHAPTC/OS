#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <cstdlib>

#define N 10
#define STACK_SIZE (1024 * 1024)

using namespace std;

static int childFunc(void *arg){
    pid_t ppid = getppid();
    for (int i = 0;  i < 5; i++){
        printf("Child`s ID: %d\n", ppid);
        sleep(1);
    }
    exit(N);
}

int main(int argc, char *argv[]) {

    int status;

    char *stack;
    char *stackTop;
    pid_t ppid = getppid();
    stack = (char*)malloc(STACK_SIZE);
    
    int child_pid = clone(childFunc, stackTop, SIGCHLD, NULL);
    printf("Parent`s ID: %d,\t Child`s ID: %d\n", ppid, child_pid);
    if (child_pid == -1){
        perror("Clone");
    }else{
        while (waitpid(child_pid, &status, WNOHANG) != -1) {
            printf("WAIT\n");
            sleep(1);
        }
        printf("Recieved from child: %d\n", WEXITSTATUS(status));
    }

    return 0;
}
