#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include "../include/router.h"
#include <stdio.h>
#include <string.h>

#define MAX_ROUTES 100
#define BUFFER_SIZE 1024

typedef struct {
    char path[256];
    route_handler_t handler;
} route_t;

static route_t routes[MAX_ROUTES];
static int route_count = 0;

void add_route(const char *path, route_handler_t handler) {
    if (route_count < MAX_ROUTES) {
        strncpy(routes[route_count].path, path, sizeof(routes[route_count].path) - 1);
        routes[route_count].handler = handler;
        route_count++;
    }
}

void handle_request(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    recv(client_socket, buffer, BUFFER_SIZE, 0);

    char *request = strtok(buffer, " ");
    char *path = strtok(NULL, " ");

    for (int i = 0; i < route_count; i++) {
        if (strcmp(routes[i].path, path) == 0) {
            routes[i].handler(client_socket);
            return;
        }
    }

    // Default 404 response
    const char *response = "HTTP/1.1 404 Not Found\r\n\r\n";
    send(client_socket, response, strlen(response), 0);
}

