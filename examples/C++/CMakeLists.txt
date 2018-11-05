cmake_minimum_required(VERSION 3.0.0)
project(amq VERSION 0.1.0)

set(CMAKE_C_COMPILER "clang")#gcc
set(CMAKE_CXX_COMPILER "clang++")#g++
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# includes
include(CPack)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
/usr/local/include/activemq-cpp-3.10.0/
/usr/local/apr/include/apr-1/
)

add_executable(amq main.cpp
w_thread.cpp
amq_producer.cpp
amq_consumer.cpp)

# compiler options
target_compile_options(amq PRIVATE -fPIC -m64)

# libs
target_link_libraries(amq /usr/local/lib/libactivemq-cpp.so pthread)
