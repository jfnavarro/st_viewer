# Spatial Transcriptomics Research Viewer

"ADD TRAVIS BUILD STATUS HERE"

The ST viewer is a tool to visualize spatially resolved gene
expression data on top of cell tissue figures with the correct
location.

The ST Viewer is cross platform which means that it can
be built and run in MAC, UNIX and WINDOWS PCs. 
The ST viewer allows to interact with the data in real time.
Users can see where specific genes are expressed and how
expressed they are. It has different threshold options
and many visualization options. It also allows
to select areas of the tissue to obtain gene patterns
to later do DEA or cell classification. 

The ST viewer uses the data generated with the ST Pipeline https://github.com/SpatialTranscriptomicsResearch/st_pipeline, 
a tissue HE image and an alignment matrix (to convert
array coordinates to image pixel coordinates in case the coordinates
in input data are not converted already).

Currently the ST Viewer works with the deprecated JSON format for the ST data.
The ST Pipeline generates the data in form of a matrix of counts and the next
release of the ST Viewer will work with this format. If you want to convert
the matrix of counts to a JSON file you can use the script matrix_to_json.py in
the ST Analysis package https://github.com/SpatialTranscriptomicsResearch/st_analysis

If you want to load a dataset you can go to the "Datasets view" and click in the button
"Import dataset" then a dialog form will be shown where you can load the ST data, the image/s
and the alignment matrix. After that you can just double click in the dataset to open it. 
(more detailed information about this in the manual).

Alternatively, the ST viewer can access datasets stored in a database trough the ST API https://github.com/SpatialTranscriptomicsResearch/st_api
For that you must have the database server and the RESFull API server up and running
and properly configured and also update the configuration file of the ST Viwer (stviewer.conf)
to the correct network settings. In this case the ST Viewer will require you to log in
with your user credentials and the datasets that you have access to will automatically
be downloaded in the "Datasets view".
(More information about this will be added to the manual soon).

You can use our public datasets hosted in http://www.spatialtranscriptomicsresearch.org/
if you want to try the ST Viewer.

## Authors
Read AUTHORS file

## Dependencies
Read DEPENDENCIES file

## Manual
See MANUAL in the document manual_old.pdf for more information
on how to use the ST Viewer.
Currently the manual is a bit outdated and a new updated
manual is being created but many of the main options
remain the same. 

## License
See LICENSE for the license terms and DEPENDENCIES for the 3rd party
libraries that are used in this software.

## Contact
For any question/bugs/feedback you can contact Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

## Building

###### OSX
* Download and install Qt open source from : http://qt-project.org/downloads

* Download and extract QCustomplot sources from http://qcustomplot.com/ (Remeber to tell CMake where QCustomplot is using CMAKE_PREFIX_PATH

* Issue the following to install the necessary dependencies:
  E.x (Assuming MacPorts is installed, alternatively you can build them from the source or use another package manager)
        
        sudo port install xcode cmake git

    NOTE : Make sure the XCode Command Line Tools are installed
* Clone the repository to a specific folder and build the application

        git clone st_viewer_repo /path/to/source
        mkdir /path/to/build
        cd /path/to/build
        cmake [-DCMAKE_INSTALL_PREFIX="/usr/local/bin"] \
          [-DCMAKE_BUILD_TYPE="Debug" | "Release"] \
          [-DCMAKE_PREFIX_PATH="/path/to/libraries"] \
          [-DCMAKE_OSX_SYSROOT=”/path/to/macosx.sdk”] \
          [-DCMAKE_OSX_DEPLOYMENT_TARGET=version] \
          [-DPUBLICKEY="path_to_ssl_key"] \
          [-DCONFIG_FILE=”path_to_networl_config_file”]
          /path/to/source

    Where : 

    DCMAKE_INSTALL_PREFIX = indicates where to install STViewer

    DCMAKE_BUILD_TYPE = indicates the type of building (Release by default)

    DCMAKE_PREFIX_PATH = indicates and extra path to look for packages for example the
    binaries of Qt5 or the source of QCustomplot.

    DCMAKE_OSX_SYSROOT = provides the path to the MacOS X SDK that is to be used (Only OSX users)
    eg: Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/

    DCMAKE_OSX_DEPLOYMENT_TARGET = indicates the target MacOS X version (Only OSX users)
    eg: 10.7 , 10.8 or 10.9
    
    DPUBLIC_KEY = In case we want the ST Viewer to access datasets in a database we must
    give a public key if the API implements HTTPS (this is optional) 
    
    DCONFIG_FILE = In case we want the ST Viewer to access datasets in a database we must
    give a configuration file with the endpoints and the OAuth access settings.
    The STViewer will load this file in running time after it is installed so you can
    also edit the file after installation (this is optional)

*   Build the application

        make -j8 
    
* Run the application typing

        /path/to/build/STViewer.app/Contents/STViewer
        
* Alternatively for MAC you can build a stand alone DMG bundle that you can install and/or distribute

        make dmg
        
    Then you can use the bundle to install the ST Viewer as an application

###### Ubuntu

* Download and install Qt open source from : http://qt-project.org/downloads

* Download and extract QCustomplot sources from http://qcustomplot.com/ (Remeber to tell CMake where QCustomplot is using CMAKE_PREFIX_PATH

* Issue the following commands (Ubuntu, for Fedora you must use yum)

        sudo apt-get install cmake git ubuntu-dev-tools
        sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev

* Perform the 4th step of the OSX guideline (clone the repo and invoke CMake)

* Then type the following to build and install

        make -j8
        make install
        
* To execute type :
      
        /path/to/bin/STViewer
  
* Alternatively for Linux you can build a stand alone tar package that you can install and/or distribute
  
        make package

###### Windows

There are different ways to build the ST Viewer in Windows.
We use Cygwin but there are probably easier ways to do it. 

* Download and install Qt open source from : http://qt-project.org/downloads

* Download and extract QCustomplot sources from http://qcustomplot.com/ (Remeber to tell CMake where QCustomplot is using CMAKE_PREFIX_PATH 

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

* Install OpenSSL

* Build from the source (Currently Qt does not support Visual Studio 2015)

    - You can use the Cygwin based script called build_cygwin.sh to build it from the Cygwin terminal.
    Make sure the script is configured to the paths of your Visual Studio, Qt and QCustompllot and that the architecture 
    is set to the one in your system (32 or 64) as well as the CMake variables (listed above)

    - Make a directory for the script to copy the build artifacts to. For example ‘st_bin’.

            mkdir st_bin

        (When the build has completed this directory will contain the application and it’s installer).
        
        Then in a Cygwin terminal, for a production build type:
        
            git clone st_client_repo ~/st_client
            ~/st_client/build_cygwin.sh ~/st_client stclient_production_build st_bin

        or for a development build type:

            ~/st_client/build_cygwin.sh  ~/st_client  stclient_development_build  st_bin

        At the end of the build script, the paths to the executable and to the package
        file are printed to the terminal so you can either execute the ST Viewer or install it from
        the installer which can be distributed as well.
    

