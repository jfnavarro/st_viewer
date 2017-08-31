# Spatial Transcriptomics Research Viewer

"ADD TRAVIS BUILD STATUS HERE"

The ST viewer is a tool that visualizes spatially resolved gene
expression data on top of HE stained tissue figures with the correct
location.

The ST Viewer is cross platform which means that it can
be built and run in OSX, LINUX and WINDOWS. 

The ST viewer allows to interact with the data in real time.
Users can see where specific genes are expressed and how
expressed they are. It has different threshold, normalization
and visualization options. It also allows
to select areas of the tissue to obtain gene patterns
to later do DEA or spot classification using machine learning. 

The ST viewer uses the data generated with the ST Pipeline 
https://github.com/SpatialTranscriptomicsResearch/st_pipeline, 
which consist of a matrix of counts in TSV format where genes are rows and spot coordinates 
are columns in the fllowing form: 

eg. 1x2 

Where 1 represents the X coordinate and 2 represents the Y coordinate.

The ST viewer also requires a tissue HE image and an optional 3x3 alignment matrix (to convert
array coordinates to image pixel coordinates).

Note that the referred 3x3 aligment matrix file must have the following format:

	a11 a21 a31 a21 a22 a23 a31 a32 a33

If the HE image is cropped to the array boundaries then no alignment matrix is needed.

The ST viewer allows to pass a spot coordinates file to correct the coordinates
positions and/or to only show the spots under the tissue. This file is compatible
with the output format of the ST Aligner https://github.com/SpatialTranscriptomicsResearch/st_aligner

If you want to load a dataset you can go to the "Datasets view" and click in the button
"Import dataset" then a dialog form will be shown where you can load the ST data, the HE
image and other files. You can also download a dataset automatically
if its files are inside a folder with the option "Load folder" and you
can use a meta-file to load a dataset. The meta-file must describe where
all the dataset's files are and it should have the following JSON format:

	{
        	"name": "test",
        	"tissue": "test_tissue",
        	"species": "test_species",
        	"comments": "test_comments",
		"data": "/Users/user/test_dataset/stdata.tsv",
		"image": "/Users/user/test_dataset/image.jpg",
		"aligment": "/Users/user/test_dataset/alignment.txt",
		"coordinates": "/Users/user/test_dataset/spots.txt",
		"spike_ins": "",
		"size_factors": ""
	}

After that you can just double click in the dataset to open it. 
(more detailed information about this in the soon to come manual).

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

## Building/Installing

