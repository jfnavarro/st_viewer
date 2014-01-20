###############################################################################
# NAME AND VERSION
###############################################################################
project(stVi C CXX)
cmake_minimum_required(VERSION 2.8.9)

set(APPLICATION_NAME "Spatial Transcriptomics Viewer Tool")

#Configuration stuff
set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "3")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_VENDOR "Spatial Transcriptomics")
set(PROJECT_COPYRIGHT_YEAR "2013")
set(PROJECT_DOMAIN_FIRST "spatialtranscriptomics")
set(PROJECT_DOMAIN_SECOND "com")
set(PROJECT_DOMAIN "www.${PROJECT_DOMAIN_FIRST}.${PROJECT_DOMAIN_SECOND}")

#Production configuration
set(ENDPOINT "https://api.spatialtranscriptomics.com")
set(APIPATH  "api-0.3")
set(CLIENTID "st-viewer-client")
set(SECRETID "ayfBaF5UAKR2")
set(PUBLICKEY "${PROJECT_SOURCE_DIR}/assets/dev_server_cert.pem")

#QA configuration
#set(ENDPOINT "https://api-qa.spatialtranscriptomics.com")
#set(APIPATH  "api-0.3")
#set(CLIENTID "st-viewer-client")
#set(SECRETID "secret")
#set(PUBLICKEY "${PROJECT_SOURCE_DIR}/assets/dev_server_cert.pem")

#DEV configuration
# set(ENDPOINT "https://esox.scilifelab.se")
# set(APIPATH  "api-0.3")
# set(CLIENTID "st-viewer-client")
# set(SECRETID "secret")
# set(PUBLICKEY "${PROJECT_SOURCE_DIR}/assets/dev_server_cert.pem")

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  if(WIN32)
    cmake_policy(SET CMP0020 NEW)
  endif(WIN32)
endif(COMMAND cmake_policy)

#set the translations directory
#this is ugly but it only to generate binaries that are able
#to load the translations in Debug and Release mode we need to uncomment the /src/ for /../share/${PROJECT_NAME}/translations/
#otherwise we can just leave the /src/
#this we will fixed when I find time
if(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
    set(TRANSLATIONS_DIR "/src/")
    #set(TRANSLATIONS_DIR "/../share/${PROJECT_NAME}/translations/")
elseif(UNIX AND NOT APPLE)
    set(TRANSLATIONS_DIR "/src/")
    #set(TRANSLATIONS_DIR "/../share/${PROJECT_NAME}/translations/")
elseif(APPLE)
    set(TRANSLATIONS_DIR UNUSED)
elseif(WIN32)
    set(TRANSLATIONS_DIR UNUSED)
endif(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])

#name of locale file
set(TRANSLATION_FILE "locale_en_us.qm")   

# Compile RC-file, only for Windows
if(WIN32)
  set (RC_FILE ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.rc)
  add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.o
    COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.rc
    -o ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.o )
endif (WIN32)

# Some in-house CMake functions/macros
include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)

configure_file(${PROJECT_SOURCE_DIR}/assets/config/application.conf.in ${PROJECT_BINARY_DIR}/application.conf )
configure_file(${PROJECT_SOURCE_DIR}/assets/application.qrc.in ${PROJECT_BINARY_DIR}/application.qrc )
configure_file(${PROJECT_SOURCE_DIR}/cmake/options_cmake.h.in ${PROJECT_BINARY_DIR}/options_cmake.h )

include_directories(${PROJECT_BINARY_DIR})

add_subdirectory(src)
