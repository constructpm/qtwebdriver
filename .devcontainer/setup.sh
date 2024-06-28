#!/bin/bash

sudo apt-get -y update 

# Enable tab autocomplete in terminal
sudo apt install bash-completion

# Install tools required to build
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

# Alias python3 to python so the build script can find it
sudo ln -s /usr/bin/python3 /usr/bin/python

# Download our prebuilt version of Qt
curl -L https://github.com/constructpm/qt-build/releases/download/v5.15.8-lts-lgpl-1/qt-5.15.8-lts-lgpl-cpp17-ubuntu-22.04-x64.tar.gz | sudo tar -xJC /opt
curl -L https://github.com/constructpm/qtwebkit-build/releases/download/v5.212.0-1/qtwebkit-d1c854e-cpp17-ubuntu-22.04-x64.tar.gz | sudo tar xvJ -C /opt
