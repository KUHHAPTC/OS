#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <cstdlib>
#include <sched.h>
#include <sys/param.h>
#include <sys/utsname.h>

using namespace std;

// Пространства имен не совпадают

static void print_nodename() {
    struct utsname utsname;
    uname(&utsname);
    printf("%s\n", utsname.nodename);
}

int main() {
    
    int status = 0;

    const char * name = "BOOM";
    sethostname(name, sizeof(name));

    printf("Original UTS namespace nodename: ");
    print_nodename();

    pid_t pid = fork();

    if (pid == 0) {
        if(unshare(CLONE_NEWUTS) == -1){
            perror("UNSHARE: ");
        }
        printf("CHILD UTS namespace nodename: ");
        print_nodename();

        const char * name = "GLaDOS";

        printf("Changing nodename inside new UTS namespace\n");
        sethostname(name, sizeof(name));

        printf("CHILD UTS namespace nodename: ");
        print_nodename();

    }else{
        while (waitpid(pid, &status, WNOHANG) != -1) {
        //    printf("WAIT\n");
            sleep(1);
        }
        printf("PARENT UTS namespace nodename: ");
        print_nodename();
    }

    return 0;
}
