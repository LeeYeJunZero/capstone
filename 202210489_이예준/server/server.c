#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <sys/stat.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024
#define ROOT "."

const char* get_content_type(const char* path) {
    const char* ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    return "application/octet-stream";
}

void send_response(SOCKET client_socket, const char* path) {
    char full_path[BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s", ROOT, path);

    // 파일 경로 출력
    printf("Requested path: %s\n", full_path);

    FILE* file = fopen(full_path, "rb");
    if (!file) {
        const char* not_found_response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
        send(client_socket, not_found_response, strlen(not_found_response), 0);
        printf("File not found: %s\n", full_path);  // 추가된 디버깅 메시지
        return;
    }

    struct stat st;
    stat(full_path, &st);
    long file_size = st.st_size;

    char response_header[BUFFER_SIZE];
    snprintf(response_header, sizeof(response_header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n", get_content_type(full_path), file_size);

    send(client_socket, response_header, strlen(response_header), 0);

    char* file_buffer = malloc(file_size);
    fread(file_buffer, 1, file_size, file);
    send(client_socket, file_buffer, file_size, 0);

    free(file_buffer);
    fclose(file);
}

void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    int read_size = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (read_size == SOCKET_ERROR) {
        perror("recv failed");
        closesocket(client_socket);
        return;
    }

    buffer[read_size] = '\0';

    char method[16], path[256], protocol[16];
    sscanf_s(buffer, "%s %s %s", method, (unsigned)_countof(method), path, (unsigned)_countof(path), protocol, (unsigned)_countof(protocol));

    if (strcmp(method, "GET") == 0) {
        if (strcmp(path, "/") == 0) {
            strcpy_s(path, sizeof(path), "/index.html");
        }
        send_response(client_socket, path);
    }
    else {
        const char* method_not_allowed_response = "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n";
        send(client_socket, method_not_allowed_response, strlen(method_not_allowed_response), 0);
    }

    closesocket(client_socket);
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code: %d", WSAGetLastError());
        return 1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d", WSAGetLastError());
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 3) == SOCKET_ERROR) {
        printf("Listen failed with error code: %d", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    while ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) != INVALID_SOCKET) {
        handle_client(client_socket);
    }

    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed with error code: %d", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}
