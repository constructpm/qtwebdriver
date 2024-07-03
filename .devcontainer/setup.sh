#!/bin/bash

sudo apt-get -y update 

# Enable tab autocomplete in terminal
sudo apt install bash-completion

# Install tools/libraries required to build
sudo apt-get install -y \
    gyp \
    libegl1-mesa-dev \
    libglib2.0-dev \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libhyphen-dev \
    libicu-dev \
    libjpeg-dev \
    libpng-dev \
    libxslt1-dev \

# Install Qt
sudo apt-get install -y \
    qtbase5-dev \
    qtdeclarative5-dev \
    qtmultimedia5-dev \
    qtpositioning5-dev \
    libqt5sensors5-dev \
    libqt5webchannel5 \
    qt5-qmake \

# Alias python3 to python so the build script can find it
sudo ln -s /usr/bin/python3 /usr/bin/python

# Download our prebuilt version of QtWebkit
curl -L https://github.com/constructpm/qtwebkit-build/releases/download/v5.212.0-1/qtwebkit-d1c854e-cpp17-ubuntu-22.04-x64.tar.gz | sudo tar xvJ -C /usr/include/x86_64-linux-gnu

# Move the files we downloaded so their locations match the locations used by the other Qt libraries we installed earlier
sudo mv /usr/include/x86_64-linux-gnu/qt5/include/* /usr/include/x86_64-linux-gnu/qt5/
sudo mv /usr/include/x86_64-linux-gnu/qt5/lib/* /usr/lib/x86_64-linux-gnu/
sudo mv /usr/include/x86_64-linux-gnu/qt5/lib/cmake/* /usr/lib/x86_64-linux-gnu/cmake/
sudo mv /usr/include/x86_64-linux-gnu/qt5/lib/pkgconfig/* /usr/lib/x86_64-linux-gnu/pkgconfig/

sudo rmdir /usr/include/x86_64-linux-gnu/qt5/include/
sudo rmdir /usr/include/x86_64-linux-gnu/qt5/lib/cmake/
sudo rmdir /usr/include/x86_64-linux-gnu/qt5/lib/pkgconfig/
sudo rmdir /usr/include/x86_64-linux-gnu/qt5/lib/
