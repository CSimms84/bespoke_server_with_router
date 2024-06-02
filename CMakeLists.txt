cmake_minimum_required(VERSION 3.10)
project(http_server)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(include)

# Manually specify the path to libpcap
set(PCAP_INCLUDE_DIRS "/opt/homebrew/opt/libpcap/include")
set(PCAP_LIBRARIES "/opt/homebrew/opt/libpcap/lib/libpcap.dylib")

include_directories(${PCAP_INCLUDE_DIRS})

add_executable(http_server src/main.c src/server.c src/router.c src/tcp_server.c src/packet_sniffer.c src/dns_server.c)
target_link_libraries(http_server ${PCAP_LIBRARIES})

# Manually specify the CUnit paths
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)


# Add test target
add_executable(run_tests tests/test_main.c src/router.c src/dns_server.c)
target_link_libraries(run_tests ${CUNIT_LIBRARY} ${PCAP_LIBRARIES})

set(CMAKE_BUILD_TYPE Debug)