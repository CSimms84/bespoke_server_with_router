#include <ctype.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#include <pcap.h>
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <pcap.h>
#endif

#include "../include/packet_sniffer.h"

#define SNAP_LEN 1518
#define SIZE_ETHERNET 14

struct sniff_ip {
    u_char ip_vhl;                 /* version << 4 | header length >> 2 */
    u_char ip_tos;                 /* type of service */
    u_short ip_len;                /* total length */
    u_short ip_id;                 /* identification */
    u_short ip_off;                /* fragment offset field */
    u_char ip_ttl;                 /* time to live */
    u_char ip_p;                   /* protocol */
    u_short ip_sum;                /* checksum */
    struct in_addr ip_src, ip_dst; /* source and dest address */
};

void print_hex_ascii_line(const u_char *payload, int len, int offset) {
    int i;
    int gap;
    const u_char *ch;

    /* offset */
    printf("%05d   ", offset);
    
    /* hex */
    ch = payload;
    for(i = 0; i < len; i++) {
        printf("%02x ", *ch);
        ch++;
        if (i == 7)
            printf(" ");
    }
    
    /* fill hex gap with spaces if not full line */
    if (len < 8)
        printf(" ");
    if (len < 16) {
        gap = 16 - len;
        for (i = 0; i < gap; i++) {
            printf("   ");
        }
    }
    printf("   ");
    
    /* ascii (if printable) */
    ch = payload;
    for(i = 0; i < len; i++) {
        if (isprint(*ch))
            printf("%c", *ch);
        else
            printf(".");
        ch++;
    }
    
    printf("\n");
}

void print_payload(const u_char *payload, int len) {
    int len_rem = len;
    int line_width = 16; /* number of bytes per line */
    int line_len;
    int offset = 0; /* zero-based offset counter */
    const u_char *ch = payload;

    if (len <= 0)
        return;

    if (len <= line_width) {
        print_hex_ascii_line(ch, len, offset);
        return;
    }

    for ( ;; ) {
        line_len = line_width % len_rem;
        print_hex_ascii_line(ch, line_len, offset);
        len_rem = len_rem - line_len;
        ch = ch + line_len;
        offset = offset + line_width;
        if (len_rem <= line_width) {
            print_hex_ascii_line(ch, len_rem, offset);
            break;
        }
    }
}

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    const struct sniff_ip *ip;
    int size_ip;

    ip = (struct sniff_ip *)(packet + SIZE_ETHERNET);
    size_ip = (ip->ip_vhl & 0x0f) * 4;

    if (size_ip < 20) {
        printf("Invalid IP header length: %u bytes\n", size_ip);
        return;
    }

    printf("From: %s\n", inet_ntoa(ip->ip_src));
    printf("To: %s\n", inet_ntoa(ip->ip_dst));
    printf("Protocol: %d\n", ip->ip_p);
    
    /* print packet content */
    printf("Payload:\n");
    print_payload(packet, header->caplen);
}

void start_packet_sniffer() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs, *d;
    pcap_t *handle;

    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
            fprintf(stderr, "WSAStartup failed\n");
            return;
        }
    #endif

    // Find all available devices
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
        return;
    }

    // Use the first available device
    d = alldevs;
    if (d == NULL) {
        fprintf(stderr, "No devices found\n");
        return;
    }

    printf("Using device: %s\n", d->name);

    handle = pcap_open_live(d->name, SNAP_LEN, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", d->name, errbuf);
        pcap_freealldevs(alldevs);
        return;
    }

    if (pcap_loop(handle, 10, got_packet, NULL) < 0) {
        fprintf(stderr, "pcap_loop() failed: %s\n", pcap_geterr(handle));
        pcap_close(handle);
        pcap_freealldevs(alldevs);
        return;
    }

    pcap_close(handle);
    pcap_freealldevs(alldevs);

    #ifdef _WIN32
        WSACleanup();
    #endif
}
