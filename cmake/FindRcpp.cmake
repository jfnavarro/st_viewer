#Cmake module to find Rcpp
# - Try to find Rcpp
# Once done, this will define
#
#  RCPP_FOUND - system has RCPP
#  RCPP_INCLUDE_DIRS - the RCPP include directories
#  RCPP_LIBRARIES - link these to use RCPP
# Autor: Omar Andres Zapata Mesa 31/05/2013
# Adjusted: Jose Fernandez Navaro

find_program(R_EXECUTABLE NAMES R R.exe)

execute_process(COMMAND echo "Rcpp:::CxxFlags()"
                COMMAND ${R_EXECUTABLE} --vanilla --slave
                OUTPUT_VARIABLE RCPP_INCLUDE_DIR
                ERROR_VARIABLE RCPP_INCLUDE_DIR_ERR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
  
execute_process(COMMAND echo "cat(find.package('Rcpp'))"
                COMMAND ${R_EXECUTABLE} --vanilla --slave
                OUTPUT_VARIABLE RCPP_PATH
                ERROR_VARIABLE RCPP_PATH_ERR
                OUTPUT_STRIP_TRAILING_WHITESPACE)

set(RCPP_PKGCONF_LIBRARY_DIRS  
    "/usr/local/lib"
    "/usr/lib"
    "/opt/R/site-library/Rcpp/lib"
    "/usr/local/lib/R/site-library/Rcpp/lib"
    "/usr/lib/R/site-library/Rcpp/lib"
    "${RCPP_PATH}/libs"
    "${RCPP_PATH}/libs/x64"
    "${RCPP_PATH}/libs/i386")

# Finally the library itself
find_library(RCPP_LIBRARY NAMES Rcpp PATHS ${RCPP_PKGCONF_LIBRARY_DIRS})

# Remove un wanted -I from the include path
string(REPLACE "-I" "" RCPP_INCLUDE_DIR "${RCPP_INCLUDE_DIR}")

# Setting up the results 
set(RCPP_INCLUDE_DIRS ${RCPP_INCLUDE_DIR})
set(RCPP_LIBRARIES ${RCPP_LIBRARY})
if (("${RCPP_INCLUDE_DIR}" STREQUAL "") OR ("${RCPP_LIBRARY}" STREQUAL "")) 
  set(RCPP_FOUND FALSE)
  message(STATUS "Looking for Rcpp -- not found ")
  message(STATUS "Install it running 'R -e \"install.packages(\\\"RInside\\\",repos=\\\"http://cran.irsn.fr/\\\")\"'")
else()
  set(RCPP_FOUND TRUE)
endif()
