#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <pthread.h>
#endif

#include "../include/server.h"
#include "../include/router.h"
#include "../include/tcp_server.h"
#include "../include/packet_sniffer.h"
#include "../include/dns_server.h"

void hello_handler(int client_socket) {
    const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";
    write(client_socket, response, strlen(response));
}

#ifdef _WIN32
DWORD WINAPI run_http_server(LPVOID arg) {
#else
void* run_http_server(void* arg) {
#endif
    add_route("/hello", hello_handler);
    start_server("8080");
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

#ifdef _WIN32
DWORD WINAPI run_tcp_server(LPVOID arg) {
#else
void* run_tcp_server(void* arg) {
#endif
    start_tcp_server("9090");
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

#ifdef _WIN32
DWORD WINAPI run_packet_sniffer(LPVOID arg) {
#else
void* run_packet_sniffer(void* arg) {
#endif
    start_packet_sniffer();
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    HANDLE http_thread, tcp_thread, sniffer_thread, dns_thread;
#else
    pthread_t http_thread, tcp_thread, sniffer_thread, dns_thread;
#endif

#ifdef _WIN32
    // Start the HTTP server in a new thread
    http_thread = CreateThread(NULL, 0, run_http_server, NULL, 0, NULL);
    if (http_thread == NULL) {
        perror("Failed to create HTTP server thread");
        return 1;
    }

    // Start the TCP server in a new thread
    tcp_thread = CreateThread(NULL, 0, run_tcp_server, NULL, 0, NULL);
    if (tcp_thread == NULL) {
        perror("Failed to create TCP server thread");
        return 1;
    }

    // Start the packet sniffer in a new thread
    sniffer_thread = CreateThread(NULL, 0, run_packet_sniffer, NULL, 0, NULL);
    if (sniffer_thread == NULL) {
        perror("Failed to create packet sniffer thread");
        return 1;
    }

    // Start the DNS server in a new thread
    dns_thread = CreateThread(NULL, 0, run_dns_server, NULL, 0, NULL);
    if (dns_thread == NULL) {
        perror("Failed to create DNS server thread");
        return 1;
    }

    // Wait for all threads to finish
    WaitForSingleObject(http_thread, INFINITE);
    WaitForSingleObject(tcp_thread, INFINITE);
    WaitForSingleObject(sniffer_thread, INFINITE);
    WaitForSingleObject(dns_thread, INFINITE);
#else
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
#endif

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
