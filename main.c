
/* Producer-Consumer using mutex and thread*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX 5

int buff[MAX];

int fill;
int use;

sem_t full;
sem_t empty;
sem_t sem_mtx;
pthread_mutex_t mutex;

void put(int value)
{
    buff[fill] = value;
    fill = (fill + 1) % MAX;
}

int get()
{
    int tmp = buff[use];
    use = (use + 1) % MAX;
    return tmp;
}

void *producer(void *args)
{
    while (1)
    {
        sleep(1);
        sem_wait(&empty);
        // pthread_mutex_lock(&mutex);
        sem_wait(&sem_mtx);

        printf("Thread [%d] put\n", *(int *)args);
        int tmp = rand() % 10;
        put(tmp);
        printf("Put:%d\n", tmp);
        // pthread_mutex_unlock(&mutex);
        sem_post(&sem_mtx);
        sem_post(&full);
    }
}

void *consumer(void *args)
{
    int tmp = 0;
    while (1)
    {
        sleep(1);
        sem_wait(&full);
        // pthread_mutex_lock(&mutex);
        sem_wait(&sem_mtx);

        printf("Thread [%d] get\n", *(int *)args);
        tmp = get();
        printf("Get:%d\n", tmp);
        // pthread_mutex_unlock(&mutex);
        sem_post(&sem_mtx);
        sem_post(&empty);
    }
}

int main()
{
    pthread_t th[10];

    sem_init(&full, 0, 0);
    sem_init(&empty, 0, MAX);
    sem_init(&sem_mtx, 0, 1);
    // pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < 10; i++)
    {
        int *a = (int *)malloc(sizeof(int));
        (*a) = i;
        if (i < 2)
        {
            if (pthread_create(&th[i], NULL, &producer, (void *)a) != 0)
            {
                perror("Failed to create thread");
            }
        }
        else
        {
            if (pthread_create(&th[i], NULL, &consumer, (void *)a) != 0)
            {
                perror("Failed to create thread");
            }
        }
    }

    for (int i = 0; i < MAX; i++)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Failed to create thread");
        }
    }

    sem_destroy(&full);
    sem_destroy(&empty);
    sem_destroy(&sem_mtx);
    // pthread_mutex_destroy(&mutex);

    return 0;
}