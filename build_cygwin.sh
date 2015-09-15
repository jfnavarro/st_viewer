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
windows_cmake_filepath=`cygpath -w '/cygdrive/C/Program Files (x86)/CMake/bin/cmake.exe'`

msvc_vars_filepath=`cygpath -w '/cygdrive/c/Program Files (x86)/Microsoft Visual Studio 12.0/VC/vcvarsall.bat'`
qt_env_filepath=`cygpath -w '/cygdrive/c/Qt/5.5/msvc2013_64/bin/qtenv2.bat'`
libjpeg_turbo_dir=`cygpath -w /cygdrive/c/libjpeg-turbo64`

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

if [ ! -f "$msvc_vars_filepath" ]; then
    echo -e "\nERROR: Visual Studio's vcvarsall.bat file was not found at '$msvc_vars_filepath'.\n"
	exit 1
fi

if [ ! -f "$qt_env_filepath" ]; then
    echo -e "\nERROR: Qt env file 'qtenv2.bat' was not found at '$qt_env_filepath'.\n"
	exit 1
fi

if [ ! -d "$libjpeg_turbo_dir" ]; then
    echo -e "\nERROR: libjpeg-turbo was not found installed at '$libjpeg_turbo_dir'.\n"
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
stclient_builddir_windows=`cygpath -w $stclient_builddir`
stclient_srcdir_windows=`cygpath -w $stclient_srcdir`

#"Visual Studio 11 Win64" 

if [ $2 = "stclient_development_build" ]; then
  server=development
  build_type=Debug
fi

if [ $2 = "stclient_production_build" ]; then
  server=production
  build_type=Release
fi

if [ $2 = "stclient_build_only_run_make" ]; then
  cmd="echo skipping running cmake"
else
  cmd=""
fi

cmd /Q /C call "$msvc_vars_filepath" x86_amd64 "&&" \
  "$qt_env_filepath" "&&" \
   cd "$stclient_builddir_windows" "&&" \
   $cmd "$windows_cmake_filepath" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=$build_type -DSERVER:STRING=$server "-DCMAKE_PREFIX_PATH=$libjpeg_turbo_dir" "$stclient_srcdir_windows" "&&" \
   nmake "&&" \
   nmake package

cp "$stclient_builddir"/stVi.exe "$result_dir"
cp "$stclient_builddir"/stVi-*-win64.exe "$result_dir"
