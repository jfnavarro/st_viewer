# Spatial Transcriptomics Viewer

The ST viewer is a tool that can be used to visualize and analize 
spatially resolved gene expression data in 3D (consecutive sections) 
or in 2D (with the HE stained tissue image).

The ST Viewer is cross platform which means that it can
be built and run in OSX, LINUX and WINDOWS. 

The ST viewer allows interaction with the data in real time.
Users can see where specific genes are expressed and how
expressed they are. It has different threshold, normalization
and visualization options. It also allows
to select areas of the tissue to obtain gene patterns
to later do DEA or spot classification using machine learning. 

The ST viewer requires as input a matrix of counts in TSV format where genes are 
columns and spot ids are rows.

The ST viewer requieres also a TSV file with spot coordinates
with one of the following formats:

	SPOT_X SPOT_Y chip_x chip_y pixel_x pixel_Y

or

	SPOT chip_x chip_y pixel_x pixel_y

or

	SPOT pixel_x pixel_y
	
or

	SPOT X Y Z 

The first listed format is compatible with the 
[ST Spot Detector](https://github.com/SpatialTranscriptomicsResearch/st_spot_detector)
and the last format is designed for 3D datasets. 

For 2D datasets the HE stained image of the tissue section is required. 

For 3D datasets a 3D Mesh object can be provided. 

## Loading a dataset

If you want to load a dataset you can go to the "Datasets view" and click in the button
"Import dataset" then a dialog form will be shown where you can load the matrix of counts, the HE
image and other files. You can also upload a dataset automatically
if its files are inside a folder with the option "Load folder" or you
can use a meta-file to load a dataset. The meta-file must describe where
all the dataset's files are and it should have the following JSON format:

	{
        	"name": "test",
        	"comments": "test_comments",
		"data": "/Users/user/test_dataset/stdata.tsv",
		"image": "/Users/user/test_dataset/image.jpg",
		"coordinates": "/Users/user/test_dataset/spots.txt",
	}

After that you can just double click in the dataset to open it. 
(more detailed information about this in the wiki).

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
For any question/bugs/feedback you can contact Jose Fernandez Navarro <jc.fernandez.navarro@gmail.com>

## Binaries (Install from binaries)
Binaries (installers) for MAC and Windows are provided in the "Releases" tab. 
To install the ST Viewer using the installers follow these intructions: 

###### OSX
The binary provided for MAC requires OSX 10.5 or bigger. 

* Download the installer (DMG) open it and drag the ST Viewer icon to Applications and then 
the ST Viewer will be installed in your system. 

###### Windows 
		
* Download the Windows installer double click on it and follow the instructions, once done the ST Viewer
will be installed in your system. 

## Docker container

This has been tested on a Linux Ubuntu 17.04 as host system.

Build the image:

	docker build . -t st_viewer
	
Launch the image, mounting also the volume where you have the dataset. For example if your dataset is in 
`/home/user/STDatasets/` you need to launch with the option `-v /home/user/STDatasets:/STDatasets` which you can then 
find, via the fileBrowser of the Viewer in the directory `/STDatasets`. 
Note that you need to allow the root user to use your Display to see the Viewer:

	xhost +local:root
	
Then launch the image according to where your file are located.

	docker run -d -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:rw -v /home/user/STDatasets:/STDatasets st_viewer
	

## Building from the source 

* Download and install CMake

* Download and install Qt open source from http://qt-project.org/downloads (Choose Desktop application and Open Source and then use the defaultsettings and location). For Windows you must choose the mingw64 option and include QT Charts for every platform. 

* Download and extract QCustomplot from http://www.qcustomplot.com/release/1.3.2/QCustomPlot.tar.gz

* Download and build Armadillo from http://arma.sourceforge.net/download.html
	
	NOTE (Armadillo only needs to be built for Linux and OSX, for Windows you just need to download and extract it to a folder):
	
	* Download the latest stable release and then open a terminal and type (x.xxx.x refers to the Armadillo version):
	
			tar -xvf armadillo-x.xxx.x.tar.xz
			cd armadillo-x.xxx.x
			./configure
			make

###### OSX

* Make sure that XCode and XCode Command Line Tools are installed (check by typing "xcode-select" on a terminal)
  If needed to you can install them from the Apple store (https://developer.apple.com/xcode/). 
  I recommend to update to the latest version of XCode.
  
* Make sure to have installed OpenMP, for example with "brew install libomp"

* Clone the repository to a specific folder and build the application

        git clone https://github.com/jfnavarro/st_viewer.git
        mkdir st_viewer_build
        cd st_viewer_build
        cmake -DCMAKE_BUILD_TYPE="Release" -DCMAKE_PREFIX_PATH="/path/to/libraries" -DCMAKE_OSX_SYSROOT="/path/to/macosx.sdk" -DCMAKE_OSX_DEPLOYMENT_TARGET=version ../st_viewer

    Where : 

    DCMAKE_BUILD_TYPE = indicates the type of building ("Debug" or "Release" which is the default)

    DCMAKE_PREFIX_PATH = the path to where Qt, armadillo and qcustomplot are installed
    
    eg: "/Users/username/Qt/5.14.1/clang_64;/path/to/qcustomplot;/Users/username/armadillo"

    DCMAKE_OSX_SYSROOT = provides the path to the MacOS X SDK that is to be used (Only OSX users)
    
    eg: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk/

    DCMAKE_OSX_DEPLOYMENT_TARGET = use 10.13 or 10.14 

*   Compile the application

        make -j4 
    
* Run the application by clicking on the STViewer.app icon that can be found in

        /st_viewer_build

###### Linux

If you are on a Ubuntu, you can use the `ubuntu_dep_and_compile.sh` to install the dependencies and
  compile the code with:
  
	sudo ubuntu_dep_and_compile.sh

Or you can follow the process outlined below.

* Issue the following commands (Ubuntu, for Fedora you must use yum)

        sudo apt-get install git ubuntu-dev-tools
        sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev

* Clone the repository to a specific folder and build the application

        git clone https://github.com/jfnavarro/st_viewer.git
        mkdir st_viewer_build
        cd st_viewer_build
        cmake -DCMAKE_BUILD_TYPE="Release" -DCMAKE_PREFIX_PATH="/path/to/libraries" ../st_viewer

    Where : 

    DCMAKE_BUILD_TYPE = indicates the type of building ("Debug" or "Release" which is the default)

    DCMAKE_PREFIX_PATH = the path to where Qt, armadillo and qcustomplot are installed
    
    eg: "/Users/username/Qt/5.14.1/gcc;/Users/username/qcustomplot;/Users/username/armadillo"

* Then type the following to build and install

        make -j4
        make install
        
* To execute type :
      
        STViewer
        or
        /path/to/bin/STViewer
	
Note that for Linux you may want to update your LD_LIBRARY_PATH variable to contain the R and QT paths

	eg: LD_LIBRARY_PATH=/usr/lib/R/lib/:/home/username/Qt/5.14.1/gcc_64/lib

###### Windows 
 
* Download and install Git for windows from https://git-scm.com/downloads
 
* Open the GIT terminal and clone the repository :
 
		git clone https://github.com/jfnavarro/st_viewer.git
	
* Install MinGW64 with multi-threading (OpenMP and threads) support (http://mingw-w64.org/doku.php)

* Make sure that your PATH environment variable contains MinGW's paths

		eg PATH=C:\MinGW;C:\MinGW\bin;C:\MinGW\lib

* Make sure that you do not have another MinGW in your PATH variable
 
* Open a windows terminal (cmd.exe)

* Type the following 

		mkdir build
		cd build
		cmake -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_PREFIX_PATH="/path/to/libraries" -DARMADILLO_PATH="C:\\armadillo" ../st_viewer
	
  Where:

  DCMAKE_BUILD_TYPE = indicates the type of building ("Debug" or "Release" which is the default)

  DCMAKE_PREFIX_PATH = the path to where Qt and qcustomplot are installed
    
    eg: "C:\Qt\5.14.1\mingw53_64;C:\qcustomplot"
    
  DARMADILLO_PATH = indicates where armadillo was extracted
  
    eg: "C:\\armadillo"
    
  ../st_viewer = is the path where the ST Viewer was cloned/downloaded
		
* Now build and install the ST Viewer by typing (you must run this as an administrator):

		mingw32-make install
		
* By default the ST Viewer will be installed in "Program Files" but that can be changed 
with the CMake variable -DCMAKE_INSTALL_PREFIX (it is recommended to install the ST Viewer as an administrator)
