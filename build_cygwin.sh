#!/bin/sh

set -e

if [ $# -ne 1 ]; then
    echo "Usage: build_cygwin.sh path_to_st_client_source" >&2
    exit 1
fi

tempdir1=`mktemp -d`
tempdir2=`mktemp -d`
#tempdir2=/tmp/tmp.wHe0yT3Qpn
#tempdir=/tmp/tmp.jm8Xyaz0al

qt3d_srcdir=$tempdir1/qt3d/src
qt3d_builddir=$tempdir1/qt3d/build

mkdir -p $qt3d_srcdir 
mkdir -p $qt3d_builddir 

stclient_srcdir="$1"
stclient_builddir=$tempdir2/stclient/build

mkdir -p $stclient_builddir

# instead of hard coding the following paths we should be able to use something like
# what is described here: http://stackoverflow.com/a/15335686

filepath1=`cygpath -w '/cygdrive/c/Program Files (x86)/Microsoft Visual Studio 12.0/VC/vcvarsall.bat'`
filepath2=`cygpath -w '/cygdrive/c/Qt/Qt5.3.0/5.3/msvc2013_64_opengl/bin/qtenv2.bat'`

# If you don't want to build the whole (qt3d + stclient), you could edit this section:
if /bin/true; then
  if /bin/true; then
    git clone git://gitorious.org/qt/qt3d.git $qt3d_srcdir
    cd $qt3d_srcdir
    # Better settle on a specific commit of qt3d.
    # Commit bdb98baf8253c69949a8c259369203da9ffb269c is from 2014-06-18
    # I just took the latest from today/ Erik Sjolund 
    git checkout bdb98baf8253c69949a8c259369203da9ffb269c -b tmpbranch
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
else
  qt3d_builddir=/tmp/tmp.haIlCvIBVQ/qt3d/build
  qt3d_builddir_windows=`cygpath -w $qt3d_builddir`
fi

cp $qt3d_builddir/lib/*.dll $qt3d_builddir/bin/

cd "$stclient_builddir" 
stclient_builddir_windows=`cygpath -w $stclient_builddir`
stclient_srcdir_windows=`cygpath -w $stclient_srcdir`

# The cmake that is found in cygwin does not contain the "NMake Makefiles" generator
# so we need to use the windows version of cmake

cmake_path="/cygdrive/c/Program Files (x86)/CMake/bin/cmake.exe"
cmake_path_windows=`cygpath -w "$cmake_path"`
#"Visual Studio 11 Win64" 
cmd /Q /C call "$filepath1" x86_amd64 "&&" \
  "$filepath2" "&&" \
   cd "$stclient_builddir_windows" "&&" \
   "$cmake_path_windows" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DSERVER:STRING=production "-DCMAKE_PREFIX_PATH=$qt3d_builddir_windows" "$stclient_srcdir_windows" "&&" \
   nmake "&&" \
   nmake package

echo "path to exe file: ${stclient_builddir}/Release/stVi.exe"

echo "path to package file:"
ls ${stclient_builddir}/stVi-*-win64.exe
