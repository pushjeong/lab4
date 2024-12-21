#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void handle_get_request(int client_socket, const char *resource) {
    char response[BUFFER_SIZE];
    if (strcmp(resource, "/") == 0) {
        resource = "/index.html";
    }

    FILE *file = fopen(resource + 1, "r");  // Remove leading '/' from resource
    if (file == NULL) {
        snprintf(response, sizeof(response),
                 "HTTP/1.1 404 Not Found\r\n"
                 "Content-Type: text/plain\r\n"
                 "\r\n"
                 "404 Not Found");
        send(client_socket, response, strlen(response), 0);
    } else {
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n\r\n");
        send(client_socket, response, strlen(response), 0);

        while (fgets(response, sizeof(response), file) != NULL) {
            send(client_socket, response, strlen(response), 0);
        }
        fclose(file);
    }
}

void handle_post_request(int client_socket, const char *body) {
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n"
             "\r\n"
             "POST data received:\n%s",
             body);
    send(client_socket, response, strlen(response), 0);
}

void handle_cgi_request(int client_socket, const char *script, const char *method, const char *body) {
    int pipe_fd[2];
    pipe(pipe_fd);

    pid_t pid = fork();
    if (pid == 0) {
        // 자식 프로세스
        dup2(pipe_fd[1], STDOUT_FILENO);  // CGI 출력 → 서버로 전달
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        // CGI 환경 변수 설정
        setenv("REQUEST_METHOD", method, 1);

        char content_length[32];
        snprintf(content_length, sizeof(content_length), "%zu", body ? strlen(body) : 0);
        setenv("CONTENT_LENGTH", content_length, 1);

        execl(script + 1, script + 1, NULL);  // CGI 실행
        perror("execl failed");
        exit(1);
    } else {
        // 부모 프로세스
        close(pipe_fd[1]);

        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n\r\n");
        send(client_socket, response, strlen(response), 0);

        int n;
        while ((n = read(pipe_fd[0], response, sizeof(response))) > 0) {
            send(client_socket, response, n, 0);
        }
        close(pipe_fd[0]);
        wait(NULL);
    }
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    read(client_socket, buffer, sizeof(buffer) - 1);

    char method[16], resource[256], protocol[16], body[BUFFER_SIZE];
    sscanf(buffer, "%s %s %s", method, resource, protocol);

    if (strcasecmp(method, "GET") == 0) {
        if (strstr(resource, ".cgi")) {
            handle_cgi_request(client_socket, resource, "GET", NULL);
        } else {
            handle_get_request(client_socket, resource);
        }
    } else if (strcasecmp(method, "POST") == 0) {
        char *body_ptr = strstr(buffer, "\r\n\r\n");
        if (body_ptr) {
            strcpy(body, body_ptr + 4);
        } else {
            body[0] = '\0';
        }
        if (strstr(resource, ".cgi")) {
            handle_cgi_request(client_socket, resource, "POST", body);
        } else {
            handle_post_request(client_socket, body);
        }
    } else {
        char response[] = "HTTP/1.1 400 Bad Request\r\n\r\n";
        send(client_socket, response, strlen(response), 0);
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&address, &addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }
        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}
