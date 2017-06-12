# Cmake module to find RcppArmadillo
# - Try to find RcppArmadillo
# Once done, this will define
#
#  RCPP_ARMADILLO_FOUND - system has RCPP
#  RCPP_ARMADILLO_INCLUDE_DIR - the RCPP include directories
# Autor: Jose Fernandez Navaro

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
    "/Library/Frameworks/R.framework/Resources"
    "/Library/Frameworks/R.framework/Resources/library")

# Find the include dir and the library
find_path(RCPP_ARMADILLO_INCLUDE_DIR
  NAMES RcppArmadillo.h
  HINTS ${R_PATH} ${R_DIR}
  PATH_SUFFIXES include RcppArmadillo/include RcppArmadillo
)

if(RCPP_INCLUDE_DIR)
    set(RCPP_ARMADILLO_FOUND TRUE)
else()
    set(RCPP_ARMADILLO_FOUND FALSE)
    message(FATAL_ERROR "Looking for RcppArmadillo -- not found.")
endif()
