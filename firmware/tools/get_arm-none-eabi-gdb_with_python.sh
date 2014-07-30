#!/bin/bash

# ------------------------------------------------------------------------------
# Get arm-none-eabi-gdb with python support
# Should take about 15 minutes
# ------------------------------------------------------------------------------

mkdir gdb-build
cd gdb-build

# Grab the pre-requisites
sudo apt-get install apt-src \
gawk \
gzip \
perl \
autoconf \
m4 \
automake \
libtool \
libncurses5-dev \
gettext \
gperf \
dejagnu \
expect \
tcl \
autogen \
flex \
flip \
bison \
tofrodos \
texinfo \
g++ \
gcc-multilib \
libgmp3-dev \
libmpfr-dev \
debhelper \
texlive \
texlive-extra-utils

# Grab the sources - UPDATE WITH LATEST SOURCES
#wget https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q2-update/+download/gcc-arm-none-eabi-4_8-2014q2-20140609-src.tar.bz2

# Extract
tar xjf gcc*
cd gcc*

# Extract gdb
cd src
tar xf gdb*
cd gdb*

# Configure
host_arch=`uname -m | sed 'y/XI/xi/'`
PKGVERSION="GNU Tools for ARM Embedded Processors --with-python=yes"

./configure --target=arm-none-eabi \
    --disable-nls \
    --disable-sim \
    --with-libexpat \
    --with-python=yes \
    --with-lzma=no \
    --build=$host_arch-linux-gnu --host=$host_arch-linux-gnu \
    --with-pkgversion="$PKGVERSION"

# Make (j = Ncores + 1)
make -j3
sudo make install

# Cleanup
cd ../../../..
#sudo rm -rf gdb-build
