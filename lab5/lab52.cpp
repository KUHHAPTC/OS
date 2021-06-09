#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

#define SEMAPHORE "/my_sem"

using namespace std;

typedef struct {
    int flag;
    char symbol;
} args_s;

sem_t *sem;
FILE *fp;

void * proc(void *arg){
    
    args_s *args = (args_s*) arg;

    printf("Поток %c начал работу\n", args->symbol);

    while (args->flag == 0) {
        sem_wait(sem);
        for(int i = 0; i < 3; i++)
        {
            putchar(args->symbol);
            fwrite(&args->symbol, sizeof(char), 1, fp);
            fflush(fp);
            fflush(stdout);
            sleep(1);
        }
        sem_post(sem);
        sleep(1);
    }

    pthread_exit((void*)2);
}

int main(){

    args_s arg;
    int exit, err = 0;
    pthread_t id;
    if ((sem = sem_open(SEMAPHORE, O_CREAT, 0644, 1)) == SEM_FAILED ) 
    {
        perror("sem_open");
        return 1;
    }

    fp = fopen("test.txt","a+");

    arg.flag = 0;
    arg.symbol = '2';

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

    printf("Поток %d закончил работу\n", exit);

    fclose(fp);

    if(sem_close(sem) < 0)
        perror("sem_close");

    if(sem_unlink(SEMAPHORE) != 0)
        perror("sem_unlink");

    printf("Программа закончила работу\n"); 

    return 0;
}