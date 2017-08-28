#
# FindLibR.cmake
#
# Copyright (C) 2009-11 by RStudio, Inc.
#
# Modified by Jose Fernandez (2017) so to also search for RInside, Rcpp and RcppArmadillo
#
# This program is licensed to you under the terms of version 3 of the
# GNU Affero General Public License. This program is distributed WITHOUT
# ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF NON-INFRINGEMENT,
# MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Please refer to the
# AGPL (http://www.gnu.org/licenses/agpl-3.0.txt) for more details.
#
#
#
# LIBR_HOME
# LIBR_INCLUDE_DIRS
# LIBR_DOC_DIR
# LIBR_LIBRARIES
# LIBRCPP_INCLUDE_DIRS
# LIBRINSIDE_INCLUDE_DIRS
# LIBRINSIDE_LIBRARIES
# LIBRCPPARMADILLO_INCLUDE_DIRS

# detection for OSX (look for R framework)
if(APPLE)

    find_library(LIBR_LIBRARIES R)
    if(LIBR_LIBRARIES)
        set(LIBR_HOME "${LIBR_LIBRARIES}/Resources" CACHE PATH "R home directory")
        set(LIBR_INCLUDE_DIRS "${LIBR_HOME}/include" CACHE PATH "R include directory")
        set(LIBR_DOC_DIR "${LIBR_HOME}/doc" CACHE PATH "R doc directory")
        set(LIBR_EXECUTABLE "${LIBR_HOME}/R" CACHE PATH "R executable")
        set(LIBR_LIBRARIES "${LIBR_HOME}/lib/libR.dylib")
    else()
        message(STATUS "Could not find libR shared library.")
    endif()

# detection for UNIX & Win32
else()

    # Find R executable and paths (UNIX)
    if(UNIX)

        # find executable
        find_program(LIBR_EXECUTABLE R)
        if(LIBR_EXECUTABLE-NOTFOUND)
            message(STATUS "Unable to locate R executable")
        endif()

        # ask R for the home path
        if(NOT LIBR_HOME)
            execute_process(
                COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "cat(R.home())"
                OUTPUT_VARIABLE LIBR_HOME
                )
            if(LIBR_HOME)
                set(LIBR_HOME ${LIBR_HOME} CACHE PATH "R home directory")
            endif()
        endif()

        # ask R for the include dir
        if(NOT LIBR_INCLUDE_DIRS)
            execute_process(
                COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "cat(R.home('include'))"
                OUTPUT_VARIABLE LIBR_INCLUDE_DIRS
                )
            if(LIBR_INCLUDE_DIRS)
                set(LIBR_INCLUDE_DIRS ${LIBR_INCLUDE_DIRS} CACHE PATH "R include directory")
            endif()
        endif()

        # ask R for the doc dir
        if(NOT LIBR_DOC_DIR)
            execute_process(
                COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "cat(R.home('doc'))"
                OUTPUT_VARIABLE LIBR_DOC_DIR
                )
            if(LIBR_DOC_DIR)
                set(LIBR_DOC_DIR ${LIBR_DOC_DIR} CACHE PATH "R doc directory")
            endif()
        endif()

        # ask R for the lib dir
        if(NOT LIBR_LIB_DIR)
            execute_process(
                COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "cat(R.home('lib'))"
                OUTPUT_VARIABLE LIBR_LIB_DIR
                )
        endif()

        # Find R executable and paths (Win32)
    else()

        # find the home path
        if(NOT LIBR_HOME)

            # read home from the registry
            get_filename_component(LIBR_HOME
                "[HKEY_LOCAL_MACHINE\\SOFTWARE\\R-core\\R;InstallPath]"
                ABSOLUTE CACHE)

            # print message if not found
            if(NOT LIBR_HOME)
                message(STATUS "Unable to locate R home (not written to registry)")
            endif()

        endif()

        # set other R paths based on home path
        set(LIBR_INCLUDE_DIRS "${LIBR_HOME}/include" CACHE PATH "R include directory")
        set(LIBR_DOC_DIR "${LIBR_HOME}/doc" CACHE PATH "R doc directory")

        # set library hint path based on whether  we are doing a special session 64 build
        if(LIBR_FIND_WINDOWS_64BIT)
            set(LIBRARY_ARCH_HINT_PATH "${LIBR_HOME}/bin/x64")
        else()
            set(LIBRARY_ARCH_HINT_PATH "${LIBR_HOME}/bin/i386")
        endif()

    endif()

    # look for the R executable
    find_program(LIBR_EXECUTABLE R
        HINTS ${LIBRARY_ARCH_HINT_PATH} ${LIBR_HOME}/bin)
    if(LIBR_EXECUTABLE-NOTFOUND)
        message(STATUS "Unable to locate R executable")
    endif()

    # look for the core R library
    find_library(LIBR_CORE_LIBRARY NAMES R libR
        HINTS ${LIBR_LIB_DIR} ${LIBRARY_ARCH_HINT_PATH} ${LIBR_HOME} ${LIBR_HOME}/lib ${LIBR_HOME}/bin)
    if(LIBR_CORE_LIBRARY)
        set(LIBR_LIBRARIES ${LIBR_CORE_LIBRARY})
    else()
        message(STATUS "Could not find libR shared library.")
    endif()

endif()

if(LIBR_INCLUDE_DIRS)
    message(STATUS "Found R: ${LIBR_INCLUDE_DIRS}")
elseif(LIBR_INCLUDE_DIRS)
    message(FATA_ERROR "R could not be found")
endif()

