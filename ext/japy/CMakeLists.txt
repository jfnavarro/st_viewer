message(STATUS "Building Japy")

set(SRC_libjapy
    ${PROJECT_SOURCE_DIR}/ext/japy/japy.hpp
)

# Add library and include directory
add_library(japy OBJECT ${SRC_libjapy})
include_directories(${PROJECT_SOURCE_DIR}/ext/japy)

# Organize lib into solution folder
project_group(japy "ext")
