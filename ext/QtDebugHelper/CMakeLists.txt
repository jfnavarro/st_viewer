# Files that make up QtDebugHelper

set(LIB QtDebugHelper)

message(STATUS "Building ${LIB}")

set(SRC_${LIB}
    ${PROJECT_SOURCE_DIR}/ext/${LIB}/src/utils/DebugHelper.cpp
    #${PROJECT_SOURCE_DIR}/ext/${LIB}/src/utils/DebugHelper.h
)

# Add pre-precessor definitions
add_definitions(-DMAKE_GLUON_CORE_LIB)

# Add library and include directory
add_library(${LIB} STATIC ${SRC_${LIB}})
#include_directories(${PROJECT_SOURCE_DIR}/ext/${LIB}/src)

# Organize lib into solution folder
project_group(${LIB} "ext")

# Clean-up
#unset(SRC_${LIB})
#unset(LIB)
