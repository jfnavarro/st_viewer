# Files that make up

add_definitions("-DBUILD_QXT_GUI -DQXT_STATIC")

set(LIB libqxt)

message(STATUS "Building ${LIB}")

set(SRC_libqxt
    ${PROJECT_SOURCE_DIR}/ext/${LIB}/qxtspanslider.cpp
    ${PROJECT_SOURCE_DIR}/ext/${LIB}/qxtspanslider_p.h
    ${PROJECT_SOURCE_DIR}/ext/${LIB}/qxtnamespace.h
    #${PROJECT_SOURCE_DIR}/ext/${LIB}/qxtspanslider.h
    #${PROJECT_SOURCE_DIR}/ext/${LIB}/qxtglobal.h
    ${PROJECT_SOURCE_DIR}/ext/${LIB}/qxtglobal.cpp
)

# Add library and include directory
add_library(${LIB} STATIC ${SRC_${LIB}})

# Organize lib into solution folder
project_group(${LIB} "ext")

# Clean-up
#unset(SRC_${LIB})
#unset(LIB)
