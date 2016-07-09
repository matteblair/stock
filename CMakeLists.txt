cmake_minimum_required(VERSION 3.0)
project(stock)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

include(deps/deps.cmake)

include_directories(src ${STOCK_INCLUDE_DIRS})

set(SOURCE_FILES
        src/main.cpp
        src/gl/VertexLayout.cpp
        src/gl/ShaderProgram.cpp
        src/gl/ShaderProgram.hpp
        src/gl/RenderState.cpp
        src/gl/ShaderUniform.cpp
        src/gl/Error.cpp
        src/io/Log.cpp
        src/gl/Mesh.cpp
        )

add_executable(stock ${SOURCE_FILES})

target_link_libraries(stock ${STOCK_LIBRARIES})
