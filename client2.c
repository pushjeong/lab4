#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void *receive_messages(void *arg) {
    int sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = read(sock, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("Message from server: %s\n", buffer);
    }

    return NULL;
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    pthread_t thread_id;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server\n");

    // 메시지 수신 쓰레드 생성
    pthread_create(&thread_id, NULL, receive_messages, &sock);

    // 메시지 전송
    char buffer[BUFFER_SIZE];
    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sock, buffer, strlen(buffer), 0);
    }

    close(sock);
    return 0;
}
