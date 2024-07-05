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
    libharfbuzz-icu0 \
    libhyphen-dev \
    libicu-dev \
    libjpeg-dev \
    libpng-dev \
    libxslt1-dev \
    libwebpdemux2 \
    libwoff1 \

# Install Qt
sudo apt-get install -y \
    libqt6core5compat6-dev \
    libqt6sensors6-dev \
    qt6-base-dev \
    qt6-declarative-dev \
    qt6-multimedia-dev \
    qt6-positioning-dev \

# Qt executables go through "qtchooser" by default. It is broken. Just replace the "qtchooser" versions with a link to the executable we actually want to run
# If you see an error of "thng: could not find a Qt installation of ''" then that means that `thng` is another executable that needs the following treatment.
sudo rm /usr/bin/qmake
sudo ln -s /usr/lib/qt6/bin/qmake /usr/bin/qmake
sudo rm /usr/bin/moc
sudo ln -s /usr/lib/qt6/libexec/moc /usr/bin/moc
sudo rm /usr/bin/uic
sudo ln -s /usr/lib/qt6/libexec/uic /usr/bin/uic

# Alias python3 to python so the build script can find it
sudo ln -s /usr/bin/python3 /usr/bin/python

# Download our prebuilt version of QtWebkit
curl -L https://github.com/constructpm/qtwebkit-build/releases/download/v6.212.0-1/qtwebkit-ee690e4-cpp17-ubuntu-22.04-x64.tar.gz | sudo tar xvJ -C /usr/include/x86_64-linux-gnu

# Move the files we downloaded so their locations match the locations used by the other Qt libraries we installed earlier
sudo mv /usr/include/x86_64-linux-gnu/qt6/include/* /usr/include/x86_64-linux-gnu/qt6/
sudo rmdir /usr/include/x86_64-linux-gnu/qt6/include/

sudo mv /usr/include/x86_64-linux-gnu/qt6/lib/cmake/* /usr/lib/x86_64-linux-gnu/cmake/
sudo rmdir /usr/include/x86_64-linux-gnu/qt6/lib/cmake/
sudo mv /usr/include/x86_64-linux-gnu/qt6/lib/pkgconfig/* /usr/lib/x86_64-linux-gnu/pkgconfig/
sudo rmdir /usr/include/x86_64-linux-gnu/qt6/lib/pkgconfig/
sudo mv /usr/include/x86_64-linux-gnu/qt6/lib/* /usr/lib/x86_64-linux-gnu/
sudo rmdir /usr/include/x86_64-linux-gnu/qt6/lib/
