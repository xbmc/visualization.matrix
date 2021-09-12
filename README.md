# Matrix themed visualization addon for Kodi (visualization.matrix)

![icon](visualization.matrix/resources/icon.jpg)

This is a [Kodi](https://kodi.tv) visualization addon.

[![License: GPL-2.0-or-later](https://img.shields.io/badge/License-GPL%20v2+-blue.svg)](LICENSE.md)
[![Build Status](https://dev.azure.com/teamkodi/binary-addons/_apis/build/status/xbmc.visualization.matrix?branchName=Matrix)](https://dev.azure.com/teamkodi/binary-addons/_build/latest?definitionId=72&branchName=Matrix)
[![Build Status](https://jenkins.kodi.tv/view/Addons/job/xbmc/job/visualization.matrix/job/Matrix/badge/icon)](https://jenkins.kodi.tv/blue/organizations/jenkins/xbmc%2Fvisualization.matrix/branches/)
<!--[![Build Status](https://ci.appveyor.com/api/projects/status/github/xbmc/visualization.matrix?branch=Matrix&svg=true)](https://ci.appveyor.com/project/xbmc/visualization-matrix?branch=Matrix)-->

### Screenshot


![fanart](visualization.matrix/resources/fanart.jpg)

### Build instructions for Linux

The following instructions assume you will have built Kodi already in the `kodi-build` directory 
suggested by the README.

1. `git clone --branch master https://github.com/xbmc/xbmc.git`
2. `git clone --branch Matrix https://github.com/xbmc/visualization.matrix.git`
3. `cd visualization.matrix && mkdir build && cd build`
4. `cmake -DADDONS_TO_BUILD=visualization.matrix -DADDON_SRC_PREFIX=../.. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../../xbmc/kodi-build/addons -DPACKAGE_ZIP=1 ../../xbmc/cmake/addons`
5. `make`

The addon files will be placed in `../../xbmc/kodi-build/addons` so if you build Kodi from source and run it directly 
the addon will be available as a system addon.
