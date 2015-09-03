#!/bin/bash

#  Normally the stclient is built like this:
#  sh build_cygwin.sh /path/to/stclient/source stclient_production_build
#
#  If you need to edit the stclient source code you can save compilation time if you run
#  sh build_cygwin.sh /path/to/stclient/source stclient_build_only_run_make

set -ex

libjpeg_turbo=`cygpath -w /cygdrive/c/libjpeg-turbo`

function print_usage_and_exit {
    echo "Usage: build_cygwin.sh path_to_st_client_source [ stclient_production_build | stclient_development_build | stclient_build_only_run_make ] result_dir" >&2
    exit 1
}

if [ $# -ne 3 ]; then
  print_usage_and_exit
fi

result_dir=$3

if ! [ $2 = "stclient_production_build" -o $2 = "stclient_development_build" -o $2 = "stclient_build_only_run_make" ]; then
  print_usage_and_exit
fi

if [ ! -d "$3" ]; then
    echo "dir $3 does not exist" >&2
    print_usage_and_exit
fi

stclient_srcdir="$1"

if [ $2 = "stclient_build_only_run_make" ]; then
  stclient_builddir=`ls -dt1 /tmp/stclient.* | head -1`
  if [ -z $stclient_builddir ]; then
    echo "error: could not find the last build dir for the stclient under /tmp/stclient.*"  >&2
    exit
  fi
else 
  stclient_builddir=`mktemp -d /tmp/stclient.XXX`
fi

# instead of hard coding the following paths we should be able to use something like
# what is described here: http://stackoverflow.com/a/15335686

filepath1=`cygpath -w '/cygdrive/c/Program Files (x86)/Microsoft Visual Studio 12.0/VC/vcvarsall.bat'`
filepath2=`cygpath -w '/cygdrive/c/Qt/Qt5.5.0/5.5/msvc2013_64/bin/qtenv2.bat'`

cd "$stclient_builddir" 
stclient_builddir_windows=`cygpath -w $stclient_builddir`
stclient_srcdir_windows=`cygpath -w $stclient_srcdir`

# The cmake that is found in cygwin does not contain the "NMake Makefiles" generator
# so we need to use the windows version of cmake

cmake_path="/cygdrive/c/cmake/bin/cmake.exe"
cmake_path_windows=`cygpath -w "$cmake_path"`

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

cmd /Q /C call "$filepath1" x86_amd64 "&&" \
  "$filepath2" "&&" \
   cd "$stclient_builddir_windows" "&&" \
   $cmd "$cmake_path_windows" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=$build_type -DSERVER:STRING=$server "-DCMAKE_PREFIX_PATH=$libjpeg_turbo" "$stclient_srcdir_windows" "&&" \
   nmake "&&" \
   nmake package

cp ${stclient_builddir}/Release/stVi.exe "$result_dir"
cp ${stclient_builddir}/stVi-*-win64.exe "$result_dir"
