#!/bin/bash

#  Normally the stclient is built like this:
#  sh build_cygwin.sh  /path/to/stclient/source  stclient_production_build  /your/result/dir
#
#  If you need to edit the stclient source code you can save compilation time if you run
#  sh build_cygwin.sh  /path/to/stclient/source  stclient_build_only_run_make  /your/result/dir

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

msvc_vars_filepath='/cygdrive/c/Program Files (x86)/Microsoft Visual Studio 12.0/VC/vcvarsall.bat'

qt_bin='/cygdrive/c/Qt/5.6/msvc2013_64/bin'
qt_env_filepath="$qt_bin/qtenv2.bat"

# Shows the user the expected arguments.
function print_usage_and_exit {
    echo "Usage: build_cygwin.sh path_to_st_client_source [ stclient_production_build | stclient_development_build | stclient_build_only_run_make ] result_dir" >&2
    exit 1
}

# Check we have the correct number of args to the script:

if [ $# -ne 3 ]; then
  echo -e "\nERROR: Incorrect number of arguments: expected 3.\n"
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

if ! [ $2 = "stclient_production_build" -o $2 = "stclient_development_build" -o $2 = "stclient_build_only_run_make" ]; then
  print_usage_and_exit
fi

result_dir=`cygpath -w $3`

if [ ! -d "$result_dir" ]; then
    echo "\nERROR: Result directory '$result_dir' does not exist.\n" >&2
    print_usage_and_exit
fi

stclient_srcdir="$1"

if [ ! -d "$stclient_srcdir" ]; then
    echo "\nERROR: Source code directory '$stclient_srcdir' does not exist.\n" >&2
    print_usage_and_exit
fi

if [ $2 = "stclient_build_only_run_make" ]; then
  stclient_builddir=`ls -dt1 /tmp/stclient.* | head -1`
  if [ -z $stclient_builddir ]; then
    echo -e "nERROR: could not find the last build dir for the stclient under /tmp/stclient.*\n"  >&2
    exit
  fi
else 
  stclient_builddir=`mktemp -d /tmp/stclient.XXX`
fi

cd "$stclient_builddir" 
stclient_builddir_windows=`cygpath -w "$stclient_builddir"`
stclient_srcdir_windows=`cygpath -w "$stclient_srcdir"`

#"Visual Studio 11 Win64" 

if [ $2 = "stclient_development_build" ]; then
  build_type=Debug
fi

if [ $2 = "stclient_production_build" ]; then
  build_type=Release
fi

# You must turn remotedata to ON and set the other variables to use the viewer with the ST API
server=development
endpoint=
clientid=
secretid=
publickey=
remotedata=OFF

if [ $2 = "stclient_build_only_run_make" ]; then
  cmd="echo skipping running cmake"
else
  cmd=""
fi

# When running ctest there was a problem with missing dll. The files were on the system but couldn't be found.
# A good command to diagnose this is "cygcheck".
# By setting the PATH environment variable, the missing dll files can be found.
export PATH="$qt_bin:$PATH"

windows_msvc_vars_filepath=`cygpath -w "$msvc_vars_filepath"`
cmd /Q /C call "$windows_msvc_vars_filepath" x86_amd64 "&&" \
  `cygpath -w "$qt_env_filepath"` "&&" \
   cd "$stclient_builddir_windows" "&&" \
   $cmd "$windows_cmake_filepath" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=$build_type -DREMOTE_DATA=$remotedata -DPUBLICKEY=$publickey -DSECRETID=$secretid -DCLIENTID=$clientid -DENDPOINT=$endpoint -D "$stclient_srcdir_windows" "&&" \
   nmake "&&" \
   "$windows_ctest_filepath" "&&" \
   nmake package

cp "$stclient_builddir"/*.exe "$result_dir"
