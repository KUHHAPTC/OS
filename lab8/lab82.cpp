#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <netdb.h>
#include <cstring>
#include <mqueue.h>

#define QUEUE "/myqueue"

using namespace std;

typedef struct {
    int flag;
} args_s;

struct mq_attr attr;
mqd_t msg_q;

void * proc(void *arg){

    int res;

    char buf[attr.mq_msgsize + 1];
    
    args_s *args = (args_s*) arg;

    while (args->flag == 0) {

        fflush(stdout);
        res = mq_receive(msg_q, buf, attr.mq_msgsize + 1, 0);

        if (res > 0) {
            cout << "mq_receive: result = " << res << ",  Received: " << buf  << endl;
        }else if (res == -1) {
            perror("mq_receive");
            sleep(1);
            continue;
        }

        sleep(1);       
    }

    pthread_exit((void*)1);
}

int main(){

    args_s arg;
    int exit, err = 0;
    pthread_t id;

    arg.flag = 0;
    attr.mq_maxmsg = 20;
    attr.mq_msgsize = 10000;
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;

    printf("Программа начала работу\n"); 
    msg_q = mq_open(QUEUE, O_CREAT | O_RDONLY | O_NONBLOCK, 0644, &attr);
    if(msg_q == -1)
    {
        perror("mq_open");
        cout << msg_q << " " << errno << " " << strerror(errno);
        return 1;
    }

    mq_getattr(msg_q, &attr);
    printf("Maximum # of messages on queue:   %ld\n", attr.mq_maxmsg);
    printf("Maximum message size:             %ld\n", attr.mq_msgsize);

    err = pthread_create(&id, NULL, proc, &arg);
    if(err != 0)
    {
        perror("pthread_create");
        return 2;    
    }

    printf("Программа ждет нажатия клавиши\n"); 

    getchar();

    printf("Клавиша нажата\n"); 
    
    arg.flag = 1;

    pthread_join(id, (void**)&exit);

    printf("Поток %d закончил работу\n", exit);

    err = mq_unlink(QUEUE);
    if(err == -1)
    {
        perror("mq_unlink");
        return 3;
    }

    err = mq_close(msg_q);
    if(err == -1)
    {
        perror("mq_close");
        return 4;
    }

    printf("Программа закончила работу\n"); 

    return 0;
}