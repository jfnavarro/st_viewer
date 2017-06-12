#
# - This module locates an installed R distribution.
#
# Input:
# R_LIB_ARCH - For windows (i386 or x64)
#
# Defines the following:
# R_COMMAND - Path to R command
# R_HOME - Path to 'R home', as reported by R
# R_INCLUDE_DIR - Path to R include directory
# R_LIBRARY_BASE - Path to R library
# R_LIBRARY_BLAS - Path to Rblas / blas library
# R_LIBRARY_LAPACK - Path to Rlapack / lapack library
# R_LIBRARY_READLINE - Path to readline library
# R_LIBRARIES - Array of: R_LIBRARY_BASE, R_LIBRARY_BLAS, R_LIBRARY_LAPACK, R_LIBRARY_BASE [, R_LIBRARY_READLINE]
#
# Variable search order:
# 1. Attempt to locate and set R_COMMAND
# - If unsuccessful, generate error and prompt user to manually set R_COMMAND
# 2. Use R_COMMAND to set R_HOME
# 3. Locate other libraries in the priority:
# 1. Within a user-built instance of R at R_HOME
# 2. Within an installed instance of R
# 3. Within external system libraries
#
set(TEMP_CMAKE_FIND_APPBUNDLE ${CMAKE_FIND_APPBUNDLE})
set(CMAKE_FIND_APPBUNDLE "NEVER")
find_program(R_COMMAND R DOC "R executable.")
set(CMAKE_FIND_APPBUNDLE ${TEMP_CMAKE_FIND_APPBUNDLE})
if(NOT R_LIB_ARCH)
    set(R_LIB_ARCH "ThisPathNotExists")
endif()
if(R_COMMAND)
    execute_process(WORKING_DIRECTORY .
        COMMAND ${R_COMMAND} --vanilla --slave RHOME
        OUTPUT_VARIABLE R_ROOT_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    find_path(R_INCLUDE_DIR R.h
        HINTS ${R_ROOT_DIR}
        PATHS /usr/local/lib /usr/local/lib64 /usr/share /usr
        PATH_SUFFIXES include R/include include/R
        DOC "Path to file R.h")

    if(NOT R_INCLUDE_DIR)
        message(
            FATAL_ERROR
            "R.h file not found. Locations tried:\n"
            "/usr/local/lib /usr/local/lib64 /usr/share ${R_ROOT_DIR}"
            )
    endif()

    find_library(R_LIBRARY_BASE R
        HINTS ${R_ROOT_DIR}/lib ${R_ROOT_DIR}/bin/${R_LIB_ARCH} ${R_ROOT_DIR}/Resources/lib ${R_ROOT_DIR}/bin/${R_LIB_ARCH})
    #TODO fix this
    set(R_LIBRARY_BASE "/Library/Frameworks/R.framework/Versions/Current/Resources/lib/libR.dylib")

    if(NOT R_LIBRARY_BASE)
        message(FATAL_ERROR
            "R library not found. Locations tried:\n"
            "${R_ROOT_DIR}/lib ${R_ROOT_DIR}/bin/${R_LIB_ARCH}"
            )
    endif()
    find_library(R_LIBRARY_BLAS NAMES Rblas blas
        HINTS ${R_ROOT_DIR}/lib ${R_ROOT_DIR}/bin/${R_LIB_ARCH})
    find_library(R_LIBRARY_LAPACK NAMES Rlapack lapack
        HINTS ${R_ROOT_DIR}/lib ${R_ROOT_DIR}/bin/${R_LIB_ARCH})
    #find_library(R_LIBRARY_READLINE readline)

else(R_COMMAND)
    message(SEND_ERROR "FindR.cmake requires the following variables to be set: R_COMMAND")
endif(R_COMMAND)

set(R_LIBRARIES ${R_LIBRARY_BASE} ${R_LIBRARY_BLAS} ${R_LIBRARY_LAPACK})

