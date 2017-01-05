cmake_minimum_required(VERSION 2.8.12)
project(stock)

list(APPEND CMAKE_CXX_FLAGS -std=c++11)

set(STOCK_SOURCE_FILES
    src/gl/Error.cpp
    src/gl/Framebuffer.cpp
    src/gl/Mesh.cpp
    src/gl/RenderState.cpp
    src/gl/ShaderProgram.cpp
    src/gl/ShaderUniform.cpp
    src/gl/Texture.cpp
    src/gl/VertexLayout.cpp
    src/io/File.cpp
    src/io/Log.cpp
    src/io/Url.cpp
    src/io/UrlSession.cpp
    src/view/Camera.cpp
    src/main.cpp
    )

add_executable(stock ${STOCK_SOURCE_FILES})

# Populate STOCK_INCLUDE_DIRS and STOCK_LIBRARIES.
include(deps/deps.cmake)

include_directories(src ${STOCK_INCLUDE_DIRS})

target_link_libraries(stock ${STOCK_LIBRARIES})
