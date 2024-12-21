#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *thread_function(void *arg) {
    printf("Hello from thread! ID: %ld\n", pthread_self());
    return NULL;
}

int main() {
    pthread_t thread_id;
    int status;

    // 쓰레드 생성
    status = pthread_create(&thread_id, NULL, thread_function, NULL);
    if (status != 0) {
        perror("pthread_create failed");
        exit(1);
    }

    // 쓰레드 종료 대기
    pthread_join(thread_id, NULL);
    printf("Thread has finished execution.\n");

    return 0;
}
