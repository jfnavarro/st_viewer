packagelist=(
	autoconf
	automake
	make
	g++
	gcc
	build-essential 
	git
	ubuntu-dev-tools
	libglu1-mesa-dev
	freeglut3-dev
	mesa-common-dev
	wget
	libfontconfig1
	qt5-default
	qtbase5-dev
	libqt5charts5-dev
	libqt5svg5-dev
	libarmadillo-dev
	r-base
	r-cran-rcpparmadillo
)

apt-get update && apt-get install -y ${packagelist[@]}
	
	
wget https://cmake.org/files/v3.7/cmake-3.7.2.tar.gz && \
    tar -xvzf cmake-3.7.2.tar.gz && \
    cd cmake-3.7.2 && \
    ./configure && \
    make -j4 && \
    make install

wget http://www.qcustomplot.com/release/1.3.2/QCustomPlot.tar.gz && \
    mkdir /opt/QCustomPlot && tar xf QCustomPlot.tar.gz -C /opt/QCustomPlot
    
echo "r <- getOption('repos'); r['CRAN'] <- 'http://cran.us.r-project.org'; options(repos = r);" > ~/.Rprofile
Rscript -e "source('https://bioconductor.org/biocLite.R'); biocLite('DESeq2'); biocLite('scran'); biocLite('Rtsne')"
Rscript -e "install.packages('RInside')"

mkdir st_viewer_build \
	&& cd st_viewer_build\ 
	&& cmake -DCMAKE_BUILD_TYPE="Release" -DCMAKE_PREFIX_PATH="/opt/QCustomPlot/qcustomplot" .. \
	&& make -j4

ln -sv `pwd`/STViewer /usr/local/bin

