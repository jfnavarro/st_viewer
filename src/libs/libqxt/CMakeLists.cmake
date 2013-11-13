# Files that make up STCore

add_definitions("-DBUILD_QXT_GUI -DQXT_STATIC")

set(LIB libqxt)

message(STATUS "Building ${LIB}")

set(SRC_libqxt
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/qxtspanslider.cpp
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/qxtspanslider_p.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/qxtnamespace.h
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/qxtspanslider.h
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/qxtglobal.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/qxtglobal.cpp
)

# Add library and include directory
add_library(${LIB} STATIC ${SRC_${LIB}})
include_directories(${CMAKE_SOURCE_DIR}/src/libs/${LIB})

# Organize lib into solution folder
project_group(${LIB} "libs")

# Clean-up
unset(SRC_${LIB})
unset(LIB)
