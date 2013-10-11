# Files that make up STCoreWidgets

set(LIB STCoreWidgets)

message(STATUS "Building ${LIB}")

set(SRC_${LIB}
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/Core.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/CorePrivate.cpp
#     ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/CorePrivate.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/LineEditButton.cpp
#     ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/LineEditButton.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/LineEditButtonPrivate.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/LineEditClear.cpp
#     ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/LineEditClear.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/LineEditClearPrivate.h
)

# Add library and include directory
add_library(${LIB} STATIC ${SRC_${LIB}})
include_directories(${CMAKE_SOURCE_DIR}/src/libs/${LIB})

# Organize lib into solution folder
project_group(${LIB} "libs")

# Clean-up
unset(SRC_${LIB})
unset(LIB)