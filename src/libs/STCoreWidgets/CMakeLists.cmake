# Files that make up STCoreWidgets

set(LIB STCoreWidgets)

message(STATUS "Building ${LIB}")

set(SRC_${LIB}
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/ExtendedTabWidget.cpp
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/MainMenuBar.cpp
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/MainStatusBar.cpp
)
# Add library and include directory
add_library(${LIB} STATIC ${SRC_${LIB}})
include_directories(${CMAKE_SOURCE_DIR}/src/libs/${LIB})

# Organize lib into solution folder
project_group(${LIB} "libs")

# Clean-up
unset(SRC_${LIB})
unset(LIB)
