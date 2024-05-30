#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "dns_server.h"

#define DNS_PORT 53
#define BUFFER_SIZE 512

struct DNSHeader {
    unsigned short id;
    unsigned char rd :1;
    unsigned char tc :1;
    unsigned char aa :1;
    unsigned char opcode :4;
    unsigned char qr :1;
    unsigned char rcode :4;
    unsigned char cd :1;
    unsigned char ad :1;
    unsigned char z :1;
    unsigned char ra :1;
    unsigned short q_count;
    unsigned short ans_count;
    unsigned short auth_count;
    unsigned short add_count;
};

struct Question {
    unsigned short qtype;
    unsigned short qclass;
};

struct ResourceRecord {
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};

void handle_query(int sock, struct sockaddr_in *client, unsigned char *buffer, int length) {
    struct DNSHeader *dns = (struct DNSHeader *) buffer;
    dns->qr = 1; // Response
    dns->opcode = 0; // Standard query
    dns->aa = 1; // Authoritative answer
    dns->rcode = 0; // No error

    unsigned char *qname = (unsigned char *) (buffer + sizeof(struct DNSHeader));
    unsigned char *reader = qname;

    while (*reader != 0) {
        reader++;
    }

    struct Question *qinfo = (struct Question *) (reader + 1);

    unsigned char response[BUFFER_SIZE];
    memset(response, 0, BUFFER_SIZE);

    struct DNSHeader *dns_res = (struct DNSHeader *) response;
    memcpy(dns_res, dns, sizeof(struct DNSHeader));

    dns_res->q_count = htons(1);
    dns_res->ans_count = htons(1);

    unsigned char *response_name = response + sizeof(struct DNSHeader);
    memcpy(response_name, qname, reader - qname + 1);

    struct ResourceRecord *rr = (struct ResourceRecord *) (response_name + (reader - qname + 1) + sizeof(struct Question));
    rr->type = htons(1); // A record
    rr->_class = htons(1); // IN class
    rr->ttl = htonl(3600); // TTL
    rr->data_len = htons(4); // IPv4 address length

    unsigned char *rdata = (unsigned char *) (rr + 1);
    rdata[0] = 127; // Response IP address (example: 127.0.0.1)
    rdata[1] = 0;
    rdata[2] = 0;
    rdata[3] = 1;

    sendto(sock, response, sizeof(struct DNSHeader) + (reader - qname + 1) + sizeof(struct Question) + sizeof(struct ResourceRecord) + rr->data_len, 0, (struct sockaddr *) client, sizeof(struct sockaddr_in));
}

int start_dns_server() {
    int sock;
    struct sockaddr_in server, client;
    unsigned char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DNS_PORT);

    if (bind(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("Bind failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        socklen_t len = sizeof(client);
        int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &client, &len);
        if (n < 0) {
            perror("Receive failed");
            continue;
        }
        handle_query(sock, &client, buffer, n);
    }

    close(sock);
    return 0;
}

void* run_dns_server(void* arg) {
    start_dns_server();
    return NULL;
}
