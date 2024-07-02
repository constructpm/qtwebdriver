# Description

This Repo contains a fork of the QtWebDriver source code. We need this because the automated Robot tests in boron use it (and maybe a few other things?).

This repo contains:
- The source code for qtwebkit (originally from cisco)
- A devcontainer that can be used for development/building of the source
- A github action that automatically builds the code on pull requests, and builds a release when a version tag (`v1` or equivalent) is pushed.

The intended usage is:
1. make any changes you need
2. (optional) use the devcontainer to confirm the builds succeed locally (open the dev container and run `build.sh`)
3. create PR and get it approved
4. push a version tag to create a release
5. use the release build anywhere you need a qtwebdriver build

To use the release, you can download it with a command that looks like the following:
```bash
curl -L https://github.com/constructpm/qtwebdriver/releases/download/v5.15.3-dev-1/qtwebdriver-5.15.8-lts-lgpl-cpp17-ubuntu-22.04-x64.tar.gz | sudo tar xvJ -C /opt
```

This process is intended to match the process used to get a build of Qt from [qt-build](https://github.com/constructpm/qt-build), and a build of QtWebkit from [qtwebkit-build](https://github.com/constructpm/qtwebkit-build) do.
It replaces the old process of building the binary manually and placing it in a shared storage area.

# Old Readme

## Synopsis
QtWebDriver is a WebDriver implementation for Qt.

It can be used to perform automated Selenium testing of applications based on:
* QtWebkit
* QWidgets
* QQuick1 (Qt4) or QQuick2 (Qt5)  

If you hadn't used Selenium for automated testing, you may also find this links helpful:
* https://github.com/seleniumhq/selenium
* http://docs.seleniumhq.org/  

## Build and run
* The build instructions are detailed in the wiki: https://github.com/cisco-open-source/qtwebdriver/wiki/Build-And-Run
* Release notes and pre-built binaries are in the Releases section: https://github.com/cisco-open-source/qtwebdriver/releases

## Other links
* An example how to customize QtWebDriver in `src/Test/main.cc`   
* [Doxygen](http://cisco-open-source.github.io/qtwebdriver/html)  
* [Wiki](https://github.com/cisco-open-source/qtwebdriver/wiki)  
* Mailing list: qtwebdriver-users@external.cisco.com

## License
LGPLv2.1
