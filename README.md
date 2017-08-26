# Spatial Transcriptomics Research Viewer

"ADD TRAVIS BUILD STATUS HERE"

The ST viewer is a tool to visualize spatially resolved gene
expression data on top of HE stained tissue figures with the correct
location.

The ST Viewer is cross platform which means that it can
be built and run in OSX, UNIX and WINDOWS. 

The ST viewer allows to interact with the data in real time.
Users can see where specific genes are expressed and how
expressed they are. It has different threshold options, normalization
and many visualization options. It also allows
to select areas of the tissue to obtain gene patterns
to later do DEA or spot classification using machine learning. 

The ST viewer uses the data generated with the ST Pipeline 
https://github.com/SpatialTranscriptomicsResearch/st_pipeline, 
which consist of a matrix of counts in TSV format where genes are rows and spot coordinates 
are columns in the form (1x2) where 1 represents the X coordinate and 2 represents the Y coordinate.
The ST viewer also requires a tissue HE image and an optional 3x3 alignment matrix (to convert
array coordinates to image pixel coordinates in case the coordinates
in input data are not converted already).

The ST viewer allows to pass a spot coordinates file to correct the coordinates
positions or to only show the spots under the tissue. This file is compatible
with the output format of the ST Aligner https://github.com/SpatialTranscriptomicsResearch/st_aligner

If you want to load a dataset you can go to the "Datasets view" and click in the button
"Import dataset" then a dialog form will be shown where you can load the ST data, the image/s
and the alignment matrix. After that you can just double click in the dataset to open it. 
(more detailed information about this in the manual).

Note that the referred 3x3 aligment matrix file must have the following format:

a11 a21 a31 a21 a22 a23 a31 a32 a33

If your HE image is cropped to the array boundaries then no alignment matrix is needed.

You can use our public datasets hosted in http://www.spatialtranscriptomicsresearch.org/
if you want to try the ST Viewer.

## Authors
Read AUTHORS file

## Dependencies
Read DEPENDENCIES file

## Manual
See the document manual_old.pdf for more information on how to use the ST Viewer.
Currently the manual is a bit outdated and a new updated
manual is being created and will be shared soon.

## License
See LICENSE for the license terms and DEPENDENCIES for the 3rd party
libraries that are used in this software.

## Contact
For any question/bugs/feedback you can contact Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

## Binaries
No installers/binaries are provided for now.

## Building

* Download and install Qt open source from http://qt-project.org/downloads

* Download and extract QCustomplot sources from http://qcustomplot.com/ 

* Download and install Armadillo from http://arma.sourceforge.net/

* Download and install R from https://cran.r-project.org/

* Open R and install the following packages (DESeq2, Rtsne and SCRAN)

###### OSX

* Make sure that XCode and XCode Command Line Tools are installed

* Install CMake

* Clone the repository to a specific folder and build the application

        git clone <st_viewer_repo>
        mkdir /path/to/build
        cd /path/to/build
        cmake [-DCMAKE_INSTALL_PREFIX="/usr/local/bin"] \
          [-DCMAKE_BUILD_TYPE="Debug" | "Release"] \
          [-DCMAKE_PREFIX_PATH="/path/to/libraries"] \
          [-DCMAKE_OSX_SYSROOT=”/path/to/macosx.sdk”] \
          [-DCMAKE_OSX_DEPLOYMENT_TARGET=version] \
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
    
* Run the application typing clicking in the app icon that can be found in

        /path/to/build/
        
* Alternatively for MAC you can build a stand alone DMG bundle that you can install and/or distribute

        make dmg
        
    Then you can use the bundle to install the ST Viewer as an application

###### Ubuntu

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

* Install Cygwin, by downloading and executing http://cygwin.com/setup-x86_64.exe

    During the installation, select these additional packages:

    * git (category: Devel)
    * wget (category: Web)

* The CMake included in Cygwin unfortunately doesn’t include
the generator for Nmake (-G "NMake Makefiles" ) so you need to
install CMake for windows : http://www.cmake.org/cmake/resources/software.html

* Install Microsoft Visual Studio (make sure you install the C++ compilers) : http://www.microsoft.com/visualstudio/eng/visual-studio-2013

* Install NSI installers in Windows : http://nsis.sourceforge.net/Download

* Build from the source

    

