# Files that make up QtDebugHelper

set(LIB QtDebugHelper)

message(STATUS "Building ${LIB}")

set(SRC_${LIB}
    ${CMAKE_SOURCE_DIR}/src/libs/QtDebugHelper/src/utils/DebugHelper.cpp
)

# Add pre-precessor definitions
add_definitions(-DMAKE_GLUON_CORE_LIB)

# Add library and include directory
add_library(${LIB} STATIC ${SRC_${LIB}})
include_directories(${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src)

# Organize lib into solution folder
project_group(${LIB} "libs")

# Clean-up
unset(SRC_${LIB})
unset(LIB)
