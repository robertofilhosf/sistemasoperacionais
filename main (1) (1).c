#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#define X 5
#define pensar 0
#define fome 1
#define comer 2
#define esquerda (phnum + 4) % X
#define direita (phnum + 1) % X

int phil[X];
int tempo = 200;

unsigned int seed;

typedef struct {
    int state[X];
    sem_t mutex;
    sem_t s[X];
} Monitor;

Monitor phil_object;

void monitor_in(Monitor *m) {
    sem_init(&m->mutex, 0, 1);
    for (int j = 0; j < X; j++) {
        sem_init(&m->s[j], 0, 0);
        m->state[j] = pensar;
    }
}

void testando(int phnum) {
    if (phil_object.state[phnum] == fome && phil_object.state[esquerda] != comer && phil_object.state[direita] != comer) {
        phil_object.state[phnum] = comer;
        sem_post(&phil_object.s[phnum]);
    }
}

void take_fork(int phnum) {
    sem_wait(&phil_object.mutex);

    phil_object.state[phnum] = fome;
    printf("filosofo %d esta com fome\n", phnum + 1);

    srand(seed);
    usleep((rand() % 100) / 50.0);

    testando(phnum);

    sem_post(&phil_object.mutex);

    sem_wait(&phil_object.s[phnum]);

    phil_object.state[phnum] = comer;
    printf("filosofo %d pegou os garfos e esta comendo\n", phnum + 1);
}

void put_fork(int phnum) {
    sem_wait(&phil_object.mutex);

    phil_object.state[phnum] = pensar;
    printf("filosofo %d soltou os garfos e esta pensando\n", phnum + 1);

    testando(esquerda);
    testando(direita);

    sem_post(&phil_object.mutex);
}

void* filosofo(void* arg) {
    int c = 0;

    while (c < tempo) {
        int j = *(int*)arg;
        usleep(3500000);
        take_fork(j);
        usleep(3500000);
        put_fork(j);
        c++;
    }
}

int main() {
    pthread_t thread_id[X];
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    seed = (unsigned int)time(NULL);
    monitor_in(&phil_object);

    for (int j = 0; j < X; j++) {
        phil[j] = j;
    }

    for (int j = 0; j < X; j++) {
        pthread_create(&thread_id[j], &attr, filosofo, &phil[j]);
        printf("filosofo %d esta pensando\n", j + 1);
    }

    for (int j = 0; j < X; j++) {
        pthread_join(thread_id[j], NULL);
    }

    pthread_attr_destroy(&attr);

    return 0;
}
