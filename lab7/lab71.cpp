#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <cstring>
#include <signal.h>
#include <stdio.h>

#define BUF_SIZE 256

using namespace std;

int fd;
pthread_t id1, id2;

typedef struct {
    int flag;
} args_s;

args_s arg2;

void sig_handler(int signo)
{
    printf("SIGPIPE received\n");
}


void * proc(void *arg){
    
    args_s *args = (args_s*) arg;
    char buf[BUF_SIZE];

    struct addrinfo *result;
    int err, mess;
    ssize_t nw;

    err = getaddrinfo("localhost", NULL, NULL, &result);
    if(err != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        exit(EXIT_FAILURE);
    }

    printf("Поток записи начал работу\n");

    freeaddrinfo(result);

    while (args->flag == 0) {

        mess = sprintf(buf, "%d", result->ai_protocol);
        nw = write(fd, buf, mess);
        if(nw == -1) {
            perror("write");
            sleep(1);
        }
       
       sleep(1);
    }

    pthread_exit((void*)2);
}

void * open_proc(void * arg){

    args_s *args = (args_s*) arg;
    int err;

    while(args->flag == 0) {

        fd = open("/tmp/my_named_pipe", O_WRONLY|O_NONBLOCK);
        if(fd == -1){
            perror("open");
            sleep(1);
        }
        else {
            err = pthread_create(&id2, NULL, proc, &arg2);
            if(err != 0)
            {
                perror("pthread_create");
            }
            args->flag = 1;
            pthread_exit((void*)1);
        }
    }

}

int main(){

    signal(SIGPIPE, sig_handler);
    args_s arg1;
    int exit, err = 0;

    err = mkfifo("/tmp/my_named_pipe", 0644);
    if(err != 0)
    {
        perror("mkfifo");
    }

    arg1.flag = 0;

    printf("Программа начала работу\n"); 

    err = pthread_create(&id1, NULL, open_proc, &arg1);
    if(err != 0)
    {
        perror("pthread_create");
        return 2;    
    }

    printf("Программа ждет нажатия клавиши\n"); 

    getchar();

    printf("Клавиша нажата\n"); 
    
    arg1.flag = 1;
    arg2.flag = 1;

    pthread_join(id1, (void**)&exit);

    printf("Поток открытия записи закончил работу\n");

    pthread_join(id2, (void**)&exit);

    printf("Поток записи закончил работу\n");

    close(fd);
    
    err = unlink("/tmp/my_named_pipe");
    if(err != 0)
        perror("unlink");

    printf("Программа закончила работу\n"); 

    return 0;
}