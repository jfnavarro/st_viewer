# Spatial Transcriptomics Research Viewer

The ST viewer is a tool to visualize spatially resolved gene
expression data on top of cell tissue figures with the correct
location.

The ST viewer allows to interact with the data dynamically.
Users can see where specific genes are expressed and how
expressed they are. It has different threshold options
and many visualization options. It also allows
to select areas of the tissue to obtain gene patterns
to later do DEA or cell classification. 

The ST viewer uses the data generated with the ST Pipeline (link), 
a pair of tissue images and an alignment matrix (to convert
array coordinates to image coordinates). 

The ST viewer can access datasets stored in a database
or imported locally trough the viewer. 

## Manual
See MANUAL in (link to manual) for more information
on how to use the ST Viewer.

## License
See LICENSE for the license terms and DEPENDENCIES for the 3rd party
libraries that are used in this software.

## Building
For the moment we do not generate binaries
but you can build the ST Viewer following these
instructions :

###### OSX
* Download and install Qt open source from : http://qt-project.org/downloads

* Issue the following to install the necessary dependencies:
  E.x (Assuming MacPorts is installed, otherwise http://www.macports.org/install.php)
        
        sudo port install xcode cmake git

    NOTE : Make sure the XCode Command Line Tools are installed
* Clone the repository to a specific folder and build the application

        git clone st_viewer_repo /path/to/source
        mkdir /path/to/build
        cd /path/to/build
        cmake [-DCMAKE_INSTALL_PREFIX="/usr/local/bin"] \
          [-DCMAKE_BUILD_TYPE="Debug" | "Release"] \
          [-DCMAKE_PREFIX_PATH="/path/to/qt"] \
          [-DCMAKE_OSX_SYSROOT=”/path/to/macosx.sdk”] \
          [-DCMAKE_OSX_DEPLOYMENT_TARGET=version] \
          [-DSERVER=”production | development | local”] \
          [-DCLIENTID="some_name"] \
          [-DSECRETID="password"]
          /path/to/source

    Where : 

    DCMAKE_INSTALL_PREFIX = indicates where to install STViewer

    CMAKE_BUILD_TYPE = indicates the type of building (Release by default)

    CMAKE_PREFIX_PATH = indicates and extra path to look for packages for example the
    binaries of Qt5.

    DCMAKE_OSX_SYSROOT = provides the path to the MacOS X SDK that is to be used
    eg: Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sd    k/

    DCMAKE_OSX_DEPLOYMENT_TARGET = indicates the target MacOS X version
    eg: 10.7 , 10.8 or 10.9
    
    DSERVER = Build STViewer to connect to the development, local or production servers (this is            optional)

    DCLIENID = In case we want the ST Viewer to access datasets in a database we must
    give the client ID (this is optional)

    DSECRETID = In case we want the ST Viewer to access datasets in a database we must
    give the client password (this is optional) 

*   Build the application

        make -j8 
        make install
        make test
    
* Run the application typing

        STViewer
        
* Alternatively for MAC you can build a DMG bundle 

        make dmg
        
    Then you can use the bundle to install the ST Viewer as an application

###### Ubuntu
* Download and install Qt open source from : http://qt-project.org/downloads
* Issue the following commands

        sudo apt-get install cmake git ubuntu-dev-tools
        sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev

* Follow from the third step in the OSX guideline

###### Windows
* Download and install Qt open source from : http://qt-project.org/downloads
* Install Cygwin, by downloading and executing http://cygwin.com/setup-x86_64.exe

    During the installation, select these additional packages:

    * git (category: Devel)
    * wget (category: Web)
    * openssh (category: Net)

* The CMake included in Cygwin unfortunately doesn’t include
the generator for Nmake (-G "NMake Makefiles" ) so you need to
install CMake for windows : http://www.cmake.org/cmake/resources/software.html

* Install Microsoft Visual Studio (make sure you install the C++ compilers) : http://www.microsoft.com/visualstudio/eng/visual-studio-2013

* Install NSI installers in Windows : http://nsis.sourceforge.net/Download

* Build from the source (Currently Qt does not support Visual Studio 2015)

    It can be built in different ways (we use CygWIN) :
    ###### CYGWIN
    - You can use the Cygwin based script called build_cygwin.sh to build it from the      Cygwin terminal.
    Make sure the script is configured to the paths of your Visual Studio, Qt,
    CMake and other libraries and that the architecture is set to the one in your          system (32 or 64)

    - Make a directory for the script to copy the build artifacts to. For example          ‘st_bin’.

            mkdir st_bin

        (When the build has completed this directory will contain the application and          it’s installer).
        
        Then in a Cygwin terminal, for a production build type:
        
            git clone st_client_repo ~/st_client
            ~/st_client/build_cygwin.sh ~/st_client stclient_production_build st_bin

        or for a development build type:

            ~/st_client/build_cygwin.sh  ~/st_client  stclient_development_build  st_bin

        At the end of the build script, the paths to the executable and to the package
        file are printed to the terminal so you can either execute the ST Viewer or install it.
    

