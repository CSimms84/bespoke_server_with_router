#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/router.h"
#include "../include/dns_server.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h> // needed for exit and EXIT_FAILURE

// Forward declaration of functions to be tested
void add_route(const char *path, route_handler_t handler);
void handle_request(int client_socket);
void* run_dns_server(void* arg);

// A dummy client socket implementation for testing
void dummy_hello_handler(int client_socket) {
    const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";
    write(client_socket, response, strlen(response));
}

// Helper function to create a pipe and return file descriptors
void create_pipe(int fds[2]) {
    if (pipe(fds) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
}

// Helper function to read from a pipe
void read_from_pipe(int fd, char *buffer, size_t size) {
    ssize_t bytes_read = read(fd, buffer, size - 1);
    if (bytes_read == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0'; // Null-terminate the string
}

// Test case for add_route
void test_add_route() {
    add_route("/hello", dummy_hello_handler);

    // Checking that the route was added by simulating a request to "/hello"
    int fds[2];
    create_pipe(fds);
    int client_socket = fds[1];

    handle_request(client_socket);
    close(client_socket);

    char buffer[1024];
    read_from_pipe(fds[0], buffer, sizeof(buffer));
    close(fds[0]);

    printf("Buffer contents: %s\n", buffer);

    CU_ASSERT_STRING_EQUAL(buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!");
}

// Test case for handle_request
void test_handle_request() {
    // A route and a simulated request to it
    add_route("/test", dummy_hello_handler);

    // Creating a pipe to simulate client socket
    int fds[2];
    create_pipe(fds);
    int client_socket = fds[1];

    // A dummy HTTP request to the client socket
    const char *request = "GET /test HTTP/1.1\r\nHost: localhost\r\n\r\n";
    write(client_socket, request, strlen(request));
    lseek(client_socket, 0, SEEK_SET);

    handle_request(client_socket);
    close(client_socket);

    char buffer[1024];
    read_from_pipe(fds[0], buffer, sizeof(buffer));
    close(fds[0]);

    printf("Buffer contents: %s\n", buffer);

    CU_ASSERT_STRING_EQUAL(buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!");
}

// Test case for DNS server
void test_dns_server() {
    // DNS server runs indefinitely so I have to test handler instead
    // Creating a dummy DNS query
    unsigned char query[] = {
        0x12, 0x34, // ID
        0x01, 0x00, // Flags (standard query)
        0x00, 0x01, // QDCOUNT (1 question)
        0x00, 0x00, // ANCOUNT
        0x00, 0x00, // NSCOUNT
        0x00, 0x00, // ARCOUNT
        // Question section
        0x07, 'a', 'n', 'a', 'd', 'a', 'p', 't','i','v','e', 
        0x03, 'c', 'o', 'm', 0x00, // QNAME (anadaptive.com)
        0x00, 0x01, // QTYPE (A)
        0x00, 0x01  // QCLASS (IN)
    };
    
    // Buffer to hold response
    unsigned char response[512];

    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr("127.0.0.1");
    client.sin_port = htons(12345);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    bind(sock, (struct sockaddr *)&client, sizeof(client));

    struct sockaddr_in server;
    socklen_t server_len = sizeof(server);

    // Simulation of receiving a DNS query
    sendto(sock, query, sizeof(query), 0, (struct sockaddr *)&server, server_len);
    recvfrom(sock, response, sizeof(response), 0, (struct sockaddr *)&server, &server_len);

    // Check if the response is valid
    CU_ASSERT(response[2] & 0x80); // QR bit should be set to 1 (response)
    CU_ASSERT_EQUAL(response[3] & 0x0F, 0); // RCODE should be 0 (no error)

    close(sock);
}

int main() {
    CU_initialize_registry();

    CU_pSuite pSuite = CU_add_suite("Suite_1", 0, 0);

    CU_add_test(pSuite, "test_add_route", test_add_route);
    CU_add_test(pSuite, "test_handle_request", test_handle_request);
    CU_add_test(pSuite, "test_dns_server", test_dns_server);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