# look for lapack
find_library(LIBR_LAPACK_LIBRARY NAMES Rlapack
    HINTS ${LIBR_LIB_DIR} ${LIBRARY_ARCH_HINT_PATH} ${LIBR_HOME} ${LIBR_HOME}/lib ${LIBR_HOME}/bin)

if(LIBR_LAPACK_LIBRARY)
    message(STATUS "Found R lapack libraries: ${LIBR_LAPACK_LIBRARY}")
elseif(LIBR_LAPACK_LIBRARY)
    message(FATA_ERROR "R lapack libraries could not be found")
endif()

# look for blas
find_library(LIBR_BLAS_LIBRARY NAMES Rblas blas
    HINTS ${LIBR_LIB_DIR} ${LIBRARY_ARCH_HINT_PATH} ${LIBR_HOME} ${LIBR_HOME}/lib ${LIBR_HOME}/bin)

if(LIBR_BLAS_LIBRARY)
    message(STATUS "Found R blas libraries: ${LIBR_BLAS_LIBRARY}")
elseif(LIBR_BLAS_LIBRARY)
    message(FATA_ERROR "R blas libraries could not be found")
endif()

set(LIBR_LIBRARIES ${LIBR_LIBRARIES} ${LIBR_BLAS_LIBRARY} ${LIBR_LAPACK_LIBRARY})

set(NUM_TRUNC_CHARS 2)

execute_process(COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "Rcpp:::CxxFlags()" OUTPUT_VARIABLE LIBRCPP_INCLUDE_DIRS)
string(SUBSTRING ${LIBRCPP_INCLUDE_DIRS} ${NUM_TRUNC_CHARS} -1 LIBRCPP_INCLUDE_DIRS)

execute_process(COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "RInside:::CxxFlags()" OUTPUT_VARIABLE LIBRINSIDE_INCLUDE_DIRS)
execute_process(COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "RInside:::LdFlags()" OUTPUT_VARIABLE LIBRINSIDE_LIBRARIES)
string(SUBSTRING ${LIBRINSIDE_INCLUDE_DIRS} ${NUM_TRUNC_CHARS} -1 LIBRINSIDE_INCLUDE_DIRS)

execute_process(COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "RcppArmadillo:::CxxFlags()" OUTPUT_VARIABLE LIBRCPPARMADILLO_INCLUDE_DIRS)
string(LENGTH ${LIBRCPPARMADILLO_INCLUDE_DIRS} INCLLENGTH)
math(EXPR INCLLENGTH "${INCLLENGTH}-4")
string(SUBSTRING ${LIBRCPPARMADILLO_INCLUDE_DIRS} 3 ${INCLLENGTH} LIBRCPPARMADILLO_INCLUDE_DIRS)

if (WIN32)
    # Remove the quotes for RInside
    string(SUBSTRING ${LIBRINSIDE_LIBRARIES} 1 -1 LIBRINSIDE_LIBRARIES)
    string(LENGTH ${LIBRINSIDE_LIBRARIES} lenRInsideFQNameLen)
    math(EXPR lenRInsideFQNameLen ${lenRInsideFQNameLen}-1)
    string(SUBSTRING ${LIBRINSIDE_LIBRARIES} 0 ${lenRInsideFQNameLen} LIBRINSIDE_LIBRARIES)
    string(LENGTH "libRInside.a" lenRInsideName)
    string(LENGTH ${LIBRINSIDE_LIBRARIES} lenRInsideFQName)
    math(EXPR RLibPathLen ${lenRInsideFQName}-${lenRInsideName}-1)
    string(SUBSTRING ${LIBRINSIDE_LIBRARIES} 0 ${RLibPathLen} LIBRINSIDE_LIBRARIES)
    math(EXPR RLibPathLen ${RLibPathLen}+1)
    string(SUBSTRING ${LIBRINSIDE_LIBRARIES} ${RLibPathLen} -1 LIBRINSIDE_LIBRARIES)
else()
    if (${LIBRINSIDE_LIBRARIES} MATCHES "[-][L]([^ ;])+")
        string(SUBSTRING ${CMAKE_MATCH_0} ${NUM_TRUNC_CHARS} -1 LIBRINSIDE_LIBRARIES)
    endif()

    if (${LIBRINSIDE_LIBRARIES} MATCHES "[-][l][R]([^;])+")
        string(SUBSTRING ${CMAKE_MATCH_0} ${NUM_TRUNC_CHARS} -1 LIBRINSIDE_LIBRARIES)
    endif()
endif()

if (LIBRCPP_INCLUDE_DIRS)
    message(STATUS "Found Rcpp: " ${LIBRCPP_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Rcpp not found!")
endif()

if (LIBRINSIDE_INCLUDE_DIRS)
    message(STATUS "Found RInside: " ${LIBRINSIDE_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "RInside not found!")
endif()

if (LIBRINSIDE_LIBRARIES)
    message(STATUS "Found RInside libraries: " ${LIBRINSIDE_LIBRARIES})
else()
    message(FATAL_ERROR "RInside libraries not found!")
endif()

if (LIBRCPPARMADILLO_INCLUDE_DIRS)
    message(STATUS "Found RcppArmadillo: " ${LIBRCPPARMADILLO_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "RcppArmadillo not found!")
endif()

# mark low-level variables from FIND_* calls as advanced
mark_as_advanced(
    LIBR_LIBRARIES
    LIBR_LAPACK_LIBRARY
    LIBR_BLAS_LIBRARY
    LIBRINSIDE_LIBRARIES
    )
