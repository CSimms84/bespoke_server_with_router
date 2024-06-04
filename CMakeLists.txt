cmake_minimum_required(VERSION 3.10)
project(http_server)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(include)

# Determine the operating system and set the appropriate paths
if(APPLE)
    set(PCAP_INCLUDE_DIRS "/opt/homebrew/opt/libpcap/include")
    set(PCAP_LIBRARIES "/opt/homebrew/opt/libpcap/lib/libpcap.dylib")
    set(CUNIT_INCLUDE_DIRS "/opt/homebrew/include")
    set(CUNIT_LIBRARIES "/opt/homebrew/lib/libcunit.dylib")
elseif(UNIX)
    find_path(PCAP_INCLUDE_DIRS pcap.h)
    find_library(PCAP_LIBRARIES pcap)
    find_path(CUNIT_INCLUDE_DIRS CUnit/CUnit.h)
    find_library(CUNIT_LIBRARIES cunit)
elseif(WIN32)
    find_path(PCAP_INCLUDE_DIRS pcap.h PATHS $ENV{WINDIR}/System32)
    find_library(PCAP_LIBRARIES NAMES wpcap PATHS $ENV{WINDIR}/System32)
    find_path(CUNIT_INCLUDE_DIRS CUnit/CUnit.h PATHS $ENV{WINDIR}/System32)
    find_library(CUNIT_LIBRARIES cunit)
endif()

# Include directories
include_directories(${PCAP_INCLUDE_DIRS})
include_directories(${CUNIT_INCLUDE_DIRS})

# Add the executable for the main server
add_executable(http_server src/main.c src/server.c src/router.c src/tcp_server.c src/packet_sniffer.c src/dns_server.c)
target_link_libraries(http_server ${PCAP_LIBRARIES})

# Add test target
add_executable(run_tests tests/test_main.c src/router.c src/dns_server.c)
target_link_libraries(run_tests ${CUNIT_LIBRARIES} ${PCAP_LIBRARIES})

set(CMAKE_BUILD_TYPE Debug)
