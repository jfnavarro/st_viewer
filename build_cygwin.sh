#!/bin/bash

#  Normally the ST Viewer is built like this:
#  sh build_cygwin.sh  /path/to/stviewer/source  /your/result/dir

set -ex

# THESE VARIABLES NEEDED TO BE CONFIGURED TO BE CORRECT FOR YOUR SYSTEM.
#
# Instead of hard coding the following paths we should be able to use something like
# what is described here: http://stackoverflow.com/a/15335686

# The cmake that is found in cygwin does not contain the "NMake Makefiles" generator
# so we need to use the version of cmake installed on Windows.

cmake_install_dir='/cygdrive/C/Program Files (x86)/CMake'
windows_cmake_filepath=`cygpath -w "${cmake_install_dir}/bin/cmake.exe"`
windows_ctest_filepath=`cygpath -w "${cmake_install_dir}/bin/ctest.exe"`

# Path to Visual Studio
msvc_vars_filepath='/cygdrive/c/Program Files (x86)/Microsoft Visual Studio 12.0/VC/vcvarsall.bat'

# Path to Qt
qt_bin='/cygdrive/c/Qt/5.9.1/msvc2013_64/bin'
qt_env_filepath="$qt_bin/qtenv2.bat"

# Shows the user the expected arguments.
function print_usage_and_exit {
    echo "Usage: build_cygwin.sh path_to_st_client_source result_dir" >&2
    exit 1
}

# Check we have the correct number of args to the script:

if [ $# -ne 2 ]; then
  echo -e "\nERROR: Incorrect number of arguments: expected 2.\n”
  print_usage_and_exit
fi

# Check the user settings are correct:

if [ ! -f "$windows_cmake_filepath" ]; then
    echo -e "\nERROR: CMake path '${windows_cmake_filepath}' was not found.\n"
	exit 1
fi

if [ ! -f "$windows_ctest_filepath" ]; then
    echo -e "\nERROR: CTest path '${windows_ctest_filepath}' was not found.\n"
	exit 1
fi

if [ ! -f "$msvc_vars_filepath" ]; then
    echo -e "\nERROR: Visual Studio's vcvarsall.bat file was not found at '$msvc_vars_filepath'.\n"
	exit 1
fi

if [ ! -f "$qt_env_filepath" ]; then
    echo -e "\nERROR: Qt env file 'qtenv2.bat' was not found at '$qt_env_filepath'.\n"
	exit 1
fi

result_dir=`cygpath -w -a $2`

if [ ! -d "$result_dir" ]; then
    echo "\nERROR: Result directory '$result_dir' does not exist.\n" >&2
    print_usage_and_exit
fi

stclient_srcdir="$1"

if [ ! -d "$stclient_srcdir" ]; then
    echo "\nERROR: Source code directory '$stclient_srcdir' does not exist.\n" >&2
    print_usage_and_exit
fi

stclient_builddir=`mktemp -d /tmp/stclient.XXX`
stclient_builddir_windows=`cygpath -w -a "$stclient_builddir"`
stclient_srcdir_windows=`cygpath -w -a "$stclient_srcdir"`

#"Visual Studio 12 Win64" 

# Paths to qcustomplot and armadillo
qcustomplot='/cygdrive/C/qcustomplot’
armadillo='/cygdrive/C/armadillo’

# When running ctest there was a problem with missing dll. The files were on the system but couldn't be found.
# A good command to diagnose this is "cygcheck".
# By setting the PATH environment variable, the missing dll files can be found.
export PATH="$armadillo:$qcustomplot:$qt_bin:$PATH"

windows_msvc_vars_filepath=`cygpath -w "$msvc_vars_filepath"`
cmd /Q /C call "$windows_msvc_vars_filepath" x86_amd64 "&&" \
  `cygpath -w "$qt_env_filepath"` "&&" \
   cd "$stclient_builddir_windows" "&&" \
   $cmd "$windows_cmake_filepath" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=$build_type "$stclient_srcdir_windows" "&&" \
   nmake "&&" \
   "$windows_ctest_filepath" "&&" \
   nmake package

cp "$stclient_builddir"/*.exe "$result_dir"
