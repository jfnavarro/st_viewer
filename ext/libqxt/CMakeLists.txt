add_definitions("-DBUILD_QXT_GUI -DQXT_STATIC")

message(STATUS "Building libqxt")

set(CMAKE_AUTOMOC ON)

set(SRC_libqxt
    ${PROJECT_SOURCE_DIR}/ext/libqxt/qxtspanslider.cpp
    ${PROJECT_SOURCE_DIR}/ext/libqxt/qxtspanslider_p.h
    ${PROJECT_SOURCE_DIR}/ext/libqxt/qxtnamespace.h
    ${PROJECT_SOURCE_DIR}/ext/libqxt/qxtspanslider.h
    ${PROJECT_SOURCE_DIR}/ext/libqxt/qxtglobal.h
    ${PROJECT_SOURCE_DIR}/ext/libqxt/qxtglobal.cpp
)

# Add library and include directory
add_library(qxt OBJECT ${SRC_libqxt})
include_directories(${PROJECT_SOURCE_DIR}/ext/libqxt)

# Organize lib into solution folder
project_group(qxt "ext")
