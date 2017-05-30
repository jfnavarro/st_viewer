# Cmake module to find Rcpp
# - Try to find Rcpp
# Once done, this will define
#
#  RCPP_FOUND - system has RCPP
#  RCPP_INCLUDE_DIR - the RCPP include directories
#  RCPP_LIBRARY - link these to use RCPP
# Autor: Omar Andres Zapata Mesa 31/05/2013
# Adjusted by: Jose Fernandez Navaro

set(R_DIR
    "/usr/local"
    "/usr"
    "/usr/lib"
    "/usr/local/lib"
    "/usr/share"
    "/opt/R/site-library"
    "/usr/local/lib/R/site-library"
    "/usr/lib/R/site-library"
    "/Library/Frameworks/R.framework/Resources"
    "/Library/Frameworks/R.framework/Resources/library")

# Find the include dir and the library
find_path(RCPP_INCLUDE_DIR
  NAMES Rcpp.h
  HINTS ${R_PATH} ${R_DIR}
  PATH_SUFFIXES include Rcpp/include Rcpp
)
#TODO FIX this
set(RCPP_INCLUDE_DIR "/Library/Frameworks/R.framework/Resources/library/Rcpp/include")

if(RCPP_INCLUDE_DIR)
    set(RCPP_FOUND TRUE)
else()
    set(RCPP_FOUND FALSE)
    message(FATAL_ERROR "Looking for Rcpp -- not found.")
endif()
