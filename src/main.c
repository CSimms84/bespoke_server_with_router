#include <pthread.h>
#include "../include/server.h"
#include "../include/router.h"
#include "../include/tcp_server.h"
#include "../include/packet_sniffer.h"
#include "../include/dns_server.h"
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

void* run_packet_sniffer(void* arg) {
    start_packet_sniffer();
    return NULL;
}

int main() {
    pthread_t http_thread, tcp_thread, sniffer_thread, dns_thread;

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

    // Start the packet sniffer in a new thread
    if (pthread_create(&sniffer_thread, NULL, run_packet_sniffer, NULL) != 0) {
        perror("Failed to create packet sniffer thread");
        return 1;
    }

    // Start the DNS server in a new thread
    if (pthread_create(&dns_thread, NULL, run_dns_server, NULL) != 0) {
        perror("Failed to create DNS server thread");
        return 1;
    }

    // Wait for all threads to finish
    pthread_join(http_thread, NULL);
    pthread_join(tcp_thread, NULL);
    pthread_join(sniffer_thread, NULL);
    pthread_join(dns_thread, NULL);

    return 0;
}