* Download and install CMake 3.8.2 (https://cmake.org/download/) (Latest versions of CMake 3.9.x have compatiblity issues with Qt so I recommend to download and install the version 3.8.2 or a previous one)

* Download and install Qt open source from http://qt-project.org/downloads (use default settings and location)

* Download and extract QCustomplot from http://www.qcustomplot.com/release/1.3.2/QCustomPlot.tar.gz

* Download and compile Armadillo from http://arma.sourceforge.net/download.html
	
	Tips (Armadillo only needs to be build in Linux and OSX):
	
	* Download the latest stable release and then on a terminal type:
	
	        		tar -xvf armadillo-x.xxx.x.tar.xz
				cd armadillo-x.xxx.x
				./configure
				make

* Download and install R from https://cran.r-project.org/ (in case you do not have it already)

* Open R and install the following packages (Rcpp, RInside, RcppArmadillo, DESeq2, Rtsne and SCRAN)

        source("https://bioconductor.org/biocLite.R")
        biocLite("DESeq2")
        biocLite("scran")
        install.packages("Rtsne")
        install.packages("RInside")
        install.packages("Rcpp")
        install.packages("RcppArmadillo")

###### OSX

* Make sure that XCode and XCode Command Line Tools are installed (check by typing "xcode-select" on a terminal)
  If needed you can install them from the Apple store. 

* Clone the repository to a specific folder and build the application (

        git clone https://github.com/jfnavarro/st_viewer.git
        mkdir st_viewer_build
        cd st_viewer_build
        cmake [-DCMAKE_INSTALL_PREFIX="/usr/local/bin"] \
          [-DCMAKE_BUILD_TYPE="Debug" | "Release"] \
          [-DCMAKE_PREFIX_PATH="/path/to/libraries"] \
          [-DCMAKE_OSX_SYSROOT=”/path/to/macosx.sdk”] \
          [-DCMAKE_OSX_DEPLOYMENT_TARGET=version] \
          [-DQCUSTOMPLOT_PATH="/path/to/qcustomplot"] \
          ../st_viewer

    Where : 

    DCMAKE_INSTALL_PREFIX = indicates where to install STViewer ("/usr/local/bin" by default)

    DCMAKE_BUILD_TYPE = indicates the type of building ("Debug" or "Release" which is the default)

    DCMAKE_PREFIX_PATH = the path to where Qt and armadillo are installed
    
    eg: "/Users/username/Qt/5.9/clang_64;/Users/username/armadillo"
    
    DQCUSTOMPLOT_PATH = the path where QCustomplot was extracted
    
    eg: "/Users/username/qcustomplot"

    DCMAKE_OSX_SYSROOT = provides the path to the MacOS X SDK that is to be used (Only OSX users)
    
    eg: Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk/

    DCMAKE_OSX_DEPLOYMENT_TARGET = indicates the target MacOS X version (Only OSX users)
    
    eg: 10.7, 10.8, 10.9, 10.11, etc ...

*   Compile the application

        make -j4 
    
* Run the application by clicking on the STViewer.app icon that can be found in

        /st_viewer_build

###### Linux

* Issue the following commands (Ubuntu, for Fedora you must use yum)

        sudo apt-get install cmake git ubuntu-dev-tools
        sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev

* Clone the repository to a specific folder and build the application

        git clone https://github.com/jfnavarro/st_viewer.git
        mkdir st_viewer_build
        cd st_viewer_build
        cmake [-DCMAKE_INSTALL_PREFIX="/usr/local/bin"] \
          [-DCMAKE_BUILD_TYPE="Debug" | "Release"] \
          [-DCMAKE_PREFIX_PATH="/path/to/libraries"] \
          [-DQCUSTOMPLOT_PATH="/path/to/qcustomplot"] \
          ../st_viewer

    Where : 

    DCMAKE_INSTALL_PREFIX = indicates where to install STViewer ("/usr/local/bin" by default)

    DCMAKE_BUILD_TYPE = indicates the type of building ("Debug" or "Release" which is the default)

    DCMAKE_PREFIX_PATH = the path to where Qt and armadillo are installed
    
    eg: "/Users/username/Qt/5.9/gcc;/Users/username/armadillo"
    
    DQCUSTOMPLOT_PATH = the path where QCustomplot was extracted
    
    eg: "/Users/username/qcustomplot"

* Then type the following to build and install

        make -j4
        make install
        
* To execute type :
      
        STViewer
        or
        /path/to/bin/STViewer

###### Windows (Visual Studio)

* Download and install Visual Studio (2013 is recommended) 
 
* Download and install Git for windows from https://git-scm.com/downloads
 
* Open the GIT terminal and clone the repository :
 
	git clone https://github.com/jfnavarro/st_viewer.git
 
* Open the CMake GUI and add the following entries(variables) :
 
 	- QCUSTOMPLOT_PATH = C:\qcustomplot // or the folder where you extracted it
	- ARMADILLO_PATH = C:\armadillo     // or the folder where you extracted it
	- CMAKE_INSTALL_PREFIX = C:\Qt\5.9.1\msvc2013_64 \\ or the Qt version that you installed
	
* Add the source diretory where the st_viewer was cloned
 
* Add the build directory for example C:\st_viewer_build
 
* Click on Configure and Generate
 
* Click on Open Project (Visual Studio will open)
 
* Click on BUILD ALL 

* The ST Viewer .exe binary will be present in the build directory
 
###### Windows (CygWin)

* Download and install CygWin from https://cygwin.com/install.html
 
* Open the CygWin terminal and clone the repository :
 
 		git clone https://github.com/jfnavarro/st_viewer.git
	
* Create a build directory :

		mkdir st_viewer_build
	
* Open the build_cygwin.sh script present in st_viewer and update the path of QT, QCUSTOMPLOT_PATH and ARMADILLO_PATH variables

* Run the script (pass the build directory as a parameter)

		sh st_viewer/build_cygwin.sh st_viewer st_viewer_build
	
* The ST Viewer .exe binary will be present in the build directory
 	


    

