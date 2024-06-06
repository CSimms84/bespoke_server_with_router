#ifndef ROUTER_H
#define ROUTER_H

#include <stdio.h> 

// Route handling function pointer type
typedef void (*route_handler_t)(int client_socket);

// Function prototypes for adding routes and handling requests
void add_route(const char *path, route_handler_t handler);
void handle_request(int client_socket);

#endif // ROUTER_H
