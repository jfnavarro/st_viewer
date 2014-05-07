#!/bin/sh

set -e
echo This is a work in progress. Right now it does not work.

exit


tempdir=`mktemp -d`


qt3d_srcdir=$tempdir/qt3d/src
qt3d_builddir=$tempdir/qt3d/build

mkdir -p $qt3d_srcdir 
mkdir -p $qt3d_builddir 

stclient_srcdir=/cygdrive/z/code/st_client
stclient_builddir=$tempdir/stclient/build

mkdir -p $stclient_builddir



filepath1=`cygpath -w '/cygdrive/c/Program Files (x86)/Microsoft Visual Studio 11.0/VC/vcvarsall.bat'`
filepath2=`cygpath -w '/cygdrive/c/Qt/Qt5.2.1/5.2.1/msvc2012_64_opengl/bin/qtenv2.bat'`

if /bin/false; then

if /bin/false; then
  qt3d_srcdir=/home/erik.sjolund/qt3d
else
  git clone git://gitorious.org/qt/qt3d.git $qt3d_srcdir
  cd $qt3d_srcdir
  # Following the advice found in:
  # http://dragly.org/2014/03/30/recent-commit-causes-qt3d-to-fail-compilation/
  git checkout d3338a9 -b older_version
  echo "INCLUDEPATH += \$\$PWD/dummy" >> src/imports/threed/threed.pro
  mkdir src/imports/threed/dummy
fi

qt3d_srcdir_windows=`cygpath -w $qt3d_srcdir`
qt3d_builddir_windows=`cygpath -w $qt3d_builddir`


cmd /Q /C call "$filepath1" x86_amd64 "&&" \
  "$filepath2" "&&" \
   cd "$qt3d_builddir_windows" "&&" \
   qmake QMAKE_CXXFLAGS+=/MP "$qt3d_srcdir_windows\\qt3d.pro"  "&&" \
   nmake "&&" \
   nmake install 
fi

cd "$stclient_builddir" 
stclient_builddir_windows=`cygpath -w $stclient_builddir`
stclient_srcdir_windows=`cygpath -w $stclient_srcdir`


cmake_path="/cygdrive/c/Program Files (x86)/CMake 2.8/bin/cmake.exe"
cmake_path_windows=`cygpath -w "$cmake_path"`
#"Visual Studio 11 Win64" 
cmd /Q /C call "$filepath1" x86_amd64 "&&" \
  "$filepath2" "&&" \
   cd "$stclient_builddir_windows" "&&" \
   "$cmake_path_windows" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug "-DCMAKE_PREFIX_PATH=$qt3d_builddir_windows" "$stclient_srcdir_windows" "&&" \
   nmake "&&" \
   nmake install 

echo "qt3d_srcdir=$qt3d_srcdir"
echo "qt3d_builddir=$qt3d_builddir"
