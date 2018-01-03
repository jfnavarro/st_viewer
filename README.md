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
which consist of a matrix of counts in TSV format where genes are 
columns and spot coordinates are rows in the fllowing form: 

eg. 1x2 

Where 1 represents the X coordinate and 2 represents the Y coordinate.

The ST viewer also requires a tissue HE image and an optional 3x3 alignment matrix (to convert
array coordinates to image pixel coordinates).

Note that the referred 3x3 aligment matrix file must have the following format:

	a11 a21 a31 a21 a22 a23 a31 a32 a33

If the HE image is cropped to the array boundaries then no alignment matrix is needed.

The ST viewer allows to pass a spot coordinates file to correct the coordinates
positions and/or to only show the spots under the tissue. This file is compatible
with the output format of the ST Spot Detector https://github.com/SpatialTranscriptomicsResearch/st_spot_detector

If you want to load a dataset you can go to the "Datasets view" and click in the button
"Import dataset" then a dialog form will be shown where you can load the matrix of counts, the HE
image and other files. You can also download a dataset automatically
if its files are inside a folder with the option "Load folder" or you
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
(more detailed information about this in the wiki).

You can use our public datasets hosted in http://www.spatialtranscriptomicsresearch.org/
if you want to try the ST Viewer.

## Authors
Read AUTHORS file

## Dependencies
Read DEPENDENCIES file

## Manual
See Wiki

## License
See LICENSE for the license terms and DEPENDENCIES for the 3rd party
libraries that are used in this software.

## Contact
For any question/bugs/feedback you can contact Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

## Binaries
No installers/binaries are provided for now.

## Building/Installing

* Download and install CMake 3.7.2 (https://cmake.org/files/v3.7/cmake-3.7.2.tar.gz) (Latest versions of CMake 3.9.x and 3.8.x have compatiblity issues with Qt so I recommend to download and install the version 3.7.2 or a previous one)

	Tips (for Linux and OSX; first download the file to a folder then in that folder open a terminal):
	
		tar -xvzf cmake-3.7.2.tar
		cd cmake-3.7.2
		./configure
		make -j4
		sudo make install

* Download and install Qt open source from http://qt-project.org/downloads (Choose Desktop application and Open Source and then use the defaultsettings and location)

* Downloan open and extract QCustomplot from http://www.qcustomplot.com/release/1.3.2/QCustomPlot.tar.gz

* Download and compile Armadillo from http://arma.sourceforge.net/download.html
	
	NOTE (Armadillo only needs to be built for Linux and OSX):
	
	* Download the latest stable release and then open a terminal and type (x.xxx.x refers to the Armadillo version):
	
			tar -xvf armadillo-x.xxx.x.tar.xz
			cd armadillo-x.xxx.x
			./configure
			make


* Download and install R from https://cran.r-project.org/ (in case you do not have it already)

* Download and install Rtools (Only for Windows) from https://cran.r-project.org/bin/windows/Rtools/

* Open R and install the following packages (Rcpp, RInside, RcppArmadillo, DESeq2, Rtsne and SCRAN)

        source("https://bioconductor.org/biocLite.R")
        biocLite("DESeq2")
        biocLite("scran")
        biocLite("Rtsne")
        biocLite("RInside")
        biocLite("Rcpp")
        biocLite("RcppArmadillo")

###### OSX

* Make sure that XCode and XCode Command Line Tools are installed (check by typing "xcode-select" on a terminal)
  If needed you can install them from the Apple store (https://developer.apple.com/xcode/). 
  I recommend to update to the latest version of XCode.  

* Clone the repository to a specific folder and build the application

        git clone https://github.com/jfnavarro/st_viewer.git
        mkdir st_viewer_build
        cd st_viewer_build
        cmake -DCMAKE_BUILD_TYPE="Release" -DCMAKE_PREFIX_PATH="/path/to/libraries" -DCMAKE_OSX_SYSROOT="/path/to/macosx.sdk" -DCMAKE_OSX_DEPLOYMENT_TARGET=version ../st_viewer

    Where : 

    DCMAKE_BUILD_TYPE = indicates the type of building ("Debug" or "Release" which is the default)

    DCMAKE_PREFIX_PATH = the path to where Qt, armadillo and qcustomplot are installed
    
    eg: "/Users/username/Qt/5.9.2/clang_64;/path/to/qcustomplot;/Users/username/armadillo"

    DCMAKE_OSX_SYSROOT = provides the path to the MacOS X SDK that is to be used (Only OSX users)
    
    eg: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk/

    DCMAKE_OSX_DEPLOYMENT_TARGET = use 10.12 or 10.11 

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
        cmake -DCMAKE_BUILD_TYPE="Release" -DCMAKE_PREFIX_PATH="/path/to/libraries" ../st_viewer

    Where : 

    DCMAKE_BUILD_TYPE = indicates the type of building ("Debug" or "Release" which is the default)

    DCMAKE_PREFIX_PATH = the path to where Qt, armadillo and qcustomplot are installed
    
    eg: "/Users/username/Qt/5.9.2/gcc;/Users/username/qcustomplot;/Users/username/armadillo"

* Then type the following to build and install

        make -j4
        make install
        
* To execute type :
      
        STViewer
        or
        /path/to/bin/STViewer

###### Windows 
 
* Download and install Git for windows from https://git-scm.com/downloads
 
* Open the GIT terminal and clone the repository :
 
	git clone https://github.com/jfnavarro/st_viewer.git
	
* Make sure that your PATH environment variable contains Rtools' bin, Rtools MinGW's bin and R's bin paths

* Make sure that you do not have another MinGW in your PATH variable

* Make sure to have a environment variable called R_HOME pointing to where R is installed (its root folder)
 
* Open a windows terminal (cmd.exe)

* Type the following 

		mkdir build
		cd build
		cmake -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_PREFIX_PATH="/path/to/libraries" -DARMADILLO_PATH="C:\\armadillo" ../st_viewer
	
  Where:

  DCMAKE_BUILD_TYPE = indicates the type of building ("Debug" or "Release" which is the default)

  DCMAKE_PREFIX_PATH = the path to where Qt and qcustomplot are installed
    
    eg: "C:\Qt\5.9.1\mingw53_32;C:\qcustomplot"
    
  DARMADILLO_PATH = indicates where armadillo was extracted
  
    eg: "C:\\armadillo"
    
  ../st_viewer = is the path where the ST Viewer was cloned/downloaded
		
* Now build and install the ST Viewer by typing:

		mingw32-make install
		
* By default the ST Viewer will be installed in "Program Files" but that can be changed 
with the CMake variable -DCMAKE_INSTALL_PREFIX

 
 	


    

