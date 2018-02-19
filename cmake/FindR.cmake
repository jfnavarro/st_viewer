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
        set(LIBR_LIB "${LIBR_HOME}/lib/libR.dylib")
    else()
        message(FATAL_ERROR "Could not find R (OSX)")
    endif()

# detection for UNIX & Win32
else()

    # look for the R executable
    find_program(LIBR_EXECUTABLE R)
    if(LIBR_EXECUTABLE-NOTFOUND)
        message(FATAL_ERROR "Unable to locate R executable")
    endif()

    # ask R for the home path
    if(NOT LIBR_HOME)
        execute_process(
            COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "cat(R.home())"
            OUTPUT_VARIABLE LIBR_HOME
            )
        if(LIBR_HOME)
            set(LIBR_HOME ${LIBR_HOME} CACHE PATH "R home directory")
		else()
			message(FATAL_ERROR "Unable to locate R home (Linux)")
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

    # look for the core R library
    find_library(LIBR_LIB NAMES R libR
        HINTS ${LIBR_LIB_DIR} ${LIBR_HOME}/bin/i386 
		${LIBR_HOME} ${LIBR_HOME}/lib ${LIBR_HOME}/bin)

endif()

if (LIBR_INCLUDE_DIRS)
    message(STATUS "Found R include dir: ${LIBR_INCLUDE_DIRS}")
elseif(LIBR_INCLUDE_DIRS)
    message(FATA_ERROR "R could not be found")
endif()

if (LIBR_LIB)
    message(STATUS "Found R libraries: ${LIBR_LIB}")
elseif(LIBR_LIB)
    message(FATA_ERROR "R libraries could not be found")
endif()

# look for lapack
find_library(LIBR_LAPACK_LIBRARY NAMES Rlapack lapack
	HINTS ${LIBR_LIB_DIR} ${LIBR_HOME}/bin/i386 ${LIBR_HOME} ${LIBR_HOME}/lib ${LIBR_HOME}/bin)

# look for blas
find_library(LIBR_BLAS_LIBRARY NAMES Rblas blas
	HINTS ${LIBR_LIB_DIR} ${LIBR_HOME}/bin/i386 ${LIBR_HOME} ${LIBR_HOME}/lib ${LIBR_HOME}/bin)

if (LIBR_LAPACK_LIBRARY)
    message(STATUS "Found R lapack libraries: ${LIBR_LAPACK_LIBRARY}")
elseif(LIBR_LAPACK_LIBRARY)
    message(FATA_ERROR "R lapack libraries could not be found")
endif()

if (LIBR_BLAS_LIBRARY)
    message(STATUS "Found R blas libraries: ${LIBR_BLAS_LIBRARY}")
elseif (LIBR_BLAS_LIBRARY)
    message(FATA_ERROR "R blas libraries could not be found")
endif()

set(LIBR_LIBRARIES ${LIBR_LIB} ${LIBR_BLAS_LIBRARY} ${LIBR_LAPACK_LIBRARY})

set(NUM_TRUNC_CHARS 2)

execute_process(COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "Rcpp:::CxxFlags()" OUTPUT_VARIABLE LIBRCPP_INCLUDE_DIRS)
string(SUBSTRING ${LIBRCPP_INCLUDE_DIRS} ${NUM_TRUNC_CHARS} -1 LIBRCPP_INCLUDE_DIRS)
execute_process(COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "RInside:::CxxFlags()" OUTPUT_VARIABLE LIBRINSIDE_INCLUDE_DIRS)
string(SUBSTRING ${LIBRINSIDE_INCLUDE_DIRS} ${NUM_TRUNC_CHARS} -1 LIBRINSIDE_INCLUDE_DIRS)
execute_process(COMMAND ${LIBR_EXECUTABLE} "--slave" "--no-save" "-e" "RInside:::LdFlags()" OUTPUT_VARIABLE LIBRINSIDE_LIBRARIES)

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
	string(REGEX REPLACE "[.][a]" ".dll" LIBRINSIDE_LIBRARIES ${LIBRINSIDE_LIBRARIES})
elseif (UNIX AND NOT APPLE)
    if (${LIBRINSIDE_LIBRARIES} MATCHES "[-][L]([^ ;])+")
        string(SUBSTRING ${CMAKE_MATCH_0} ${NUM_TRUNC_CHARS} -1 LIBRINSIDE_LIBRARIES_DIR)
    endif()
    if (${LIBRINSIDE_LIBRARIES} MATCHES "[-][l][R]([^ ;])+")
        string(SUBSTRING ${CMAKE_MATCH_0} ${NUM_TRUNC_CHARS} -1 LIBRINSIDE_LIBRARIES_NAME)
        set(LIBRINSIDE_LIBRARIES "${LIBRINSIDE_LIBRARIES_DIR}/lib${LIBRINSIDE_LIBRARIES_NAME}.so")
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
