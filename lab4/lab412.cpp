#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <cstdlib>

using namespace std;

int main() {
    
    int status = 0;
    pid_t ppid = getppid();
    const char* k = "./lab411";
    char *new_env[] = {"I`ve come from parent as enviroment", NULL};

    pid_t pid = fork();
    
    printf("Parent`s ID: %d,\t Child`s ID: %d\n", ppid, pid);

    if (pid == 0) {
        printf("ENTERED\n");
        execle(k, "I`ve come from parent!", NULL, new_env);
        
    }else{
        while (waitpid(pid, &status, WNOHANG) != -1) {
            printf("WAIT\n");
            sleep(1);
        }
        printf("Recieved from child: %d\n", WEXITSTATUS(status));
    }

    return 0;
}
