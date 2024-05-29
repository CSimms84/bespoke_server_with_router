#include <pthread.h>
#include "./server.h"
#include "./router.h"
#include "./tcp_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void hello_handler(int client_socket) {
    const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";
    write(client_socket, response, strlen(response));
}

void* run_http_server(void* arg) {
    add_route("/hello", hello_handler);
    start_server("8080");
    return NULL;
}

void* run_tcp_server(void* arg) {
    start_tcp_server("9090");
    return NULL;
}

int main() {
    pthread_t http_thread, tcp_thread;

    // Start the HTTP server in a new thread
    if (pthread_create(&http_thread, NULL, run_http_server, NULL) != 0) {
        perror("Failed to create HTTP server thread");
        return 1;
    }

    // Start the TCP server in a new thread
    if (pthread_create(&tcp_thread, NULL, run_tcp_server, NULL) != 0) {
        perror("Failed to create TCP server thread");
        return 1;
    }

    // Wait for both threads to finish
    pthread_join(http_thread, NULL);
    pthread_join(tcp_thread, NULL);

    return 0;
}

