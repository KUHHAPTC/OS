#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

typedef struct
{
    int flag;
    char symbol;
}gargs;

void * proc1(void *arg){
    gargs * args = (gargs*) arg; 

    printf("Поток %c начал работу\n", args->symbol);

    while (args->flag == 0) {
	    putchar(args->symbol);
	    fflush(stdout);
	    sleep(2);
    }

    pthread_exit((void*)1);
 
}

void * proc2(void *arg){
    gargs * args = (gargs*) arg;

    printf("Поток %c начал работу\n", args->symbol);

    while (args->flag == 0) {
	    putchar(args->symbol);
	    fflush(stdout);
	    sleep(2);
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

    printf("Программа закончила работу\n"); 

    return 0;
}