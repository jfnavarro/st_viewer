# Cmake module to find Rcpp
# - Try to find Rcpp
# Once done, this will define
#
#  RCPP_FOUND - system has RCPP
#  RCPP_INCLUDE_DIR - the RCPP include directories
#  RCPP_LIBRARY - link these to use RCPP
# Autor: Omar Andres Zapata Mesa 31/05/2013
# Adjusted by: Jose Fernandez Navaro

# Make sure find package macros are included
include(FindPackageHandleStandardArgs)

set(R_DIR
    "/usr/local"
    "/usr"
    "/usr/lib"
    "/usr/local/lib"
    "/usr/share"
    "/opt/R/site-library"
    "/usr/local/lib/R/site-library"
    "/usr/lib/R/site-library"
    "/Library/Frameworks/R.framework/Headers"
    "/Library/Frameworks/R.framework/Resources/library")

# Find the include dir
find_path(R_INCLUDE_DIR
  NAMES R.h libR.h
  HINTS ${R_PATH} ${R_DIR}
  PATH_SUFFIXES include R/include R
)

find_path(RCPP_INCLUDE_DIR
  NAMES Rcpp.h libRcpp.h
  HINTS ${R_PATH}
  PATHS ${R_DIR}
  PATH_SUFFIXES include Rcpp/include Rcpp
  NO_CMAKE_FIND_ROOT_PATH
)

find_library(RCPP_LIBRARY
    NAMES Rcpp libRcpp
    HINTS ${R_PATH}
    PATHS ${R_DIR}
    PATH_SUFFIXES libs lib lib32 lib64 Rcpp/libs Rcpp/lib Rcpp/lib32 Rcpp/lib64
)

find_library(R_LIBRARY
    NAMES R libR
    HINTS ${RCPP_PATH}
    PATHS ${R_DIR}
    PATH_SUFFIXES libs lib lib32 lib64 R/libs R/lib R/lib32 R/lib64
)

message(STATUS "RCPP paths ${R_DIR}")
message(STATUS "RCPP library ${RCPP_LIBRARY}")
message(STATUS "RCPP include ${RCPP_INCLUDE_DIR}")
message(STATUS "R include ${R_INCLUDE_DIR}")
message(STATUS "R library ${R_LIBRARY}")

if(RCPP_INCLUDE_DIR AND RCPP_LIBRARY)
    set(RCPP_FOUND TRUE)
    mark_as_advanced(RCPP_LIBRARY RCPP_LIBRARY)
else()
    set(RCPP_FOUND FALSE)
    message(FATAL_ERROR "Looking for Rcpp -- not found. Try to set RCPP_PATH to the path of Rcpp")
endif()
