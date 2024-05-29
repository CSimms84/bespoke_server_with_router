#include "./server.h"
#include "./router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void hello_handler(int client_socket) {
    const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";
    write(client_socket, response, strlen(response));
}

int main() {
    add_route("/hello", hello_handler);
    start_server("8080");
    return 0;
}

