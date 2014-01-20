## Files that make up QtSingleApplication

set(LIB QtSingleApplication)

message(STATUS "Building ${LIB}")

if(WIN32)
    set(PLATFORM win)
else()
    set(PLATFORM unix)
endif()

set(SRC_${LIB}
    ${CMAKE_SOURCE_DIR}/src/libs/QtSingleApplication/src/qtlocalpeer.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/QtSingleApplication/src/qtlocalpeer.h
    ${CMAKE_SOURCE_DIR}/src/libs/QtSingleApplication/src/qtlockedfile.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/QtSingleApplication/src/qtlockedfile.h
    ${CMAKE_SOURCE_DIR}/src/libs/QtSingleApplication/src/qtlockedfile_${PLATFORM}.cpp
    ${CMAKE_SOURCE_DIR}/src/libs/QtSingleApplication/src/qtsingleapplication.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/QtSingleApplication/src/qtsingleapplication.h
)

# Add library and include directory
add_library(${LIB} STATIC ${SRC_${LIB}})
include_directories(${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src)

# Organize lib into solution folder
project_group(${LIB} "libs")

# Clean-up
unset(SRC_${LIB})
unset(PLATFORM)
unset(LIB)
