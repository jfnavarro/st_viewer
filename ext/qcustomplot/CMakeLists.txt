message(STATUS "Building qcustomplot")

set(CMAKE_AUTOMOC ON)

set(SRC_libqcustomplot
    ${PROJECT_SOURCE_DIR}/ext/qcustomplot/qcustomplot.cpp
    ${PROJECT_SOURCE_DIR}/ext/qcustomplot/qcustomplot.h
)

# Add library and include directory
add_library(qcustomplot OBJECT ${SRC_libqcustomplot})
include_directories(${PROJECT_SOURCE_DIR}/ext/libqcustomplot)

# Organize lib into solution folder
project_group(qcustomplot "ext")
