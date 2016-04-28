#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

sem_t empty, full, mutex;
int cnt = 0;
int n, m, tot;
int buf[200], bg = 0, ed = 0;

void* senda(void* arg)
{
    int no = *(int*)arg;
    for(int flag = 1; flag;)
    {
        sem_wait(&empty);
        sem_wait(&mutex);
        if (cnt < tot)
        {
            printf("sender %d send message %d, %d remain in buf\n", no, cnt, m-(ed-bg+m-1)%m);
            buf[ed] = cnt++;
            ed = (ed+1)%m;
        } else
            flag = 0;
        sem_post(&mutex);
        sem_post(&full);
        sleep(rand()%100/1e1);
    }
    return NULL;
}

void* receiveb(void* arg)
{
    for(int flag = 1; flag;)
    {
        sem_wait(&full);
        sem_wait(&mutex);
        printf("receiver get message %d, %d remain in buf\n", buf[bg], m-(ed-bg+m-1)%m);
        bg = (bg+1)%m;
        if (bg == ed && cnt == tot)
            flag = 0;
        sem_post(&mutex);
        sem_post(&empty);
        sleep(rand()%100/1e1);
    }
    return NULL;
    return NULL;
}

int main()
{
    pthread_t pa[100], pb;
    int no[100];
    srand(time(0));
    printf("please input N(threads) M(buf_size) MSG(messages), such as 3 5 10:\n");
    scanf("%d%d%d", &n, &m, &tot);
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, m);
    sem_init(&full, 0, 0);

    for (int i = 0; i < n; ++i)
    {
        no[i] = i;
        pthread_create(pa+i, NULL, senda, no+i);
    }
    pthread_create(&pb, NULL, receiveb, NULL);
    for (int i = 0; i < n; ++i)
        pthread_join(pa[i], NULL);
    pthread_join(pb, NULL);
    return 0;
}
