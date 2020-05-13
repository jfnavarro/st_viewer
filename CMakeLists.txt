project(STViewer CXX)

cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR)

set(APPLICATION_NAME "Spatial Transcriptomics Research Viewer")

# Configuration variables
set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "8")
set(PROJECT_VERSION_PATCH "3")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_DESCRIPTION "Visualization tool for Spatial Transcriptomics data")
set(PROJECT_COPYRIGHT "Copyright (C) 2020 Jose Fernandez Navarro")
set(PROJECT_CONTACT "jc.fernandez.navarro@gmail.com")
set(PROJECT_VENDOR "Jose Fernandez Navarro")
set(CONFIG_FILE "${PROJECT_SOURCE_DIR}/assets/stviewer.conf" CACHE STRING
    "The file with the configuration settings")

# print main variables
message(STATUS)
message(STATUS
"-------------------------------------------------------------------------------"
)
message(STATUS "Building ${APPLICATION_NAME} .....")
message(STATUS "Change a configuration variable with: cmake -D<Variable>=<Value>")
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
message(STATUS "TARGET_ARCH = ${TARGET_ARCH}")
message(STATUS "VERSION = ${PROJECT_VERSION}")
message(STATUS "CONFIGURATION FILE = ${CONFIG_FILE}")
message(STATUS
"-------------------------------------------------------------------------------"
)
message(STATUS)

if(WIN32)
    SET(CMAKE_FIND_LIBRARY_PREFIXES "")
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
endif()

# Restrict possible builds to the selected build type.
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")

# We just need to pass the app the name of the config file so it can be used
get_filename_component(CONFIG_FILE_NAME ${CONFIG_FILE} NAME)

# Some in-house CMake functions/macros
include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)

# Initialize project settings (important to do it first as it defines compilation flags)
initialise_project()

# Compile RC-file, only for Windows
if(WIN32)
  set (RC_FILE ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.rc)
  add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.o
    COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.rc
    -o ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.o)
endif (WIN32)

# Check the the configuration file is present
if(NOT EXISTS ${CONFIG_FILE})
    message(FATAL_ERROR "Configuration file not present!")
endif()

# Compile CMake generated based files
configure_file(${PROJECT_SOURCE_DIR}/assets/application.qrc.in ${PROJECT_BINARY_DIR}/application.qrc)
configure_file(${PROJECT_SOURCE_DIR}/cmake/options_cmake.h.in ${PROJECT_BINARY_DIR}/options_cmake.h)

# Include the project binary dir
include_directories(${PROJECT_BINARY_DIR})

# Add cutom find.cmake files
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Include local configuration file if present
if(EXISTS "${CMAKE_SOURCE_DIR}/local_configuration.cmake")
  include("${CMAKE_SOURCE_DIR}/local_configuration.cmake")
endif()

# Include source code
add_subdirectory(src)
