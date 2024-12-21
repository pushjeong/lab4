#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int count = 0;  // 현재 버퍼에 있는 항목 수
pthread_mutex_t mutex;
pthread_cond_t not_empty, not_full;

void *producer(void *arg) {
    int item = 0;
    while (1) {
        pthread_mutex_lock(&mutex);

        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&not_full, &mutex);
        }

        buffer[count++] = ++item;
        printf("Produced: %d\n", item);

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
}

void *consumer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (count == 0) {
            pthread_cond_wait(&not_empty, &mutex);
        }

        int item = buffer[--count];
        printf("Consumed: %d\n", item);

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);

        sleep(2);
    }
}

int main() {
    pthread_t producer_thread, consumer_thread;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full, NULL);

    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);

    return 0;
}
