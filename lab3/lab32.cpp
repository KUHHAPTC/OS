#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <fcntl.h>

#define BUF_SIZE 256


using namespace std;

typedef struct
{
    int flag;
    char symbol;
}gargs;

int pipefd[2];

void * proc1(void *arg){
    gargs * args = (gargs*) arg; 
    char buf[BUF_SIZE];
    struct addrinfo *result;
    int s, mess;
    ssize_t nr, nw;

    printf("Поток %c начал работу\n", args->symbol);

    s = getaddrinfo("localhost", NULL, NULL, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);
    
    mess = sprintf(buf, "%d", result->ai_protocol);

    while (args->flag == 0) {

        mess = sprintf(buf, "%d", result->ai_protocol);
        nw = write(pipefd[1], buf, mess);
        if (nw == -1) {
            perror("write");
            sleep(1);
        }

	    sleep(1);
    }
    pthread_exit((void*)1);
}

void * proc2(void *arg){
    gargs * args = (gargs*) arg;
    char buf[256];
    int nr;

    printf("Поток %c начал работу\n", args->symbol);

    while (args->flag == 0) {

        fflush(stdout);
        buf[0] = 0;
	    nr = read(pipefd[0], &buf, BUF_SIZE);
        if(nr == -1) {
            perror("read");
            sleep(1);
        }

        else if (nr == 0) {
        printf("pipe closed\n");
        sleep(1);
        }

        else{
        cout << "(" << buf << ")";
        }

    } 
    pthread_exit((void*)2);
}


int main(){

    gargs arg1, arg2;
    int exit1, exit2;
    pthread_t id1, id2;
    int err = 0;

    arg1.flag = 0;
    arg1.symbol = '1';
    arg2.flag = 0;
    arg2.symbol = '2';
    
    printf("Программа начала работу\n");

    err = pipe2(pipefd, O_NONBLOCK);
    if(err != 0)
    {
        printf("Канал не был создан");
        return 1;    
    }

    err = pthread_create(&id1,  NULL, proc1, &arg1);
    if(err != 0)
    {
        printf("Поток не был запущен");
        return 2;    
    }

    err = pthread_create(&id2, NULL, proc2, &arg2);
    if(err != 0)
    {
        printf("Поток не был запущен");
        return 3;    
    }

    printf("Программа ждет нажатия клавиши\n"); 

    getchar();

    printf("Клавиша нажата\n");
    
    arg1.flag = 1;
    arg2.flag = 1;

    pthread_join(id1, (void**)&exit1);
    pthread_join(id2, (void**)&exit2);

    printf("Поток %d закончил работу\n", exit1);
    printf("Поток %d закончил работу\n", exit2);

    close(pipefd[0]);
    close(pipefd[1]);

    printf("Программа закончила работу\n"); 

    return 0;
}