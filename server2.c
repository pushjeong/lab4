#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int client_sockets[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t mutex;

void broadcast_message(const char *message, int sender_socket) {
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] != sender_socket) {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }

    pthread_mutex_unlock(&mutex);
}

void *client_handler(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("Message from client: %s\n", buffer);
        broadcast_message(buffer, client_socket);
    }

    // 클라이언트 종료 처리
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] == client_socket) {
            for (int j = i; j < client_count - 1; j++) {
                client_sockets[j] = client_sockets[j + 1];
            }
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    close(client_socket);
    printf("Client disconnected\n");
    return NULL;
}

int main() {
    int server_socket, new_socket;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    pthread_t thread_id;

    pthread_mutex_init(&mutex, NULL);

    // 소켓 생성
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 소켓 바인딩
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // 연결 대기
    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        if ((new_socket = accept(server_socket, (struct sockaddr *)&address, &addr_len)) < 0) {
            perror("Accept failed");
            continue;
        }

        pthread_mutex_lock(&mutex);
        if (client_count < MAX_CLIENTS) {
            client_sockets[client_count++] = new_socket;
            printf("New client connected\n");

            pthread_create(&thread_id, NULL, client_handler, &new_socket);
            pthread_detach(thread_id);
        } else {
            printf("Maximum client limit reached. Connection refused.\n");
            close(new_socket);
        }
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_destroy(&mutex);
    close(server_socket);
    return 0;
}
