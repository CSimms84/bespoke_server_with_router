#ifndef DNS_SERVER_H
#define DNS_SERVER_H

#ifdef _WIN32
// Include Windows headers
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib
#else
#include <arpa/inet.h>
#endif

// Function prototypes
void* run_dns_server(void* arg);
int start_dns_server();

#endif // DNS_SERVER_H
