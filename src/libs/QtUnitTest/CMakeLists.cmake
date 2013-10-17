# Files that make up QtUnitTest

set(LIB QtUnitTest)

message(STATUS "Building ${LIB}")

set(SRC_${LIB}
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/algorithm/breadthfirsttopdown.cpp
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/algorithm/linearizer.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/asciigenerator.cpp
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/binarygenerator.cpp
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/sourcegenerator.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/util/random.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/testsuite.cpp
)

# Add library and include directory
add_library(${LIB} STATIC ${SRC_${LIB}})
include_directories(${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src)

# Organize lib into solution folder
project_group(${LIB} "libs")

# Clean-up
unset(SRC_${LIB})
unset(LIB)