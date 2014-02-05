## Files that make up QtSingleApplication

set(LIB QtSingleApplication)

message(STATUS "Building ${LIB}")

if(WIN32)
    set(PLATFORM win)
else()
    set(PLATFORM unix)
endif()

set(SRC_${LIB}
    ${PROJECT_SOURCE_DIR}/ext/${LIB}/src/qtlocalpeer.cpp
    #${PROJECT_SOURCE_DIR}/ext/${LIB}/src/qtlocalpeer.h
    ${PROJECT_SOURCE_DIR}/ext/${LIB}/src/qtlockedfile.cpp
    #${PROJECT_SOURCE_DIR}/ext/${LIB}/src/qtlockedfile.h
    ${PROJECT_SOURCE_DIR}/ext/${LIB}/src/qtlockedfile_${PLATFORM}.cpp
    ${PROJECT_SOURCE_DIR}/ext/${LIB}/src/qtsingleapplication.cpp
    #${PROJECT_SOURCE_DIR}/ext/${LIB}/src/qtsingleapplication.h
)

# Add library and include directory
add_library(${LIB} STATIC ${SRC_${LIB}})

# Organize lib into solution folder
project_group(${LIB} "ext")

# Clean-up
#unset(SRC_${LIB})
#unset(PLATFORM)
#unset(LIB)
