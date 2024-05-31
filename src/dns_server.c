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
    unsigned short id; // identification number
    unsigned char rd :1; // recursion desired
    unsigned char tc :1; // truncated message
    unsigned char aa :1; // authoritative answer
    unsigned char opcode :4; // purpose of message
    unsigned char qr :1; // query/response flag

    unsigned char rcode :4; // response code
    unsigned char cd :1; // checking disabled
    unsigned char ad :1; // authenticated data
    unsigned char z :1; // its z! reserved
    unsigned char ra :1; // recursion available

    unsigned short q_count; // number of question entries
    unsigned short ans_count; // number of answer entries
    unsigned short auth_count; // number of authority entries
    unsigned short add_count; // number of resource entries
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
    unsigned char *qname = buffer + sizeof(struct DNSHeader);
    struct Question *qinfo = (struct Question *) (qname + strlen((const char*)qname) + 1);

    unsigned char response[BUFFER_SIZE];
    memset(response, 0, BUFFER_SIZE);

    struct DNSHeader *dns_res = (struct DNSHeader *) response;
    memcpy(dns_res, dns, sizeof(struct DNSHeader));

    dns_res->qr = 1; // Response
    dns_res->opcode = 0; // Standard query
    dns_res->aa = 1; // Authoritative answer
    dns_res->rcode = 0; // No error
    dns_res->q_count = htons(1);
    dns_res->ans_count = htons(1);
    dns_res->auth_count = 0;
    dns_res->add_count = 0;

    unsigned char *response_name = response + sizeof(struct DNSHeader);
    strcpy((char*)response_name, (char*)qname);

    struct Question *qinfo_res = (struct Question *) (response_name + strlen((const char*)response_name) + 1);
    qinfo_res->qtype = qinfo->qtype;
    qinfo_res->qclass = qinfo->qclass;

    struct ResourceRecord *rr = (struct ResourceRecord *) (response_name + strlen((const char*)response_name) + 1 + sizeof(struct Question));
    rr->type = htons(1); // A record
    rr->_class = htons(1); // IN class
    rr->ttl = htonl(3600); // TTL
    rr->data_len = htons(4); // IPv4 address length

    unsigned char *rdata = (unsigned char *) (rr + 1);
    rdata[0] = 127; // Response IP address (example: 127.0.0.1)
    rdata[1] = 0;
    rdata[2] = 0;
    rdata[3] = 1;

    int response_size = sizeof(struct DNSHeader) + (strlen((const char*)response_name) + 1) + sizeof(struct Question) + sizeof(struct ResourceRecord) + 4;
    sendto(sock, response, response_size, 0, (struct sockaddr *) client, sizeof(struct sockaddr_in));
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

    printf("DNS server started on port %d\n", DNS_PORT);

    while (1) {
        socklen_t len = sizeof(client);
        int n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &client, &len);
        if (n < 0) {
            perror("Receive failed");
            continue;
        }
        printf("Received DNS query\n");
        handle_query(sock, &client, buffer, n);
    }

    close(sock);
    return 0;
}

void* run_dns_server(void* arg) {
    start_dns_server();
    return NULL;
}
