#include <iostream>
#include <pthread.h>
#include <unistd.h>                                                             
#include <errno.h>
#include <string.h>


using namespace std;


typedef struct {
    int flag;
    char symbol;

    pthread_mutex_t *mutex;
} use_mutex;


pthread_mutex_t mutex;


void * proc1(void *arg){
    
    use_mutex *args = (use_mutex*) arg;
    int k;

    printf("Поток %c начал работу\n", args->symbol);

    while (args->flag == 0) {
        k = pthread_mutex_trylock(args->mutex);
        while(k != 0) {
            cout << " " << strerror(k) << endl;
            k = pthread_mutex_trylock(args->mutex);
            sleep(1);
        }
        for(int i = 0; i < 3; i++)
        {
            putchar(args->symbol);
            fflush(stdout);
            sleep(1);
        }
        pthread_mutex_unlock(args->mutex);
	    sleep(1);
    }

    pthread_exit((void*)1);
}

void * proc2(void *arg){
    
    use_mutex *args = (use_mutex*) arg;
    int k;

    printf("Поток %c начал работу\n", args->symbol);

    while (args->flag == 0) {
        k = pthread_mutex_trylock(args->mutex);
        while(k != 0) {
            cout << " " << strerror(k) << endl;
            k = pthread_mutex_trylock(args->mutex);
            sleep(1);
        }
        for(int i = 0; i < 3; i++)
        {
            putchar(args->symbol);
            fflush(stdout);
            sleep(1);
        }
        pthread_mutex_unlock(args->mutex);
	    sleep(1);

    } 

    pthread_exit((void*)2);
}



int main(){

    use_mutex arg1, arg2;
    int exit1, exit2;
    pthread_t id1, id2;
    int err = 0;

    arg1.flag = 0;
    arg1.symbol = '1';
    arg2.flag = 0;
    arg2.symbol = '2';

    pthread_mutex_init(&mutex, NULL);

    arg1.mutex = &mutex;
    arg2.mutex = &mutex;

    printf("Программа начала работу\n"); 

    err = pthread_create(&id1, NULL, proc1, &arg1);
    if(err != 0)
    {
        printf("Поток не был запущен");
        return 1;    
    }
   
    err = pthread_create(&id2, NULL, proc2, &arg2);
    if(err != 0)
    {
           printf("Поток не был запущен");
        return 1;    
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

    pthread_mutex_destroy(&mutex);    

    printf("Программа закончила работу\n"); 

    return 0;
}