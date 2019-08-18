# Qt-DAB [![Build Status](https://travis-ci.org/JvanKatwijk/qt-dab.svg?branch=master)](https://travis-ci.org/JvanKatwijk/qt-dab)

Qt-DAB is a Software for Windows, Linux and Raspberry Pi for listening to terrestrial Digital Audio Broadcasting (DAB and DAB+). It is the successor of both DAB-rpi and sdr-j-DAB, two former programs by the same author.

-----------------------------------------------------------------
NEW in 2.7.1
-----------------------------------------------------------------

2.7.1 is merely a "bug-fix" version.

Added a "default" filename when touching the "content" button to
save the description of an ensemble.
You could consider sending the DAB ensemble description to Gunther Lorenz glorenz@fmlist.org. He maintains a large database with FM stations and DAB ensembles.

------------------------------------------------------------------
NEW in 2.7
------------------------------------------------------------------

New devices:

 a) the LimeSDR is now supported (due to missing a decent "dll" file
for the library, for now in Linux only);

 b) as an experimental feature, a Soapy interface is implemented. Tested
with the SDRplay and the LimeSDR it works.
Soapy support is included in neither the appImage nor the Windows version.

Synchronization reconsidered

One of the issues to be resolved was resynchronization. It could
happen that - due to some faulty input - the synchronization
got lost and could not be regained without a reset.

The chain of events leading to such a loss of synchronization is
identified and chance that - due to some faulty input - synchronization
will be lost is - at least - much smaller.

Obsolete properties

The DAB standard eliminated the support for Modes other than Mode 1 and
Bands other than VHF Band III. The implementation still supports these
features, however, since they are obsolete, there are no controls on the
GUI anymore (the control for the Mode was already removed from earlier
versions). 
Explicitly setting the Mode and/or the Band is possible by 
including some command lines in the ".qt-dab.ini" file.

For the Mode, one will add/set a line

	dabMode=Mode x, where x is either 1, 2 or 4

For the Band, one will add/set a line

	dabBand=band, where band is either VHF Band III or L_Band

Since the control for the dab band is now removed from the GUI, this GUI
could be made slightly smaller.


------------------------------------------------------------------
Table of Contents
------------------------------------------------------------------

