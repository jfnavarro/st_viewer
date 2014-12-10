message(STATUS "Building qjpeg-turbo")

find_package(JpegTurbo REQUIRED QUIET)
include_directories(${LIBJPEG_INCLUDE_DIRS})
#ugly trick to pass the scope of the libjpeg library to root
set(LIBJPEG_LIBRARIES ${LIBJPEG_LIBRARIES} PARENT_SCOPE)

set(SRC_libqjpeg-turbo
    ${PROJECT_SOURCE_DIR}/ext/qjpeg-turbo/qjpeghandler.cpp
    ${PROJECT_SOURCE_DIR}/ext/qjpeg-turbo/qjpeghandler_p.h
)

# Add library and include directory
add_library(qjpeg-turbo OBJECT ${SRC_libqjpeg-turbo})
include_directories(${PROJECT_SOURCE_DIR}/ext/qjpeg-turbo)

# Organize lib into solution folder
project_group(qjpeg-turbo "ext")
