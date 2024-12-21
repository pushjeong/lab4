#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *message = "Hello from client";
    char buffer[BUFFER_SIZE] = {0};

    // 소켓 파일 디스크립터 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 서버 주소 변환
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // 서버에 연결
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // 메시지 전송
    send(sock, message, strlen(message), 0);
    printf("Message sent to server: %s\n", message);

    // 서버로부터 응답 읽기
    read(sock, buffer, BUFFER_SIZE);
    printf("Message from server: %s\n", buffer);

    close(sock);
    return 0;
}
