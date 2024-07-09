#!/bin/bash

sudo apt-get -y update 

# Enable tab autocomplete in terminal
sudo apt install bash-completion

# Install tools/libraries required to build
sudo apt-get install -y \
    gyp \
    libdbus-1-dev \
    libegl1-mesa-dev \
    libglib2.0-dev \
    libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libharfbuzz-icu0 \
    libhyphen-dev \
    libicu-dev \
    libjpeg-dev \
    libpng-dev \
    libxslt1-dev \
    libwebpdemux2 \
    libwoff1 \
    libxkbcommon-dev \

# Alias python3 to python so the build script can find it
sudo ln -s /usr/bin/python3 /usr/bin/python

# Download our prebuilt version of Qt
curl -L https://github.com/constructpm/qt-build/releases/download/v6.7.1-1/qt-6.7.1-cpp17-ubuntu-22.04-x64.tar.gz | sudo tar xvJ -C /opt
# Download our prebuilt version of QtWebkit
curl -L https://github.com/constructpm/qtwebkit-build/releases/download/v6.212.0-1/qtwebkit-ee690e4-cpp17-ubuntu-22.04-x64.tar.gz | sudo tar xvJ -C /opt
