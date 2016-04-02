cmake_minimum_required(VERSION 3.0)
project(stock)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include(deps/deps.cmake)

include_directories(${STOCK_INCLUDES})

set(SOURCE_FILES src/main.cpp)
add_executable(stock ${SOURCE_FILES})

target_link_libraries(stock ${STOCK_LIBRARIES})
