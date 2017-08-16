# Qt-DAB [![Build Status](https://travis-ci.org/JvanKatwijk/qt-dab.svg?branch=master)](https://travis-ci.org/JvanKatwijk/qt-dab)

Qt-DAB is a Software for Windows, Linux and Raspberry Pi for listening to terrestrial Digital Audio Broadcasting (DAB and DAB+). It is the successor of both DAB-rpi and sdr-j-DAB, two former programs by the same author.

------------------------------------------------------------------
Table of Contents
------------------------------------------------------------------

* [Introduction](#introduction)
* [Features](#features)
* Installation
  * [Windows](#windows)
  * [Ubuntu Linux](#ubuntu-linux)
  	- [Configuring using the qt-dab.pro file](#configuring-using-the-qt-dabpro-file)
   	- [Configuring using CMake](#configuring-using-cmake)
   	- [Qt](#qt)
  * [Raspberry PI](#raspberry-pi)
  * [appImage](#appimage)
 * [Comment on some settings](#comment-on-some-settings)
 * [Copyright](#copyright)
 
------------------------------------------------------------------
Features
------------------------------------------------------------------

  * DAB (mp2) and DAB+ (HE-AAC v1, HE-AAC v2 and LC-AAC) decoding
  * MOT SlideShow (SLS)
  * Dynamic Label (DLS) 
  * Both DAB bands supported: 
  	* VHF Band III
   	* L-Band (only used in Czech Republic and Vatican)
  * Spectrum view (incl. constellation diagram)
  * Scanning function (scan the subsequent channels in the selected band  until a channel is encountered where a DAB signal is detected)
  * Detailed information for selected service (SNR, bitrate, frequency, ensemble name, ensemble ID, subchannel ID, used CUs, protection level, CPU usage, program type, language, 4 quality bars)
  * Dumping of the complete DAB channel (Warning: produces large raw files!) into *.sdr files and playing them again later
  * Saving audio as uncompressed wave file
  * Saving the ensemble content (audio and data streams, including almost all technical data) into a text file
  * Supports various inputs from 
  	- Airspy, including Airspy mini,
   	- SDR DAB sticks (RTL2838U or similar), 
   	- SDRplay and 
   	- prerecorded dump (*.raw, *.iq and *.sdr) 
 
Not (or partly) implemented:

  * DMB (Audio and Video)
  * TPEG
  * EPG: when configured, the EPG decoding will generate so called EHB files.
  * Journaline (an untested Journaline implementation is part of the sources)
  * Other bands than used for terrestrial broadcasting in Europe (like DAB over cable)
  * HackRF


------------------------------------------------------------------
Introduction
------------------------------------------------------------------

![Qt-DAB with SDR file loaded](/screenshot_qt-dab.png?raw=true)

**Qt-DAB** is the result of merging the DAB-rpi and the sdr-j-DAB programs of the author. It became more and more complex to maintain different versions: modifications made in one version did not always end up in the other versions, so the versions started to diverge.

Since furthermore a separate "command line only" version is developed (a version not using Qt at all), while a large part of the sources is also used in the development of a version handling ETI files, there was a real need to re-organize.

Therefore, it was decided to merge DAB-rpi and sdr-j-DAB and rename the result **Qt-DAB** - to distinguish from the Qt-free version.

The Qt-free version, the "command line only" version, is named dab-cmdline, and is built around a library that does the DAB decoding. It has its own repository on Github.

The Qt-DAB and the dab-cmdline version both support decoding of terrestrial DAB and DAB+ reception from either an AIRSPY, a SDRplay or a dabstick (rtl_sdr).

The Qt-DAB version also supports input from an rtl-tcp server and pre-recorded files (`*.sdr`, `*.iq` and `*.raw`), which obviously provides the opportunity of dumping the input into a (*.sdr)-file. 

Since the Qt-DAB version has to run on a headless RPI 2, using the home WiFi, in- or excluding the part for showing the spectrum and the constellation, is determined by setting the configuration. Furthermore, a configuration option is included to have the sound output delivered its samples through a TCP connection.

For further information please visit http://www.sdr-j.tk

An (outdated) manual in PDF format can be found at http://www.sdr-j.tk/sdr-j-dab-manual-0.99.pdf (is valid for qt-dab as well)

Some settings are preserved between program invocations, they are stored in a file `.qt-dab.ini`, to be found in the home directory. See [Comment on some settings](#comment-on-some-settings) for more details.

------------------------------------------------------------------
Windows
------------------------------------------------------------------

Windows releases can be found at https://github.com/JvanKatwijk/qt-dab/releases . Please copy them into the same directory you've unzipped http://www.sdr-j.tk/windows-bin.zip as it uses the same libraries.

If you want to compile it by yourself, please install Qt through its online installer, see https://www.qt.io/ 

------------------------------------------------------------------
Ubuntu Linux
------------------------------------------------------------------

If you are not familar with compiling then please continue reading by jumping to chapter [appImage](#appimage) which is much easier for Linux beginners.

For generating an executable under Ubuntu, you can put the following commands into a script. 

1. Fetch the required components
   ```
   sudo apt-get update
   sudo apt-get install qt4-qmake build-essential g++
   sudo apt-get install libsndfile1-dev qt4-default libfftw3-dev portaudio19-dev  
   sudo apt-get install libfaad-dev zlib1g-dev rtl-sdr libusb-1.0-0-dev mesa-common-dev 
   sudo apt-get install libgl1-mesa-dev libqt4-opengl-dev libsamplerate-dev libqwt-dev
   cd
   ```
   
2. Fetch the required libraries 

  a) Assuming you want to use a dabstick (also known as rtlsdr) as device, fetch a version of the library for the dabstick
  ```
  wget http://sm5bsz.com/linuxdsp/hware/rtlsdr/rtl-sdr-linrad4.tbz
  tar xvfj rtl-sdr-linrad4.tbz 
  cd rtl-sdr-linrad4
  sudo autoconf
  sudo autoreconf -i
  ./configure --enable-driver-detach
  make
  sudo make install
  sudo ldconfig
  cd
  ```
   	
   b) Assuming you want to use an Airspy as device, fetch a version of the library for the Airspy	
  ```
  sudo apt-get install build-essential cmake libusb-1.0-0-dev pkg-config
  wget https://github.com/airspy/host/archive/master.zip
  unzip master.zip
  cd airspyone_host-master
  mkdir build
  cd build
  cmake ../ -DINSTALL_UDEV_RULES=ON
  make
  sudo make install
  sudo ldconfig
  ```   
	
  Clean CMake temporary files/dirs:
  ```
  cd host-master/build
  rm -rf *
  ```
	
3. Get a copy of the Qt-DAB sources
  ```
  git clone https://github.com/JvanKatwijk/qt-dab.git
  cd qt-dab
  ```
	
4. Edit the `qt-dab.pro` file for configuring the supported devices and other options. Comment the respective lines out if you don't own an Airspy (mini) or an SDRplay.

5. If DAB spectrum and the constellation diagram should be displayed, check the installation path to qwt. If you were downloading it from http://qwt.sourceforge.net/qwtinstall.html please mention the correct path in `qt-dab.pro` file (for other installation change it accordingly): 
  ```
  INCLUDEPATH += /usr/local/include  /usr/local/qwt-6.1.3
  ```
	
6. Build and make
  ```
  qmake qt-dab.pro
  make
  ```

  You could also use QtCreator, load the `qt-dab.pro` file and build the executable.
  
  Remark: The executable file can be found in the sub-directory `linux-bin`. A make install command is not implemented.


------------------------------------------------------------------
Configuring using the qt-dab.pro file
------------------------------------------------------------------

Options in the configuration are:

  a) select or unselect devices

  b) select the output: soundcard or TCP connection

  c) select or unselect basic MSC data handling

Adding or removing from the configuration is in all cases by commenting or uncommenting a line in the configuration file.

Comment the lines out by prefixing the line with a `#` in the `qt-dab.pro` file (section "unix") for the device(s) you want to exclude in the configuration. In the example below, rtl_tcp (i.e. the connection to the rtlsdr server) won't be used.
```
CONFIG          += dabstick
CONFIG          += sdrplay
#CONFIG          += rtl_tcp
CONFIG          += airspy
```
	
Remark: Input from pre-recorded files (8 bit unsigned *.raw and 16-bit "wav" *.sdr files) is configured by default.


In order to show the spectrum and the constellation, uncomment
```
CONFIG          += spectrum  
```
In such a case
```
DEFINES		+= __QUALITY
```
a "quality indicator" (standard phase deviation of the demodulated signal) will be shown (smaller is better).

For selecting the output to be sent to a RCP port, uncomment
```
CONFIG         += tcp-streamer         # use for remote listening
```

The source tree contains a directory "sound-client", that contains sources to generate a simple "listener" for remote listening.

For showing some information on the selected program, uncomment
```
DEFINES         += TECHNICAL_DATA
```

For basic MSC data handling, i.e. MOT handling etc, uncomment
```
DEFINES         += MSC_DATA__           # use at your own risk
```

and for experimental MOT handling within the data handling, uncomment
```
DEFINES		+= MOT_DATA
```

Two experimental configuration parameters are
```
CONFIG		+= try_epg
```

and
```
CONFIG		+= try_tii
```

If `try_epg` is configured then EPG (which stands for Electronic Program Guide) data will be written to xml files. Please select the service carrying EPG data.

If `try_tii` is configured an attempt will be made to decode the Transmitter Identification Information (TII). Note: Many ensembles do not carry this information.

	
------------------------------------------------------------------
Configuring using CMake
------------------------------------------------------------------

Note that there are now **TWO** (!) `CMakeLists.txt-xxx` files in the directory, if you want to use cmake, copy `CMakeLists.txt-qt5` to `CMakeLists.txt`.

The `CMakeLists.txt-qt4` file is used for creating an appImage and still "under development".

The `CMakeLists.txt` file has all devices and the spectrum switched off as default. You can select a device (or more devices) without altering the `CMakeLists.txt` file, but by passing on definitions to the command line.

An example:
```
cmake .. -DDABSTICK=ON -DRTLTCP=ON -DSPECTRUM=ON
```
	
will generate a makefile with support for a) the dabstick, b) for the remote dabstick (using the rtl_tcp connection) and c) for the spectrum in the configuration.

Other devices that can be selected (beside dabstick and rtl_tcp) are sdrplay and airspy.

The default location for installation depends on your system, mostly `/usr/local/bin` or something like that. Set your own location by adding
```
-DCMAKE_INSTALL_PREFIX=your installation prefix
```

For other options, see the `CMakeLists.txt` file.

Important: Note that CMake expects Qt5 to be installed.

-----------------------------------------------------------------
SDRplay
-----------------------------------------------------------------

The current set of sources has some support for the RSP-II. It is assumed that at least library version 2.09 is installed.

------------------------------------------------------------------
Qt
------------------------------------------------------------------

The software uses the Qt library and - for the spectrum and the constellation diagram - the qwt library.

The `CMakeLists.txt` assumes Qt5, if you want to use Qt4, and you want to have the spectrum in the configuration, be aware of the binding of the qwt library (i.e. Qt4 and a qwt that uses Qt5 does not work well). As mentioned above, in the (January 2017) distribution of Raspbian Jessie, the qwt library provided is compiled against Qt4. 

-----------------------------------------------------------------
Raspberry PI
------------------------------------------------------------------

The Qt-DAB software runs pretty well on the author's RPI-2. The average load on the 4 cores is somewhat over 60 percent.

One remark: getting "sound" is not always easy. Be certain that you have installed the alsa-utils, and that you are - as non-root user - able to see devices with `aplay -L`

In arch, it was essential to add the username to the group "audio".

Note that on the current distribution of Raspbian Jessie (i.e. January 2017) the best way to generate the executable for Qt-DAB is to use Qt4. The qwt library - needed if you want the spectrum viewer and the constellation diagram - in the distribution is compiled with Qt4. 

---------------------------------------------------------------------------
appImage
---------------------------------------------------------------------------

https://github.com/JvanKatwijk/qt-dab/releases contains a generated appImage which is created on Ubuntu 14.04 (Trusty), and uses Qt4. It assumes that you have installed a device, either a dabstick (i.e. rtlsdr), an Airspy or a SDRplay. All further dependencies are included. There is only one file which you have to make executable in order to run.

For more information see http://appimage.org/

--------------------------------------------------------------------------------
Comment on some settings
-------------------------------------------------------------------------------

Some values of settings are maintained between program invocations. This is done in the (hidden) file `.qt-dab.ini` in the user's home directory.

Some settings are not influenced by buttons or sliders of the GUI, they will only change by editing the .ini file.

Typical examples are

`autoStart=0` 
when set to 1 the program will start the DAB handling automatically, so you do not have to press the `START` button.

`saveSlides=1` 
when set to 0 the slides that are attached to audio programs will not be saved. If set to 1 the slides will be saved in a directory `/tmp/qt-pictures` (Linux) or in `%tmp%\qt-pictures` (Windows).

`picturesPath` 
defines the directory where the slides (MOT slideshow) should be stored.

`showSlides=1` 
when set to 0 the slides will not be shown.

`has-presetName=1` 
when set the name of the selected service - that is selected when closing down the program - is kept and at the next invocation of the program, an attempt is made to start that particular service. The name of the service is kept as `presetname=xxxx`


The background colors of the spectrum can be changed by setting 
```
displaycolor=blue
gridcolor=red
```

# Copyright


	Copyright (C)  2013, 2014, 2015, 2016, 2017
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	The Qt-DAB software is made available under the GPL-2.0.
	The SDR-J software, of which the Qt-DAB software is a part, 
	is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

