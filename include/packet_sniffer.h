#ifndef PACKET_SNIFFER_H
#define PACKET_SNIFFER_H

#ifdef _WIN32
// Include Windows headers
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib
#else
#include <arpa/inet.h>
#endif

// Function prototype
void start_packet_sniffer();

#endif // PACKET_SNIFFER_H
