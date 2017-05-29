
# Cmake module to find RInisde
# - Try to find Rinside
# Once done, this will define
#
#  RINSIDE_FOUND - system has RINSIDE
#  RINSIDE_INCLUDE_DIR - the RINSIDE include directories
#  RINSIDE_LIBRARY - link these to use RINSIDE
# Autor: Omar Andres Zapata Mesa 31/05/2013
# Mofified by: Jose Fernandez Navarro

set(RINSIDE_PKGCONF_LIBRARY_INCLUDE
    "/usr/local/include"
    "/usr/include"
    "/opt/R/site-library/RInside/include"
    "/usr/local/lib/R/site-library/RInside/include"
    "/usr/lib/R/site-library/RInside/include"
    "${RINSIDE_PATH}/include")

set(RINSIDE_PKGCONF_LIBRARY_DIRS
    "/usr/local/lib"
    "/usr/lib"
    "/opt/R/site-library/RInside/lib"
    "/usr/local/lib/R/site-library/RInside/lib"
    "/usr/lib/R/site-library/RInside/lib"
    "${RINSIDE_PATH}/lib"
    "${RINSIDE_PATH}/libs"
    "${RINSIDE_PATH}/libs/x64"
    "${RINSIDE_PATH}/libs/i386")

# Find the include dir
find_path(RINSIDE_INCLUDE_DIR
  NAMES RInside.h
  PATHS ${RINSIDE_PKGCONF_LIBRARY_INCLUDE}
)

# Find the library
find_library(RINSIDE_LIBRARY NAMES RInside.so PATHS ${RINSIDE_PKGCONF_LIBRARY_DIRS})

if(RINSIDE_INCLUDE_DIR AND RINSIDE_LIBRARY)
    set(RINSIDE_FOUND TRUE)
    mark_as_advanced(RINSIDE_LIBRARY RINSIDE_LIBRARY)
else()
    set(RINSIDE_FOUND FALSE)
    message(FATAL_ERROR "Looking for RInside -- not found. Try to set RINSIDE_PATH to the path of RInside")
endif()
