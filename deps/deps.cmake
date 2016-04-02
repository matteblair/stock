set(STOCK_DEPS_DIR deps)
set(STOCK_INCLUDES "")
set(STOCK_LIBRARIES "")

## GLFW ##

# configure GLFW to build only the library
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")

add_subdirectory(${STOCK_DEPS_DIR}/glfw)
target_compile_options(glfw PRIVATE -O3)
target_compile_definitions(glfw PRIVATE NDEBUG)
set(STOCK_INCLUDES "${STOCK_INCLUDES} ${STOCK_DEPS_DIR}/glfw/include")
list(APPEND STOCK_LIBRARIES glfw)
list(APPEND STOCK_LIBRARIES ${GLFW_LIBRARIES})

## OpenGL ##

find_package(OpenGL REQUIRED)
list(APPEND STOCK_LIBRARIES ${OPENGL_LIBRARIES})
