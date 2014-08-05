#!/bin/bash

# Better settle on a specific commit of qt3d.
# Commit bdb98baf8253c69949a8c259369203da9ffb269c is from 2014-06-18
# I just took the latest from today/ Erik Sjolund 

qt3d_commit=bdb98baf8253c69949a8c259369203da9ffb269c

#  Normally bthe stclient is built like this:
#  sh build_cygwin.sh /path/to/stclient/source build_qt3d stclient_production_build
#
#  If you need to edit the stclient source code you can save compilation time if you run
#  sh build_cygwin.sh /path/to/stclient/source use_last_built_qt3d stclient_build_only_run_make

set -e

function print_usage_and_exit {
    echo "Usage: build_cygwin.sh path_to_st_client_source [ build_qt3d | use_last_built_qt3d ] [ stclient_production_build | stclient_development_build | stclient_build_only_run_make ]" >&2
    exit 1
}

if [ $# -ne 3 ]; then
  print_usage_and_exit
fi

if ! [ $2 = "build_qt3d" -o $2 = "use_last_built_qt3d" ]; then
  print_usage_and_exit
fi

if ! [ $3 = "stclient_production_build" -o $3 = "stclient_development_build" -o $3 = "stclient_build_only_run_make" ]; then
  print_usage_and_exit
fi

stclient_srcdir="$1"

if [ $3 = "stclient_build_only_run_make" ]; then
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
filepath2=`cygpath -w '/cygdrive/c/Qt/Qt5.3.1/5.3/msvc2013_64_opengl/bin/qtenv2.bat'`

if [ $2 = "build_qt3d" ]; then
  qt3d_dir=`mktemp -d /tmp/qt3d.XXX`

  qt3d_srcdir=$qt3d_dir/src
  qt3d_builddir=$qt3d_dir/build

  mkdir -p $qt3d_srcdir 
  mkdir -p $qt3d_builddir 

  if /bin/true; then
    git clone git://gitorious.org/qt/qt3d.git $qt3d_srcdir
    cd $qt3d_srcdir
    git checkout $qt3d_commit -b tmpbranch
    echo "INCLUDEPATH += \$\$PWD/dummy" >> src/imports/threed/threed.pro
    mkdir src/imports/threed/dummy
  else
    qt3d_srcdir=/home/erik.sjolund/qt3d
  fi
  qt3d_srcdir_windows=`cygpath -w $qt3d_srcdir`
  qt3d_builddir_windows=`cygpath -w $qt3d_builddir`
  cmd /Q /C call "$filepath1" x86_amd64 "&&" \
  "$filepath2" "&&" \
  cd "$qt3d_builddir_windows" "&&" \
  qmake QMAKE_CXXFLAGS+=/MP "$qt3d_srcdir_windows\\qt3d.pro"  "&&" \
  nmake release "&&" \
  nmake install 
  cp $qt3d_builddir/lib/*.dll $qt3d_builddir/bin/
else
  qt3d_builddir=`ls -dt1 /tmp/qt3d.*/build | head -1`
  if [ -z $qt3d_builddir ]; then
    echo "error: could not find the last built qt3d under /tmp/qt3d.*/build"  >&2
    exit
  fi
  qt3d_builddir_windows=`cygpath -w $qt3d_builddir`
fi

cd "$stclient_builddir" 
stclient_builddir_windows=`cygpath -w $stclient_builddir`
stclient_srcdir_windows=`cygpath -w $stclient_srcdir`

# The cmake that is found in cygwin does not contain the "NMake Makefiles" generator
# so we need to use the windows version of cmake

cmake_path="/cygdrive/c/Program Files (x86)/CMake/bin/cmake.exe"
cmake_path_windows=`cygpath -w "$cmake_path"`
#"Visual Studio 11 Win64" 

if [ $3 = "stclient_development_build" ]; then
  server=development
  build_type=Debug
fi

if [ $3 = "stclient_production_build" ]; then
  server=production
  build_type=Release
fi

if [ $3 = "stclient_build_only_run_make" ]; then
  cmd="echo skipping running cmake"
else
  cmd=""
fi

cmd /Q /C call "$filepath1" x86_amd64 "&&" \
  "$filepath2" "&&" \
   cd "$stclient_builddir_windows" "&&" \
   $cmd "$cmake_path_windows" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=$build_type -DSERVER:STRING=$server "-DCMAKE_PREFIX_PATH=$qt3d_builddir_windows" "$stclient_srcdir_windows" "&&" \
   nmake "&&" \
   nmake package


echo "path to exe file: ${stclient_builddir}/Release/stVi.exe"

echo "path to package file:"
ls ${stclient_builddir}/stVi-*-win64.exe
