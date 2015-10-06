message(STATUS "Building qtwaitingspinner")

set(CMAKE_AUTOMOC ON)

set(SRC_libqtwaitingspinner
    ${PROJECT_SOURCE_DIR}/ext/QtWaitingSpinner/waitingspinnerwidget.cpp
    ${PROJECT_SOURCE_DIR}/ext/QtWaitingSpinner/waitingspinnerwidget.h
)

# Add library and include directory
add_library(qtwaitingspinner OBJECT ${SRC_libqtwaitingspinner})
include_directories(${PROJECT_SOURCE_DIR}/ext/QtWaitingSpinner)

# Organize lib into solution folder
project_group(qtwaitingspinner "ext")
