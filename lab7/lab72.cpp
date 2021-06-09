#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUF_SIZE 256

using namespace std;
int fd;

typedef struct {
    int flag;
} args_s;

void * proc(void *arg){
    
    args_s *args = (args_s*) arg;
    char buf[BUF_SIZE];
    int nr;

    printf("Поток чтения начал работу\n");

    while (args->flag == 0) {
        buf[0] = 0;
        nr = read(fd, &buf, BUF_SIZE);  
        if(nr == -1) {
            perror("read");
            sleep(1);
        }
        else if (nr == 0) {
            printf("Fifo is not opened or closed\n");
            sleep(1);
        }
        else{
            cout << "(" << buf << ")";
            fflush(stdout);
            sleep(1);
        }
    }

    pthread_exit((void*)2);
}

int main(){

    args_s arg;
    int exit, err = 0;
    pthread_t id;

    err = mkfifo("/tmp/my_named_pipe", 0644);
    if(err != 0)
    {
        perror("mkfifo");
    }

    fd = open("/tmp/my_named_pipe", O_RDONLY|O_NONBLOCK);

    arg.flag = 0;

    printf("Программа начала работу\n"); 

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

    printf("Поток чтения закончил работу\n");

    close(fd);
    
    err = unlink("/tmp/my_named_pipe");
    if(err != 0)
        perror("unlink");


    printf("Программа закончила работу\n"); 

    return 0;
}