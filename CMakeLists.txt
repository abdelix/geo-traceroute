project(traceroute)
    cmake_minimum_required(VERSION 2.8)

add_executable(traceroute main.c)
include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_BINARY_DIR}/src)

include_directories(${PROJECT_SOURCE_DIR}/nxjson)
link_directories(${PROJECT_BINARY_DIR}/nxjson)

target_link_libraries(traceroute icmp_chksum geoloc_ip)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(nxjson)
