cmake_minimum_required(VERSION 3.17)
project(zmqtest)

set(CMAKE_CXX_STANDARD 17)

set(POCO_PREFIX "/usr/local") # the directory containing "include" and "lib"
set(POCO_INCLUDE_DIR"${POCO_PREFIX}/include")
set(POCO_LIB_DIR "${POCO_PREFIX}/lib")
set(POCO_LIBS
        "${POCO_LIB_DIR}/libPocoNet.dylib"
        "${POCO_LIB_DIR}/libPocoUtil.dylib"
        "${POCO_LIB_DIR}/libPocoJSON.dylib"
        "${POCO_LIB_DIR}/libPocoFoundation.dylib")

include_directories(/usr/local/include)

add_executable(zmqtest main.cpp)

target_link_libraries(zmqtest "${POCO_LIBS}")