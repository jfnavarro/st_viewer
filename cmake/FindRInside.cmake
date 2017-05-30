# Cmake module to find RInisde
# - Try to find Rinside
# Once done, this will define
#
#  RINSIDE_FOUND - system has RINSIDE
#  RINSIDE_INCLUDE_DIR - the RINSIDE include directories
#  RINSIDE_LIBRARY - link these to use RINSIDE
# Autor: Omar Andres Zapata Mesa 31/05/2013
# Mofified by: Jose Fernandez Navarro

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

# Find the include dir
find_path(RINSIDE_INCLUDE_DIR
  NAMES RInside.h
  HINTS ${R_PATH} ${R_DIR}
  PATH_SUFFIXES include RInside/include RInside
)
# TODO fix this
set(RINSIDE_INCLUDE_DIR "/Library/Frameworks/R.framework/Resources/library/RInside/include")

# Find the library
find_library(RINSIDE_LIBRARY
    NAMES RInside
    HINTS ${R_PATH} ${R_DIR} ${R_PATH}/library ${R_DIR}/library
    PATH_SUFFIXES libs lib lib32 lib64 RInside/libs RInside/lib RInside/lib32 RInside/lib64
)
# TODO fix this
set(RINSIDE_LIBRARY "/Library/Frameworks/R.framework/Resources/library/RInside/lib/libRInside.a")

if(RINSIDE_INCLUDE_DIR AND RINSIDE_LIBRARY)
    set(RINSIDE_FOUND TRUE)
    mark_as_advanced(RINSIDE_LIBRARY RINSIDE_LIBRARY)
else()
    set(RINSIDE_FOUND FALSE)
    message(FATAL_ERROR "Looking for RInside -- not found.")
endif()
