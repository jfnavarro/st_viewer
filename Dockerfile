# To run it locally you need
# 1) Allow local appliaction to use your Xserver
# 
#     xhost +local:root
# 
# 2) mount the display and the temporary directory 
# 
# 
#     docker run -d -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:rw st_viewer
#

FROM ubuntu:17.10

RUN apt-get update && apt-get install -y \
	autoconf \
	automake \
	make \
	g++ \
	gcc \
	build-essential \ 
	git \
	ubuntu-dev-tools \
	libglu1-mesa-dev \
	freeglut3-dev \
	mesa-common-dev \
	wget \
	libfontconfig1 \
	qt5-default \
	qtbase5-dev \
	libqt5charts5-dev \
	libqt5svg5-dev \
	libarmadillo-dev \
	cmake \
	&& rm -rf /var/lib/apt/lists/*

# Install Qt (Faking display)
# Installed via Apt

# DONWLOAD the QCustomplot
RUN wget http://www.qcustomplot.com/release/1.3.2/QCustomPlot.tar.gz && \
    mkdir /opt/QCustomPlot && tar xf QCustomPlot.tar.gz -C /opt/QCustomPlot

WORKDIR /opt/
RUN mkdir /opt/st_viewer
WORKDIR /opt/st_viewer 
ADD . . 
RUN mkdir st_viewer_build
WORKDIR /opt/st_viewer/st_viewer_build

# Optimize this in one line.
RUN cmake -DCMAKE_BUILD_TYPE="Release" -DCMAKE_PREFIX_PATH="/opt/QCustomPlot/qcustomplot" .. \
	&& make -j4
#	&& make install

CMD ["/opt/st_viewer/st_viewer_build/STViewer"]
