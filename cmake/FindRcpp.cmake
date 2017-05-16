# Cmake module to find Rcpp
# - Try to find Rcpp
# Once done, this will define
#
#  RCPP_FOUND - system has RCPP
#  RCPP_INCLUDE_DIR - the RCPP include directories
#  RCPP_LIBRARY - link these to use RCPP
# Autor: Omar Andres Zapata Mesa 31/05/2013
# Adjusted by: Jose Fernandez Navaro

set(RCPP_PKGCONF_LIBRARY_INCLUDE
    "/usr/local/include"
    "/usr/include"
    "/opt/R/site-library/Rcpp/include"
    "/usr/local/lib/R/site-library/Rcpp/include"
    "/usr/lib/R/site-library/Rcpp/include"
    "${RCPP_PATH}/include")

set(RCPP_PKGCONF_LIBRARY_DIRS
    "/usr/local/lib"
    "/usr/lib"
    "/opt/R/site-library/Rcpp/lib"
    "/usr/local/lib/R/site-library/Rcpp/lib"
    "/usr/lib/R/site-library/Rcpp/lib"
    "${RCPP_PATH}/libs"
    "${RCPP_PATH}/libs/x64"
    "${RCPP_PATH}/libs/i386")

# Find the include dir
find_path(RCPP_INCLUDE_DIR
  NAMES Rcpp.h
  PATHS ${RCPP_PKGCONF_LIBRARY_INCLUDE}
)

# Find the library
find_library(RCPP_LIBRARY NAMES Rcpp PATHS ${RCPP_PKGCONF_LIBRARY_DIRS})

if(RCPP_INCLUDE_DIR AND RCPP_LIBRARY)
    set(RCPP_FOUND TRUE)
    mark_as_advanced(RCPP_LIBRARY RCPP_LIBRARY)
else()
    set(RCPP_FOUND FALSE)
    message(FATAL_ERROR "Looking for Rcpp -- not found. Try to set RCPP_PATH to the path of Rcpp")
endif()
