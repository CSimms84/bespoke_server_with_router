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
    message(STATUS "PCAP_INCLUDE_DIRS: ${PCAP_INCLUDE_DIRS}")
    find_library(PCAP_LIBRARIES pcap)
    message(STATUS "PCAP_LIBRARIES: ${PCAP_LIBRARIES}")
    find_path(CUNIT_INCLUDE_DIRS CUnit/CUnit.h)
    message(STATUS "CUNIT_INCLUDE_DIRS: ${CUNIT_INCLUDE_DIRS}")
    find_library(CUNIT_LIBRARIES cunit)
    message(STATUS "CUNIT_LIBRARIES: ${CUNIT_LIBRARIES}")
elseif(WIN32)
    find_path(PCAP_INCLUDE_DIRS pcap.h PATHS $ENV{WINDIR}/System32)
    message(STATUS "PCAP_INCLUDE_DIRS: ${PCAP_INCLUDE_DIRS}")
    find_library(PCAP_LIBRARIES NAMES wpcap PATHS $ENV{WINDIR}/System32)
    message(STATUS "PCAP_LIBRARIES: ${PCAP_LIBRARIES}")
    find_path(CUNIT_INCLUDE_DIRS CUnit/CUnit.h PATHS $ENV{WINDIR}/System32)
    message(STATUS "CUNIT_INCLUDE_DIRS: ${CUNIT_INCLUDE_DIRS}")
    find_library(CUNIT_LIBRARIES cunit)
    message(STATUS "CUNIT_LIBRARIES: ${CUNIT_LIBRARIES}")
endif()

# Check if the required libraries and include directories are found
if(NOT PCAP_INCLUDE_DIRS)
    message(FATAL_ERROR "PCAP include directory not found")
endif()
if(NOT PCAP_LIBRARIES)
    message(FATAL_ERROR "PCAP library not found")
endif()
if(NOT CUNIT_INCLUDE_DIRS)
    message(FATAL_ERROR "CUnit include directory not found")
endif()
if(NOT CUNIT_LIBRARIES)
    message(FATAL_ERROR "CUnit library not found")
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
