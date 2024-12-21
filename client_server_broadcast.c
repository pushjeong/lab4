#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENTS 3
#define MESSAGE_LENGTH 256

char message[MESSAGE_LENGTH];
int message_ready = 0;  // 메시지 준비 상태 플래그
pthread_mutex_t mutex;
pthread_cond_t cond;

// 서버 쓰레드 함수
void *server_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // 메시지가 준비될 때까지 대기
        while (!message_ready) {
            pthread_cond_wait(&cond, &mutex);
        }

        // 메시지 브로드캐스트
        printf("Server broadcasting message: %s\n", message);

        // 메시지 플래그 초기화
        message_ready = 0;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// 클라이언트 쓰레드 함수
void *client_thread(void *arg) {
    int client_id = *(int *)arg;
    char local_message[MESSAGE_LENGTH];

    while (1) {
        sleep(rand() % 5 + 1);  // 랜덤 대기 시간

        // 메시지 준비
        snprintf(local_message, MESSAGE_LENGTH, "Hello from client %d", client_id);

        pthread_mutex_lock(&mutex);

        // 메시지를 서버에 보냄
        strncpy(message, local_message, MESSAGE_LENGTH);
        message_ready = 1;

        printf("Client %d sent message: %s\n", client_id, local_message);

        // 조건 변수 신호
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);

        sleep(1);  // 클라이언트가 메시지를 자주 보내지 않도록 조절
    }

    return NULL;
}

int main() {
    pthread_t server_tid;
    pthread_t client_tids[MAX_CLIENTS];
    int client_ids[MAX_CLIENTS];

    // 뮤텍스 및 조건 변수 초기화
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // 서버 쓰레드 생성
    pthread_create(&server_tid, NULL, server_thread, NULL);

    // 클라이언트 쓰레드 생성
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_ids[i] = i + 1;
        pthread_create(&client_tids[i], NULL, client_thread, &client_ids[i]);
    }

    // 클라이언트 쓰레드 대기
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pthread_join(client_tids[i], NULL);
    }

    // 서버 쓰레드 대기 (사실상 여기서 프로그램은 무한 루프에 의해 종료되지 않음)
    pthread_join(server_tid, NULL);

    // 뮤텍스 및 조건 변수 해제
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}

