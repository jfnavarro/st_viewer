project(STViewer CXX)
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
set(APPLICATION_NAME "Spatial Transcriptomics Research Viewer")

# Configuration variables
set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "6")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_DESCRIPTION "Visualization tool for Spatial Transcriptomics data")
set(PROJECT_COPYRIGHT "Copyright (C) Jose Fernandez Navarro, KTH, KI.")
set(PROJECT_CONTACT "jose.fernandez.navarro@scilifelab.se")
set(PROJECT_VENDOR "Jose Fernandez Navarro")

option(REMOTE_DATA "Whether the application can connect to a ST API to retrieve data" OFF)

# Check if the remote mode is desired
if(REMOTE_DATA)
  if (ENDPOINT STREQUAL "")
      message(FATAL_ERRROR "ENDPOINT is needed to enable the online mode")
  endif()
  if (CLIENTID STREQUAL "")
      message(FATAL_ERRROR "CLIENTID is needed to enable the online mode")
  endif()
  if (SECRECTID STREQUAL "")
      message(FATAL_ERRROR "SECRECTID is needed to enable the online mode")
  endif()
  if (PUBLICKEY STREQUAL "")
      message(FATAL_ERRROR "PUBLICKEY is needed to enable the online mode")
  endif()
endif()

set(APIPATH  "api")

# print main variables
message( STATUS )
message( STATUS
"-------------------------------------------------------------------------------"
)
message(STATUS "Building ${APPLICATION_NAME} ....." )
message(STATUS "Change a configuration variable with: cmake -D<Variable>=<Value>" )
message(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
message(STATUS "CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}" )
message(STATUS "TARGET_ARCH = ${TARGET_ARCH}" )
message(STATUS "VERSION = ${PROJECT_VERSION}")
message(STATUS "INTERNET SUPPORT = ${REMOTE_DATA}")
if (REMOTE_DATA)
    message(STATUS "ENDPOINT = ${ENDPOINT}")
    message(STATUS "CLIENT ID = ${CLIENTID}")
    message(STATUS "SSL KEY = ${PUBLICKEY}")
endif()
message(STATUS
"-------------------------------------------------------------------------------"
)
message( STATUS )

enable_testing()

# Restrict possible builds to the selected build type.
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  if(WIN32)
    cmake_policy(SET CMP0020 NEW)
  endif(WIN32)
  if(APPLE)
    cmake_policy(SET CMP0025 NEW)
  endif()
endif(COMMAND cmake_policy)

# File that contains the default language for Qt
set(TRANSLATION_FILE "locale_en_us.qm")

# Compile RC-file, only for Windows
if(WIN32)
  set (RC_FILE ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.rc)
  add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.o
    COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.rc
    -o ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.o )
  # Failed to find "glu32"
  # The suggested solution in 
  # http://www.qtcentre.org/threads/56185-cmake-error-with-Failed-to-find-quot-glu32-quot-in-quot-quot?p=251234
  # solves the issue. Maybe it's a Qt bug?
  set(CMAKE_LIBRARY_PATH "C:\\Program Files (x86)\\Windows Kits\\8.0\\Lib\\win8\\um\\x64")
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG)
endif (WIN32)

# Some in-house CMake functions/macros
include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)

# Compile CMake generated based files
configure_file(${PROJECT_SOURCE_DIR}/assets/config/application.conf.in ${PROJECT_BINARY_DIR}/application.conf )
configure_file(${PROJECT_SOURCE_DIR}/assets/application.qrc.in ${PROJECT_BINARY_DIR}/application.qrc )
configure_file(${PROJECT_SOURCE_DIR}/cmake/options_cmake.h.in ${PROJECT_BINARY_DIR}/options_cmake.h )

# Include the project binary dir
include_directories(${PROJECT_BINARY_DIR})

# Find the path to Qt binaries and set the Qt libraries
find_package(Qt5LinguistTools REQUIRED CONFIG)
set(ST_QT_LIBS  Widgets PrintSupport Network Core Gui OpenGL Concurrent)
if(UNIX AND NOT APPLE)
  list(APPEND ST_QT_LIBS DBus Svg)
endif()
foreach(i ${ST_QT_LIBS})
  use_qt5lib("Qt5${i}")
endforeach()

# Add cutom find.cmake files
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Initialize project settings (important to do it first as it defines compilation flags)
initialise_project()

add_subdirectory(ext) # include third party libraries
add_subdirectory(src) # include source code