* [Introduction](#introduction)
* [Features](#features)
* [Widgets and scopes](#widgets-and-scopes)
* [Installation](#features)
  * [Windows](#windows)
  * [Ubuntu Linux](#ubuntu-linux)
  	- [Configuring using the qt-dab.pro file](#configuring-using-the-qt-dabpro-file)
   	- [Configuring using CMake](#configuring-using-cmake)
   	- [Qt](#qt)
  * [Raspberry PI](#raspberry-pi)
  * [appImage for x64 Linux systems](#appimage-for-x64-linux-systems)
  * [Comment on some settings](#comment-on-some-settings)
  * [A note on intermittent sound](#a-note-on-intermittent-sound)
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
  * Spectrum view (incl. constellation diagram, impulse response, NULL symbol)
  * Scanning function (scan the subsequent channels in the selected band until a channel is encountered where a DAB signal is detected)
  * Detailed information for selected service (SNR, bitrate, frequency, ensemble name, ensemble ID, subchannel ID, used CUs, protection level, CPU usage, program type, language, 4 quality bars)
  * Automatic display of TII (Transmitter Identification Information) data when transmitted
  * Dumping of the complete DAB channel (Warning: produces large raw files!) into \* sdr files and playing them again later
  * Saving audio as uncompressed wave files
  * Saving the ensemble content (audio and data streams, including almost all technical data) into a text file
  * Supports various inputs from 
  	- SDRplay (both RSP I and RSP II),
  	- Airspy, including Airspy mini,
   	- SDR DAB sticks (RTL2838U or similar), and
	- HACKRF One, 
	- limeSDR, 
	- Soapy (experimental, Linux only),
   	- prerecorded dump (*.raw, *.iq and *.sdr) 
 
Not  (Not yet or partly) implemented:

  * DMB (Audio and Video)
  * TPEG: when configured, TPEG messages are being sent to a TCP port; sources for a simple client
    are part of the source distribution.
  * EPG: when configured, the EPG decoding will generate so called EHB files.
  * Journaline (an untested Journaline implementation is part of the sources).
  * ip output: when configured the ip data - if selected - is sent to a specificied ip address (default: 127.0.0.1:8888).
  * Other bands than used for terrestrial broadcasting in Europe (like DAB over cable)

------------------------------------------------------------------
Introduction
------------------------------------------------------------------

![Qt-DAB with sdrplay input](/qt-dab-1.png?raw=true)

**Qt-DAB-2.7* is an implementation of a DAB decoder for use on Linux and Windows based PC's, including some ARM based boards, such as the Raspberry PI, both 2 and 3.

Beside to Qt-DAB, there exists a "light" version, **dabradio**, an SDRPlay-specific version **sdrplayDab** , a command-line based version and a stand-alone
server version **dab-server**. All these versions with a GUI are implemented in C++, using the Qt framework for the implementation of the GUI. The command-line version is implemented using C++, and does not depend on Qt.
The **dab-server** will run as a "service" on an RPI, with control - over a bluetooth connection - from an "app" on
an Android tablet.

**dabradio**, **sdrplayDab**, the Qt-free version, **dab-cmdline** the "command-line only", and the **dab-server** version have their own repository on Github.

The Qt-DAB, the dab-cmdline version, and the dab-server version support decoding of terrestrial DAB and DAB+ reception from either an AIRSPY, an SDRplay, the HACK RF, or a dabstick (rtl_sdr). The sdrplayDab version is tightly couples to the SDRplay device.

Beside to these C++ based versions,  (slightly limited) versions are implemented in Java and Ada, both with their own repository on Github. 

The Qt-DAB version also supports input from an rtl-tcp server (see osmocom software) and pre-recorded files (`*.sdr`, `*.iq` and `*.raw`). Obviously there is a provision for dumping the input into an (\*.sdr)-file. 

Note that if the rtl_tcp server is used as input device, the connection needs to support the inputrate, i.e. 2,048,000 I/Q samples (i.e. 2 * 2,048,000 bytes/second).

Since the Qt-DAB version has to run on a headless RPI 2/3, using the home WiFi, the resulting PCM output can be sent - if so configured - to a TCP port (Sources for a small client are part of the source distribution).

For further information please visit http://www.sdr-j.tk

Some settings are preserved between program invocations, they are stored in a file `.qt-dab.ini`, to be found in the home directory. See [Comment on some settings](#comment-on-some-settings) for more details.

------------------------------------------------------------------
Widgets and scopes
------------------------------------------------------------------

The picture below shows Qt-DAB's main window, with the (few) control buttons, and 5 other widgets

* a widget with controls for the attached device,
* a widget showing the technical information of the selected service, 
* a widget showing the spectrum of the received radio signal and the constellation of the decoded signal,
* a widget showing the spectrum of the NULL period between successive DAB frames,
* and a widget showing the response(s) from different transmitters in the SFN.

While the main window and the widget for the device control are always shown, each of the others is only shown when pushing a button on the main window. In case a widget is invisible (i.e. not selected), the software to generate a spectrum is bypassed, so not to waste CPU power.

![Qt-DAB with SDRplay input](/qt-dab-2.png?raw=true)

------------------------------------------------------------------
Windows
------------------------------------------------------------------

For windows there is an installer, to be found in the releases section, https://github.com/JvanKatwijk/qt-dab/releases. The installer will install the executable as well as required libraries.
The installer will also call the official installer for the dll implementing the api for getting access to the SDRplay devices.

If you want to compile it by yourself, please install Qt through its online installer, see https://www.qt.io/ 

------------------------------------------------------------------
Ubuntu Linux
------------------------------------------------------------------

If you are not familar with compiling then please continue reading by jumping to chapter [appImage](#appimage-for-x64-linux-systems) which is much easier for Linux beginners.

Ubuntu 16.04 (and on) have good support for Qt5 and qwt (compiled for Qt5). For generating an executable under Ubuntu (16.04 or newer), you can put the following commands into a script. (For Ubuntu 14.04 look into the package manager for Qt4 packages)

1. Fetch the required components
   ```
   sudo apt-get update
   sudo apt-get install qt5-qmake build-essential g++
   sudo apt-get install libsndfile1-dev qt5-default libfftw3-dev portaudio19-dev 
   sudo apt-get install libfaad-dev zlib1g-dev rtl-sdr libusb-1.0-0-dev mesa-common-dev
   sudo apt-get install libgl1-mesa-dev libqt5opengl5-dev libsamplerate0-dev libqwt-qt5-dev
   sudo apt-get install qtbase5-dev

   ```
   
2. Fetch the required libraries 

  a) Assuming you want to use a dabstick (also known as rtlsdr) as device, fetch a version of the library for the dabstick
  ```
   git clone git://git.osmocom.org/rtl-sdr.git
   cd rtl-sdr/
   mkdir build
   cd build
   cmake ../ -DINSTALL_UDEV_RULES=ON -DDETACH_KERNEL_DRIVER=ON
   make
   sudo make install
   sudo ldconfig
   cd ..
   rm -rf build
   cd ..
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

  Note that in order for the libraries to be effective, ensure that
  
 a) the path `/usr/local/lib` is in the load library paths (i.e. it is named in one of the `.conf` files in the `etc/ld.so.conf` directory
 
 b) a file exists in the `/etc/udev/rules.d` directory describing the device, allowing "ordinary" users to access the device.

3. Get a copy of the Qt-DAB sources
  ```
  git clone https://github.com/JvanKatwijk/qt-dab.git
  cd qt-dab
  ```
	
4. Edit the `qt-dab.pro` file for configuring the supported devices and other options. Comment the respective lines out if you don't own an Airspy (mini) or an SDRplay.

4.a. Check the installation path to qwt. If you were downloading it from http://qwt.sourceforge.net/qwtinstall.html please mention the correct path in `qt-dab.pro` file (for other installation change it accordingly): 
  ```
  INCLUDEPATH += /usr/local/include  /usr/local/qwt-6.1.3
  ````

4.b. If you are compiling on/for an RPI2 device, you might want to uncomment the line DEFINE+=__THREADED_BACKEND__. This will cause a better load balance on the cores of the processor. 

5. Build and make
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
CONFIG		+= hackrf
```

In the Windows configuration one may also choose
````
CONFIG		+= extio
````

for use with (appropriate) extio handlers

Remark: Input from pre-recorded files (8 bit unsigned `*.raw` and `*.iq` as well as 16-bit wav `*.sdr` files) is configured by default.

Audio samples are - by default - sent to an audio device using the portaudio library. Two alternatives are available:

For selecting the output to be sent to a TCP port, uncomment
```
#CONFIG         += tcp-streamer         # use for remote listening
```

The source tree contains a directory `sound-client`, that contains sources to generate a simple "listener" for remote listening.

For selecting the output to be handled by the Qt system (default device only) uncomment
````
#CONFIG		+= qt-audio
````

For basic MSC data handling, i.e. MOT handling etc, uncomment
```
DEFINES         += MSC_DATA__           # use at your own risk
```

An experimental configuration parameter is
```
CONFIG		+= try_epg
```

If `try_epg` is configured then EPG (Electronic Program Guide) data will be written to `*.xml` files. Please select the service carrying EPG data.


------------------------------------------------------------------
Configuring using CMake
------------------------------------------------------------------

The `CMakeLists.txt` file has all devices and the spectrum switched off per default. You can select a device (or more devices) without altering the `CMakeLists.txt` file, but by passing on definitions to the command line.

An example:
```
cmake .. -DRTLSDR=ON -DRTLTCP=ON -DSPECTRUM=ON
```
	
will generate a makefile with support for 

 a) the RTLSDR (dabstick) device, 
 
 b) for the remote dabstick (using the rtl_tcp connection) and 
 
 c) for the spectrum in the configuration.

Other devices that can be selected (beside dabstick and rtl_tcp) are sdrplay, HackRF and airspy. Use `-DHACKRF=ON`, `-DSDRPLAY=ON`, or `-DAIRSPY=ON` after the `cmake` command if you want to configure them.

The default location for installation depends on your system, mostly `/usr/local/bin` or something like that. Set your own location by adding
```
-DCMAKE_INSTALL_PREFIX=your installation prefix
```

For other options, see the `CMakeLists.txt` file.

Important: Note that `CMakeLists.txt` file expects the appropriate Qt version (and - if configured - the qwt library) to be installed.

-----------------------------------------------------------------
SDRplay
-----------------------------------------------------------------

The current set of sources provides support for both the RSP-I and the RSP-II. Due to an incompatibility between libraries 2.13 and older versions, be certain to have at least 1.13 installed.

------------------------------------------------------------------
Qt
------------------------------------------------------------------

The software uses the Qt library and - for the spectrum and the constellation diagram - the qwt library.

The `CMakeLists.txt` assumes Qt5. If you want to use Qt4, and you want to have the spectrum in the configuration, be aware of the binding of the qwt library (i.e. Qt4 and a qwt that uses Qt5 does not work well).  

-----------------------------------------------------------------
Raspberry PI
------------------------------------------------------------------

The Qt-DAB software runs pretty well on the author's RPI-2 and 3. The average load on the 4 cores is somewhere between 50 and 60 percent.

One remark: getting "sound" is not always easy. Be certain that you have installed the alsa-utils, and that you are - as non-root user - able to see devices with `aplay -L`

In arch, it was essential to add the username to the group "audio".

The most recent distribution of Raspbian Stretch (i.e. August 2017) supports both Qt5 and a qwt compiled against Qt5.

Since Raspbian Stretch is a Debian derivate, the description for creating a version under Ubuntu applies, a script to download all required packages, compile
and build a full executable for qt-dab is part of the releases section.

---------------------------------------------------------------------------
appImage for x64 Linux systems
---------------------------------------------------------------------------

https://github.com/JvanKatwijk/qt-dab/releases contains a generated appImage, **Qt-DAB-x64.Appimage**, which is created on Ubuntu 14.04 (Trusty), and uses Qt4 (so it basically should run on any x-64 based linux system that isn't too old.).

It assumes that you have installed an appropriate usb library, libraries to support either a dabstick (i.e. rtlsdr) or an Airspy are included in the appImage (the appropriate udev rules, i.e. rules to allow a non-root user to use the device through USB, will be installed by the execution of the appImage, that is why it will ask for your password. If you have installed the device of your choice on your system, you can just cancel this request).

If you want to run with an SDRplay, follow the installation instructions for the library from http://www.sdrplay.com . All further dependencies are included.

The appImage is just a self-contained single file which you have to make executable in order to run.

It furthermore contains an (experimental) appImage for use under Stretch on an RPI2/3, **Qt-DAB-ARM.AppImage**.

All further dependencies are included

For more information see http://appimage.org/

--------------------------------------------------------------------------------
Comment on some settings
-------------------------------------------------------------------------------

Some values of settings are maintained between program invocations. This is done in the (hidden) file `.qt-dab.ini` in the user's home directory.

Some settings are not influenced by buttons or sliders of the GUI, they will only change by editing the .ini file.

Typical examples are

`saveSlides=1` 
when set to 0 the slides that are attached to audio programs will not be saved. If set to 1 the slides will be saved in a directory `/tmp/qt-pictures` (Linux) or in `%tmp%\qt-pictures` (Windows).

`picturesPath` 
defines the directory where the slides (MOT slideshow) should be stored. Default is the home directory.

`showSlides=1` 
when set to 0 the slides will not be shown.

`has-presetName=1` 
when set the name of the selected service - that is selected when closing down the program - is kept and at the next invocation of the program, an attempt is made to start that particular service. The name of the service is kept as `presetname=xxxx`

The background colors of the spectrum can be changed by setting 
```
displaycolor=blue
gridcolor=red
```

The Qt-DAB program searches - if available - for TII (transmitter identification information) data in the FIC. If TII data is available, the small label at the bottom of the Technical Details widget will color green, and the main ID if the transmitters is shown. Also the list of geographical positions of the transmitters of the SFN is printed on the terminal. If the identification of the transmitter received can be decoded, it will be shown at the bottom line of the Technical Data Widget.

The periodicity of the search for the data defining the position of the transmitter received can be set by adding a line to the `.qt-dab.ini` file

```
tii_delay=xxx
```
where `xxx` is the number of seconds. Default of the delay is 20 seconds.

The Qt-DAB program will search for additional transmitter identification
if

```
tii_depth=xx
```

is set. A reasonable value is 3, default the value will be set to 1.


Similarly, if 

```
echo_depth=xxx
```
is set, a guess will be made about the relative delays of the incoming
data of other transmitters in the SFN.


If one uses the rtl_tcp handler, the default value for the "port" is 1234,
a port can de set in the "ini" file by setting

```
rtl_tcp_port=xxx
```

--------------------------------------------------------------------------------
A note on intermittent sound 
-------------------------------------------------------------------------------

In some cases, in some periods of listening, the sound is (or at least seems) interrupted. There are two different causes for this:

First of all the incoming signal is weak and audio packages do not pass the many controls that are executed. This shows in the widget "technical data", not all the colored bars at the bottom are 100 percent green. An audio package represents 24 milliseconds of audio, loss of a few packages leads to an interruption of the sound.

A second reason has to do with system parameters. Too small a buffersize in the audio driver causes too high a frequency of calls to a callback function. In Linux this shows by an underrun reported by the alsa sound system. The buffer size can be set (in multiples of 256 audio samples) by the value of "latency" in the `.ini` file. The default value is 1.

On my RPI 2 - with Stretch - `latency=2` works best, on my RPI 3 there is
no problem whatsoever.

# Copyright


	Copyright (C)  2013, 2014, 2015, 2016, 2017, 2018, 2019
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	The Qt-DAB software is made available under the GPL-2.0.
	The SDR-J software, of which the Qt-DAB software is a part, 
	is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

