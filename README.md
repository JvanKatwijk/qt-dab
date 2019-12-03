# Qt-DAB-3.1-Beta [![Build Status](https://travis-ci.org/JvanKatwijk/qt-dab.svg?branch=master)](https://travis-ci.org/JvanKatwijk/qt-dab)

Qt-DAB-3.1-Beta is software for Windows, Linux and Raspberry Pi for listening to terrestrial Digital Audio Broadcasting (DAB and DAB+). It is the successor of both DAB-rpi and sdr-j-DAB, two former programs by the same author.

---------------------------------------------------------------------
Summary of new features in Qt-DAB-3.1-Beta
---------------------------------------------------------------------

* system related data is now displayed on the main widget
* the "picture(s)" transmitted with the various services are now
shown on the "technicl details" widget
* easy channel scrolling by a "+" and a "-" button, below the
channel selector (not not visible when playing from file input)
* easy scrolling though the list of services by a "+" and a "-"
button, just below the preset selector, or - if the servicelist
has the focus - with the up- or down-arrow moving through the list,
acknowledging by pressing the "enter"
* maintaining a "history" list, a list containing the channel:name
combinations of all services encountered. Selecting from this list
is by selecting an element and touching the *left* mouse button.
Clicking with the *right* mouse button will clear the history list.
The list can be made (in)visible by touching the "xx" button.

----------------------------------------------------------------------
Why a Beta version
----------------------------------------------------------------------

In order to support the new features, such as  + and - buttons for
both services and channels, as well handling the presets,
significant parts of the control software were rewritten
(and somewhat "modernized").
In spite of extensive testing on RPI, x64 Linux and Windows,
the software still may contain some errors.
Error reports are appreciated.

------------------------------------------------------------------
Table of Contents
------------------------------------------------------------------

