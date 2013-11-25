# Files that make up STCore

set(LIB STCore)

message(STATUS "Building ${LIB}")

set(SRC_${LIB}
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/data/SimpleCrypt.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/data/SimpleCrypt.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/data/SimpleCryptDevice.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/data/SimpleCryptDevice.h
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/view/AnimatedDatasetsTableView.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/view/AnimatedDatasetsTableView.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/view/GenesTableView.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/view/GenesTableView.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/view/GeneSelectionTableView.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/view/GeneSelectionTableView.cpp
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/view/WidgetBackgroundAnimation.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/view/WidgetBackgroundAnimation.h
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/view/CellViewPageToolBar.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/core/view/CellViewPageToolBar.cpp
)

# Add library and include directory
add_library(${LIB} STATIC ${SRC_${LIB}})
include_directories(${CMAKE_SOURCE_DIR}/src/libs/${LIB})

# Organize lib into solution folder
project_group(${LIB} "libs")

# Clean-up
unset(SRC_${LIB})
unset(LIB)
