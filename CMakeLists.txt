cmake_minimum_required(VERSION 3.0)
project(stock)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

include(deps/deps.cmake)

include_directories(src ${STOCK_INCLUDE_DIRS})

set(SOURCE_FILES
        src/gl/Error.cpp
        src/gl/Mesh.cpp
        src/gl/RenderState.cpp
        src/gl/ShaderProgram.cpp
        src/gl/ShaderUniform.cpp
        src/gl/Texture.cpp
        src/gl/VertexLayout.cpp
        src/io/File.cpp
        src/io/Log.cpp
        src/view/Camera.cpp
        src/main.cpp
        )

add_executable(stock ${SOURCE_FILES})

target_link_libraries(stock ${STOCK_LIBRARIES})
