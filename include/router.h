#ifndef ROUTER_H
#define ROUTER_H

typedef void (*route_handler_t)(int client_socket);

void add_route(const char *path, route_handler_t handler);

void handle_request(int client_socket);

#endif