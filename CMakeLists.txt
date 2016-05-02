cmake_minimum_required(VERSION 3.0)
project(stock)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

include(deps/deps.cmake)

include_directories(src ${STOCK_INCLUDE_DIRS})

set(SOURCE_FILES src/main.cpp src/gl/VertexLayout.cpp src/gl/VertexLayout.hpp src/gl/GL.hpp src/gl/ShaderProgram.cpp src/gl/ShaderProgram.hpp src/gl/RenderState.cpp src/gl/RenderState.hpp src/gl/ShaderUniform.hpp src/gl/ShaderUniform.cpp src/gl/Error.hpp src/gl/Error.cpp src/io/Log.hpp src/io/Log.cpp)
add_executable(stock ${SOURCE_FILES})

target_link_libraries(stock ${STOCK_LIBRARIES})
