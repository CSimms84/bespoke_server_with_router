#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/tcp_server.h"

#define BUFFER_SIZE 1024

void handle_tcp_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    int bytes_read;
    
#ifdef _WIN32
    bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);
#else
    bytes_read = read(client_socket, buffer, BUFFER_SIZE);
#endif

    if (bytes_read > 0) {
        printf("Received message: %s\n", buffer);
        
#ifdef _WIN32
        send(client_socket, buffer, bytes_read, 0);
#else
        write(client_socket, buffer, bytes_read);
#endif
    }
    
#ifdef _WIN32
    closesocket(client_socket);
#else
    close(client_socket);
#endif
}

void start_tcp_server(const char *port) {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }
#endif

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(port));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("TCP server listening on port %s\n", port);

    while ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0) {
        handle_tcp_client(client_socket);
    }

    if (client_socket < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

#ifdef _WIN32
    closesocket(server_fd);
    WSACleanup();
#else
    close(server_fd);
#endif
}
