#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <netdb.h>
#include <cstring>

#define SEMAPHOREW "/my_sem_w"
#define SEMAPHORER "/my_sem_r"
#define SHARED "my_shared_memory"

using namespace std;

typedef struct {
    int flag;
} args_s;

sem_t *sem_w, *sem_r;
int *addr;

void * proc(void *arg){

    int k, s;

    struct addrinfo *result;
    
    args_s *args = (args_s*) arg;

    s = getaddrinfo("localhost", NULL, NULL, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    while (args->flag == 0) {

        printf("%d ", result->ai_protocol);
        fflush(stdout);

        memcpy(addr, &result->ai_protocol, sizeof(result->ai_protocol));

        sem_post(sem_w); // count_sem_w++ (= 1)

        k = sem_trywait(sem_r); // count_sem_r-- (= -1)
        while(k != 0) {
            cout << "Reading is busy :" << endl;
            sleep(1);
            k = sem_trywait(sem_r);
        }
 
        sleep(1);       
        result->ai_protocol++;
    }

    pthread_exit((void*)1);
}

int main(){

    args_s arg;
    int err = 0;
    pthread_t id;

    int shm = shm_open(SHARED, O_CREAT|O_RDWR, 0644);

    ftruncate(shm, sizeof(int));  

    addr = (int*) mmap(0, sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, shm, 0);
    if (addr == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    if ((sem_w = sem_open(SEMAPHOREW, O_CREAT, 0644, 0)) == SEM_FAILED ) 
    {
        perror("sem_open");
        return 2;
    }
    if ((sem_r = sem_open(SEMAPHORER, O_CREAT, 0644, 0)) == SEM_FAILED ) 
    {
        perror("sem_open");
        return 3;
    }

    arg.flag = 0;

    printf("Программа начала работу\n"); 

    err = pthread_create(&id, NULL, proc, &arg);
    if(err != 0)
    {
        perror("pthread_create");
        return 4;    
    }

    printf("Программа ждет нажатия клавиши\n"); 

    getchar();

    printf("Клавиша нажата\n"); 
    
    arg.flag = 1;

    pthread_join(id, (void**)&exit);

    printf("Поток %d закончил работу\n", exit);

    if(sem_close(sem_w) < 0)
        perror("sem_close_write");

    if(sem_unlink(SEMAPHOREW) != 0)
        perror("sem_unlink_write");

    if(sem_close(sem_r) < 0)
        perror("sem_close_read");

    if(sem_unlink(SEMAPHORER) != 0)
        perror("sem_unlink_read");

    munmap(addr, sizeof(int));

    close(shm);

    shm_unlink(SHARED);

    printf("Программа закончила работу\n"); 

    return 0;
}