cmake_minimum_required(VERSION 3.10)
project(http_server)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(include)

# Manually specify the path to libpcap
set(PCAP_INCLUDE_DIRS "/opt/homebrew/opt/libpcap/include")
set(PCAP_LIBRARIES "/opt/homebrew/opt/libpcap/lib/libpcap.dylib")

include_directories(${PCAP_INCLUDE_DIRS})

add_executable(http_server src/main.c src/server.c src/router.c src/tcp_server.c src/packet_sniffer.c)
target_link_libraries(http_server ${PCAP_LIBRARIES})

add_executable(dns_server src/dns_server.c)