* [Introduction](#introduction)
* [Features](#features)
* [Widgets and scopes](#widgets-and-scopes)
* [Presets](#Presets)
* [History](#History)
* [Comment on some settings](#comment-on-some-settings)
* [Obsolete properties](#Obsolete-properties)
* [Installation](#features)
  * [Windows](#windows)
  * [Ubuntu Linux](#ubuntu-linux)
  	- [Configuring using the qt-dab.pro file](#configuring-using-the-qt-dabpro-file)
   	- [Configuring using CMake](#configuring-using-cmake)
   	- [Qt](#qt)
  * [Raspberry PI](#raspberry-pi)
  * [appImage for x64 Linux systems](#appimage-for-x64-linux-systems)
* [Interfacing to another device](#interfacing-to-another-device)
* [Copyright](#copyright)

-------------------------------------------------------------------
------------------------------------------------------------------

![Qt-DAB with sdrplay input](/qt-dab-3.0-a.png?raw=true)

------------------------------------------------------------------
Features
------------------------------------------------------------------

  * DAB (mp2) and DAB+ (HE-AAC v1, HE-AAC v2 and LC-AAC) decoding
  * MOT SlideShow (SLS)
  * Dynamic Label (DLS) 
  * Both DAB bands supported: 
  	* VHF Band III
   	* L-Band (only used in Czech Republic and Vatican) (see "obsolete properties")
  * Spectrum view (incl. constellation diagram, impulse response, TII spectrum)
  * Scanning function (scan the subsequent channels in the selected band until a channel is encountered where a DAB signal is detected)
  * Detailed information for selected service (SNR, bitrate, frequency, ensemble name, ensemble ID, subchannel ID, used CUs, protection level, CPU usage, program type, language, 4 quality bars)
  * Detailed information for other services by right-clicking on their name (bitrate, subchannel ID, used CU's protection level, program type)
  * Automatic display of TII (Transmitter Identification Information) data when transmitted
  * Presets for easy switching of programs in different ensembles (see *Presets*)
  * Dumping of the complete DAB channel (Warning: produces large raw files!) into \* sdr files and playing them again later
  * Saving audio as uncompressed wave files
  * Saving aac frames from DAB+ services for processing by e.g. VLC
  * Saving the ensemble content (description of audio and data streams, including almost all technical data) into a text file readable by e.g *LibreOfficeCalc*
  * Supports inputs from 
  	- SDRplay (both RSP I and RSP II),
  	- Airspy, including Airspy mini,
   	- SDR DAB sticks (RTL2838U or similar), 
	- HACKRF One, 
	- limeSDR, 
	- Soapy (experimental, Linux only), and
   	- prerecorded dump (*.raw, *.iq and *.sdr) 
	- rtl_tcp servers
  * Clean interface to add other devices, see below.

Not  (Not yet or partly) implemented:

  * DMB (Audio and Video)
  * TPEG: when configured, TPEG messages are being sent to a TCP port; sources for a simple client are part of the source distribution.
  * EPG: when configured, the EPG decoding will generate so called EHB files.
  * Journaline (an untested Journaline implementation is part of the sources).
  * ip output: when configured the ip data - if selected - is sent to a specificied ip address (default: 127.0.0.1:8888).
  * Other bands than used for terrestrial broadcasting in Europe (like DAB over cable)

------------------------------------------------------------------
Introduction
------------------------------------------------------------------

**Qt-DAB-3.1-Beta** is an implementation of a DAB decoder for use on Linux and Windows based PC's, including some ARM based boards, such as the Raspberry PI, both 2 and 3.

Some other programs are derived from the sources of Qt-DAB, a "light" version **dabradio**, an SDRPlay-specific version **sdrplayDab**, a command-line based version and a stand-alone server version **dab-server**.
The versions with a GUI are implemented in C++, using the Qt framework for the implementation of the GUI. The command-line version dab-cmdline and the dab-server are implemented using C++, and do not depend on Qt.

The **dab-server** can be installed to run as a "service" on an RPI, with control - over a bluetooth connection - from an "app" on an Android tablet.

Furthermore, for DX purposes, a **dab-scanner** is implemented that allows
for a continuous scanning of selected channels in a given band. Results are 
written in a txt file, formatted for use with *LibreOffice Calc* and
comparable programs.

**dabradio**, **sdrplayDab**, the Qt-free version **dab-cmdline**, the **dab-server** and the **dab-scanner** have their own repository on Github.

Qt-DAB-3.1-Beta also supports input from an rtl-tcp server (see osmocom software) and from pre-recorded files (`*.sdr`, `*.iq` and `*.raw`). Obviously there is a provision for dumping the input into an (\*.sdr)-file. 

Note that if the rtl_tcp server is used as input device, the connection needs to support the inputrate, i.e. 2,048,000 I/Q samples (i.e. 2 * 2,048,000 bytes/second).

Since the Qt-DAB program has to run on a headless RPI 2/3 using the home WiFi, the resulting PCM output can be sent - if so configured - to a TCP port (Sources for a small client are part of the source distribution).

For further information please visit http://www.sdr-j.tk

------------------------------------------------------------------
Widgets and scopes
------------------------------------------------------------------

The picture below shows Qt-DAB's main window and the other 6 **optional**
widgets

* a widget with controls for the attached device,
* a widget showing the technical information of the *selected service* as well
as some information on the quality of the decoding, 
* a widget showing the spectrum of the received radio signal and the constellation of the decoded signal,
* a widget showing the spectrum of the NULL period between successive DAB frames,
* and a widget showing the response(s) from different transmitters in the SFN.

While the main window is always shown, the others are only shown when pushing a button on the main window (touching the
button again will cause the widget to disappear from the screen).

![Qt-DAB with SDRplay input](/qt-dab-3.0-c.png?raw=true)

The buttons and other controls on the main widget are equipped with
*tool tips* briefly explaining the (in most cases obvious) function
of the element.

The elements in the **left part** of the widget are concerned with
selecting a channel and a service. 

To ease operation the channel selector is augmented with a "-" and a "+"
button for selecting the previous resp. next channel.

To ease selection of a service, a second pair of "-" and "+" buttons
is available, now for selecting the previous resp. the next service 
on the list.

Different from previous versions is that now some information, previously
shown on the "technical data" widger is now shown on the main widget.

Some data on the selected service - if any - is to be found on
a separate widget. This widget will show where the data for the
service is to be found in the DAB frames, and how it is encoded.

Furthermore, if the service is accompanied by a logo, that logo will
be shown here.

The further selectors are concentrated on the bottom part of the right side
of the widget. Buttons to make scopes visible, to store input and or
output into a file, to select input device and the audio and to
scan and store a description of the ensemble are in that section.

----------------------------------------------------------------------
Presets
----------------------------------------------------------------------

A *preset* selector is available to store and retrieve "favorit" services,
note that the services are encoded as "channel:serviceName" pair,
it sometimes happens that a service appears in more than one ensemble
(see as example the "Omroep West" service, appearing in channels 5B and 8A.

![Qt-DAB with sdrplay input](/qt-dab-3.0-d.png?raw=true)

---------------------------------------------------------------------------
History 
---------------------------------------------------------------------------

Qt-DAB-3.1-Beta saves data on all services found. Pairs Channel:serviceName
will be made (in)visible when touching the appropriate button (the
one labeled with "xx").

The data in stored in a file in xml format.
The history can be cleared by a click of the right mouse button,
clicking on a channel:servicename combination with the left
mouse button will cause the software to attempt to set the channel and
select the name.

This feature is experimental

---------------------------------------------------------------------------
Comment on some settings
-------------------------------------------------------------------------------

Some settings are preserved between program invocations, they are stored in a file `.qt-dab.ini`, to be found in the home directory.
Settings maintained between program invocations are typically the name 
of the device used, the channel that was used, the name of the service
last used etc.

The presets are stored in an xml file, `.qt-dab-presets.xml'.

Some settings are not influenced by buttons or sliders of the GUI, they will only change by editing the .ini file.

Two settings have an impact on the speed and accuracy of the synchronization,

`threshold_1=x'
`threshold_2=x'

Detecting whether a channel might contain a DAB signal is essentially
by looking at the amplitude of the signal and by correlating the signal
with the signal as it should be. Of course detecting the existence
of a signal is important, however, a speedy detection that no signal
is available is also important, especially when stepping though the
channels.

The `threshold_x=y' setting is a value used in determining whether
or not the correlation is sufficiently strong to assume the existence
of DAB data in the channel.
'threshold_1=x' is used on start up of the channel, `threshold_2=x' is
used when running. To avoid false positives, `threshold_2' ususally
holds a higher value.
Default values are 3 and 5.

`diff_length=x'

Once the existence of DAB data is established, an attempt is made
to estimate the frequency offset of the incoming signal. Again, this
is done by correlating a fragment of the (processed) incoming signal
with a predefined signal. The length of the segment (length interms
of samples) used is defined by the settings for `diff_length'.
If no value is given, the default value of 40 is used 

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
If one uses the rtl_tcp handler, the default value for the "port" is 1234,
a port can de set in the "ini" file by setting

```
rtl_tcp_port=xxx
```

-------------------------------------------------------------------------
Obsolete properties
-------------------------------------------------------------------------

The recent DAB standard eliminated the support for Modes other than Mode 1 and Bands other than VHF Band III. The Qt-DAB implementation still supports these features, however, since they are obsolete, there are no controls on the GUI anymore (the control for the Mode was already removed from earlier versions). 

Explicitly setting the Mode and/or the Band is possible by including some command lines in the ".qt-dab.ini" file.

For the Mode, one will add/set a line

	dabMode=Mode x, where x is either 1, 2 or 4

For the Band, one will add/set a line

	dabBand=band, where band is either VHF Band III or L_Band

Since the control for the dab band is now removed from the GUI, this GUI could be made slightly smaller.

------------------------------------------------------------------
Windows
------------------------------------------------------------------

For windows an  **installer** is to be found in the releases section, https://github.com/JvanKatwijk/qt-dab/releases. The installer will install the executable as well as required libraries.
The installer will also call the official installer for the dll implementing the api for getting access to the SDRplay devices.

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
	
4. Edit the `qt-dab.pro` file for configuring the supported devices and other options. Comment the respective lines for devices that you do not have or do not
want to support out.

4.a. Check the installation path to qwt. If you were downloading it from http://qwt.sourceforge.net/qwtinstall.html please mention the correct path in `qt-dab.pro` file (for other installation change it accordingly): 
  ```
  INCLUDEPATH += /usr/local/include  /usr/local/qwt-6.1.3
  ````

4.b. If you are compiling on/for an RPI2/3 device, you might want to uncomment the line DEFINE+=__THREADED_BACKEND. This will cause a better load balance on the cores of the processor. 

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

  c) select defines related to performance

Adding or removing from the configuration is in all cases by commenting or uncommenting a line in the configuration file.

Comment the lines out by prefixing the line with a `#` in the `qt-dab.pro` file (section "unix") for the device(s) you want to exclude in the configuration. In the example below, rtl_tcp (i.e. the connection to the rtlsdr server) won't be used.
```
CONFIG          += dabstick
CONFIG          += sdrplay
CONFIG          += lime
CONFIG          += rtl_tcp
CONFIG          += airspy
CONFIG          += hackrf
#CONFIG         += soapy

```

In the Windows configuration one may also choose, however *soapy* is not supported.
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

Note that on default audio output is handled
by the portaudio library. Other options are using Qt audio or
audio through an TCP port.

The source tree contains a directory `sound-client`, that contains sources to generate a simple "listener" for remote listening.

For selecting the output to be handled by the Qt system (default device only) uncomment
````
#CONFIG		+= qt-audio
````

The CPU load of running the Qt-DAB program can be divided over more
CPU cores by uncommenting
````
DEFINES		+= __THREADED_BACKEND
````

This causes the backend to run in a separate thread.

For choosing between single and double precision floats
one can comment out or uncomment the line
````
DEFINES		+= __HIGH_PRECISION__
````
Note that with __HIGH_PRECISION__ defined, the load on an RPI3 will rise
to app 60 %, while with __HIGH_PRECISION__ not defined, the load will
remain app 42 %.


------------------------------------------------------------------
Configuring using CMake
------------------------------------------------------------------

The `CMakeLists.txt` file has all devices and the spectrum switched off per default. You can select a device (or more devices) without altering the `CMakeLists.txt` file, but by passing on definitions to the command line.

An example:
```
cmake .. -DRTLSDR=ON -DRTLTCP=ON 
```
	
will generate a makefile with support for 

 a) the RTLSDR (dabstick) device, 
 
 b) for the remote dabstick (using the rtl_tcp connection) and 
 
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

The current set of sources provides support for both the RSP-I and the RSP-II. Due to an incompatibility between libraries 2.13 and older versions, be certain to have at least 2.13 installed.

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

The most recent distributions, Raspbian Stretch and Raspbian Buster
supports both Qt5 and a qwt compiled against Qt5.

Since Raspbian Stretch is a Debian derivate, the description for creating a version under Ubuntu applies, a script to download all required packages, compile
and build a full executable for qt-dab is part of the source tree.

---------------------------------------------------------------------------
appImage for x64 Linux systems
---------------------------------------------------------------------------

https://github.com/JvanKatwijk/qt-dab/releases contains a generated appImage, **Qt-DAB-x64.Appimage**, which is created on Ubuntu 16.04 (Xenial).

Different from previous versions, this version assumes you have installed
the support library for the devices you want to use.

If you want to run with an SDRplay, follow the installation instructions for the library from http://www.sdrplay.com . All further dependencies are included.

If you want to run an RTLSDR based dabstick, please note that
the appImage may complain with some Linux distros with a "librtlsdr.so" pre-installed
that the kernel module has to be blacklisted. Depending on the distribution, blacklisting is in
/etc/modprobe.d/local-blaclist

For compiling and installing a support library for an RTLSDR device, 
follow the instruction on https://osmocom.org/projects/rtl-sdr/wiki/Rtl-sdr.

For compiling and installing a support library for an AIRspy device,
find the sources on https://github.com/airspy/airspyone_host/tree/master/libairspy

The appImage itself is just a self-contained single file which you have to make executable in order to run.

For more information see http://appimage.org/

-----------------------------------------------------------------------
Interfacing to another device
-----------------------------------------------------------------------

There are - obviously - more different devices than supported
here. Interfacing another device is not very complicated,
it might be done using the "Soapy" interface, or one might
write a new interface class.

The device handlers are implemented as a class, derived from
the class *virtualInput*. Only a few functions have to
be implemented, to *set* and *get* the VFO frequency, 
to inspect the number of samples available and to get a number
of samples, to start and stop operating the device
and to report on the number of bits per sample. This last
function, is used to scale the various spectrum scopes.

A complete description is given in the file "interfacing.txt",
in the sourcetree

-----------------------------------------------------------------------
# Copyright
------------------------------------------------------------------------


	Copyright (C)  2013, 2014, 2015, 2016, 2017, 2018, 2019
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	The Qt-DAB software is made available under the GPL-2.0.
	The SDR-J software, of which the Qt-DAB software is a part, 
	is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

