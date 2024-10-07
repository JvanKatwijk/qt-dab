# Qt-DAB-6 [![Build Status](https://travis-ci.com/JvanKatwijk/qt-dab.svg?branch=master)](https://travis-ci.com/JvanKatwijk/qt-dab)

-------------------------------------------------------------------

![6.1](/front-picture.png?raw=true)

-------------------------------------------------------------------
-------------------------------------------------------------------------

**Qt-DAB-6.X is software for Linux, Windows, MacOS and Raspberry Pi for listening to terrestrial **Digital Audio Broadcasting (DAB and DAB+)**.

------------------------------------------------------------------------
About Qt-DAB-6.X
-------------------------------------------------------------------------

Qt-DAB-6.X is the stable version of Qt-DAB, i.e. a version with
a windows installer (actually, there are three) and a Linux x64 AppImage.

![6.5](/Qt_DAB-6.X-1.png?raw=true)
![6.5](/Qt_DAB-6.X-2.png?raw=true)

Table of Contents
=================================================================

* [Introduction](#introduction)
* [Features](#features)
* [Widgets and scopes](#widgets-and-scopes-for-qt-dab)
* [Scan control](#scan-control)
* [Showing a map for TII](#a-note-on-showing-a-map)
* [Documentation](#documentation)
* [Installation on Windows](#installation-on-Windows)
* [Installation on Linux](#installation-on-Linux)
* [Notes on building an executable](#building-an-executable-for-qt-dab-a-few-notes)
* [Using user specified bands](#using-user-specified-bands)
* [xml-files and support](#xml-files-and-support)
* [Copyright](#copyright)

Introduction
=================================================================

**Qt-DAB-6.X** is the stable version of Qt-DAB.
Qt-DAB a rich implementation of a DAB decoder for use
on Linux and Windows based PCs, including some ARM based boards,
such as the Raspberry PI 2 and up.
Qt-DAB can be used with a variety of SDR devices, including DABsticks, all models of the SDRplay, Airspy etc.

Precompiled versions of Qt-DAB-6.X for Linux-x64 (AppImage)
and Windows (installers for
Windows) are, as usual, available. 

The full GUI when running Qt-DAB-6.X contains 4+ widgets, one of them, the
main widget - with sufficient controls to select channels and service(s) -
is always visible. The main widget contains selector
for controlling the visibility of other widgets.

Qt-DAB is being developed under Fedora, and cross compiled -
using the excellent Mingw64 toolset - for Windows.
Thanks to Richard Huber, **Qt-DAB** can be compiled on the Mac as well.

Features
=================================================================

  * Qt-DAB supports input device:
   	- SDR DAB sticks (RTL2838U or similar), 
  	- All SDRplay SDR models (RSP I,  RSP 1A and 1B, RSP II, RSP Duo, RSP Dx and RSP DxII), with separate entries for the v2 and v3 library 
	- HACKRF One, 
  	- Airspy, including Airspy mini,
	- LimeSDR, 
	- Adalm Pluto,
	- **untested** UHD (anyone wants to help testing?)
	- Soapy, a renewed soapy interface driver is even able to map other samplerates than the required 2048000 (limited to the range 2000000 .. 4000000);
	- ExtIO (experimental, Windows only),
	- input from a spyServer,
	- input from rtl_tcp servers.
  * Always supported input from files:
   	- prerecorded dump (`.raw`, `.iq`, '.sdr`, and '.uff' (xml)) files,
  * Qr-DAB supports *Favorites* for easy switching of programs in different ensembles (see section *Favorites*),
  * Qt-DAB supports  DAB (mp2) and DAB+ (HE-AAC v1, HE-AAC v2 and AAC-LC) decoding
  * Qt-DAB supports the MOT SlideShow (SLS)  and Dynamic Label (DLS) with the possibility of saving slides and  dynamic Label text (the latter by right clicking with the mouse, or saving *all* dynamic label texts  - augmented with channel and time info - in a file),
  * Qt-DAB supports automatic reconfiguration of services,
  * Qt-DAB provides different views on the DAB input signal (spectrum, correlation, channel, etc),
  * Qt-DAB provides detailed information (including strength and spectrum views) on the  selected  audio service,
  * Qt-DAB recognizes and interprets  *TII* (Transmitter Identification Information) data),
  *  Qt-DAB-6.X shows - if the received signal is from multiple transmitters - *all* detectable transmitters and displays the transmitters on a map,
  * Qt-DAB supports *dumping* of the input data of the DAB channel (Warning: produces large raw files!) into `.sdr` files or `.xml` file formats and playing them again later (see section on xml format),
  * Qt-DAB supports *saving audio*, either as uncompressed ".wav" files (samplerate 48000, two channels) or saving *aac* frames from DAB+ services for processing by e.g. VLC),
  * Qt-DAB supports saving the ensemble content description and advanced scanning possibilities,
  * Qt-DAB supports generating ip output: when configured the ip data - if selected - is sent to a specified ip address (default: 127.0.0.1:8888),
  * Qt-DAB supports sending *TPEG output* - when configured - as datagrams to port 8888;
  *  Qt-DAB supports (automatic) *EPG* detection and building up a time table,
  * Qt-DAB-6.X supports decoding and displaying  *journaline* when transmitted
as subservice,
  * Qt-DAB offers *Scheduling* of some operations on services for up to 7 days;
  * Qt-DAB allows *background services*. It is possible to run an arbitrary number of DAB+ audioservices (from the current ensemble) as background service with the output sent to a file.
  * Qt-DAB offers options to select other bands, i.e. the L-Band, or channel descriptions from a user prvided file or obsolete modes (Mode II and Mode IV),
  *The Qt-DAB implementation provides a clean device interface, easy to add other devices.

Not implemented:
  * Other bands than used for terrestrial broadcasting in Europe (like DAB over cable)

Widgets and scopes
=======================================================================

![6.2](/Qt_DAB-6.X-2.png)

The full GUI for Qt-DAB 6.X shows 4 (four) larger widgets and - depending
on the settings - a few smaller ones.

The **mainWidget** (see picture) is always visible, it will show
a list of services in either the currently selected channel or from the favourites.  It provides  all means for selecting a channel,
and selecting a service. 
It will  further show the dynamic label, and the slides - if transmitted as part of the service - or a series of default slides.

The widget shows also selectors for the control  of the visibility of the other 3 (three) large widgets.

* touching the *ensemble name* (NPO (8001) in the picture) makes the
content table, i.e. an overview of the content of the ensemble, visible
(or if it is visible, touching will hide it);
 * touching the small icon left of the name of the selected service (here left
of the bold text NPO Radio 5), will show (or hide) the technical widget,
a widget showing all technical details as well as strength indicators and 
a spectrum of the audio of the selected service;
 * touching the icon showing a *speaker* controls the muting of the signal,
if no audio is generated, or if the signal is muted, the icon will show this;
 * touching *with the right hand mouse button* the text of the dynamic label (here, in italic ("ABC - Poisin Arrow") will whow a small menu to put the text on the clipboard;
 * the button labeled *scan* controls the visibility of a the scan handler widget;
 * the button labeled *http* controls the http handler with which a map on which rhe transmitters will be shown;
 * the button labeled *spectrum* controls the visibility of the spectrum widget, a widget that contains information on the DAB signal itself;
 * the button labeled *show controls* controls the visibility of the so-called *configuration and control widget*, a widget that contains numerous settings for this software;
 * the button labeled *favorites* controls  whether the list of services
from the currently selected channel of the list of favourites
is visible.
 * the button labeled "scanlist* controls the visibility
of the scan list, i.e the list of services seen at the most recent scan.

(Obviously, the colors of the buttons, as well as the *font*, the *font size* and the *font color* of the service list shown can be set by the user.)

* The list of services is shown in one of two modesa: either the services
of the currently selected channel are shown, or the list of *favourites*
is shown. Selecting a service* (just clicking with the left
mouse button on its name) is possible in both views.
Of course when selecting a service in the favorites
view it might take some time before the software has switched over to the
appropriate channel, and has received sufficient information on
the ensemble carried in that channel
before being able to select the service in that channel.

The effect of clicking into the right hand column the services list
depends on the viewmode.  In the **ensembleview ** mode, clicking
in the right column indicates **adding** the service to or
removing it from the favorites. In the favorites column it just means removing the element from the favorites.

![6.1](/technical-widget.png)

The technical widget shows - as the name suggests - all technical details of
the selected audio service. If the  audio of the service is also transmitted
on FM, the FM frequency - derived from additional data in the DAB data -
is shown as well.

The buttons at the top of the widget control dumping the audio
(".wav" file) resp. the AAC frames into a file.
The three quality indicators for DAB+ give success rates of
resp. detecting a DAB+ frame, the RS error repair and the AAC decoding.
(For "old" DAB transmissions a single quality indicator appears.

![6.1](/spectrum-scope.png)

The *spectrum scope* shows - as the name suggests - the spectrum of
the incoming DAB signal. One sees clearly that the width of the signal is
app 1.5 MHz (1536 KHz to be precise). To the right, one
sees the *signal constellation*,
the mapping from the complex signals onto their real and imaginary components.
If the selector labeled "ncp" is set, the centerpoints of the 4
lobs is shown. 

The widget shows some quality indicators, such as SNR, frequency offset,
and signal quality.

![6.1](/spectrum-ideal.png)

The ideal form of the spectrum - and the signal constellation as shown in the
IQScope - is not seen often for real inputs.

![6.1](/qt-dab-correlation.png)

The *correlation* scope shows the correlation between the incoming signal and
the data as it should be.
*Correlation* is used in finding the precise start of the
(relevant) data in the input sample stream.
The picture shows that there are more peaks, i.e. the signal from
more than one transmitter is received. 
If an estimate of the distance from the current transmitter
to the home location is known, the display will show it.

![6.1](/qt-dab-null-period.png)

A DAB signal is built up from frames in the sequence of input samples, frames
are separated by a small period of (almost) "no signal", the NULL period.
The NULL scope shows the samples in the transition from the NULL period to
the first data block of a DAB frame.

![6.1](/qt-dab-tii-data.png)

In reality the NULL period is not completely empty, it comtains an encoding
of the TII data. The TII scope shows the spectrum of the data in the NULL
period.

![6.1](/qt-dab-channel.png)

The channel scope shows the *channel response* on the transmitted data, i.e.
the deformation of the transmitted signal as received.
 The picture shows the *cyan colored line*,
i.e. the channel response on the amplitude,
and the *red line*, i.e.  the channel effects on the phase of
the samples.

![6.1](/qt-dab-stddev.png)

The deviation scope shows the mean deviation of the carriers
in the decoded signal, before mapping the carriers to bits.
The Y-axis is in Hz.

![8.1](/configuration-and-control.png)

The configuration and control widget contains  check boxes and
buttons with which the configuration of the decoding process can be
influenced.

At starting up Qt-DAB for the (very) first time, obviously no device
is selected yet, and the widget is made visible 
to allow selection of an input device (the combobox at the bottom line right).

For a detailed description of all selectors, see the manual.

Scan control
=======================================================================

![6.1](/scan-widget.png?raw=true)

A separate widget - visibility under control of the *scan* button on the
main widget - provides the control ob scanning.

The *show* button controls the visibility of the *skiptable*, skiptables
can be load and stored in either the ".ini" file (use the "...default" buttons,
or can be kept as xml file on a user defined place (the other load/store buttons).

The small table at the bottom of the widget is just for convenience, on scanning it will display the number of services found in the channels visited.

A note on showing a map
=======================================================================

![6.1](/QTmap.png?raw=true)

Qt-DAB has - on the main widget -  a button labeled **http**,
when touched, a small webserver starts that can show
the position(s) of the transmitter(s) received on the map. 
Note that two preconditions have to be met:
 * a location has to be known (see the button "coordinates");
 * a database has to be installed (see the button "load table")

Adding the receiver coordinates is simple,  touch the button "coordinates"
on the configuration  and control widget and a small widget appears where the data can be entered.

When running a precompiled version of Qt-DAB (i.e. an AppImage or
a Windows installer from this repository), installing the database
is simple, just touch the button labeled "load table".

Otherwise, a *precomputed database* is available in this repository, 
just unpack the file "tiiFile.zip", copy the resulting ".txdata.tii" file
to your **home** directory. 
(Due to licencing issues the code to upload a database is proprietary).
By default, on starting the server, the "standard" browser on the system will be invoked, listening to port 8080. The configuration (configuration/control) widget contains a selector for switching this off, so that one might choose his/hers own browser.

Documentation
=================================================================

An extensive **user's guide** - in PDF format - for the 6.X version can be found in the "docs" section of the source tree. The documentation contains a complete description of the widgets, of the values in the `.ini` file, on configuring for creating an executable (Linux), and even a complete description on how to add a device to the configuration.

![Qt-DAB documentation](/qt-dab-6-manual.png?raw=true)

Installation on Windows
=================================================================

For Windows an  **installer** can be found in the releases section,
 * https://github.com/JvanKatwijk/qt-dab/releases.
The installer will install the executable as well as required libraries.

:information_source: Note that the device library for the SDRplay devices is no part of the installer. Similar for the Adalm Pluto device library.

Installation on Linux-x64
=================================================================

For Linux-x64 systems, an **appImage** can be found in the releases section,
 * https://github.com/JvanKatwijk/qt-dab/releases.

The appImage contains next to the executable Qt-DAB program, the required
libraries **but not the support libraries for the configured devices**

:information_source: If you want to use a physical device - e.g. a DABstick, an SDRplay, or an AIRspy you need to install the driver libraries for
these devices as well.

For using an SDRplay device one should download the - proprietary - driver software from the SDRplay site. Note that the "old" 2.13 library does not support
the newer SDRPlay device model such as the SDRPlay 1B. the SDRplayDx
and the SDRPlay Dx-II. Use the 3.XX library.

For using the AIRspy and the Hackrf devices, the Ubuntu repository as well as the bullseye repository (for the RPI) provide a working library.

For using an rtlsdr device,  the Ubuntu (and bullseye) repositories
provide a package. However, using that package one needs the
kernel module to be blacklisted, see e.g.
 * https://www.reddit.com/r/RTLSDR/wiki/blacklist_dvb_usb_rtl28xxu/

Personally, I prefer to build a version of the library myself, see
 * "https://osmocom.org/projects/rtl-sdr/wiki".

For using the Lime device I went back to the sources and compiled the
support library myself, see
 * "https://wiki.myriadrf.org/Lime_Suite".

For using the Pluto I followed the instructions on
 * "https://wiki.analog.com/university/tools/pluto/users"

Building an executable for Qt-DAB: a few notes
=================================================================

The steps for generating an executable are straightforward,
It is strongly advised to use qmake/make for the compilation process,
the **qt-dab-6.X.pro** file contains (much) more configuration options
than the **CMakeLists.txt** file does.

Note that the scheme presented below is applied to the Ubuntu 20 system
on the PC, and the  "bullseye" system on the RPI.
While for Debian derived distributions (e.g. Ubuntu)
this scheme probably works more or less directly, For other distributions
names of the library packages may be different. Note that in all cases, 
the development versions are required.

For Windows the easiest approach is to install msys/mingw and follow
the process as sketched.

Step 1
-----------------------------------------------------------------

- :information_source:  In the qt-dab sourcetree, the sources for 6.X are in the subdirectory `qt-dab-6.X` and for qt-dab-6.5 in the subdirectory `qt-dab-6` 

- :information_source: Qt-DAB uses - as the name suggests - the Qt framework,
for the time being still the version 5, it uses further
the Qwt library and the gcc compiler suite.

For compiling and installing Qt-DAB on Ubuntu 20, I load the required libraries as given below:

 *   sudo apt-get update
 *   sudo apt-get install git cmake
 *   sudo apt-get install qt5-qmake build-essential g++
 *   sudo apt-get install pkg-config
 *   sudo apt-get install libfftw3-dev portaudio19-dev 
 *   sudo apt-get install zlib1g-dev 
 *   sudo apt-get install libusb-1.0-0-dev mesa-common-dev
 *   sudo apt-get install libgl1-mesa-dev libqt5opengl5-dev
 *   sudo apt-get install libsamplerate0-dev libqwt-qt5-dev qtmultimedia5-dev
 *   sudo apt-get install qtbase5-dev libqt5svg5-dev
 *   sudo apt-get install libcurl4-openssl-dev

A note on libfdk-aac and libfaad
-------------------------------------------------------------------

Since - esp. for lower bitrates - libfdk-aac is believed to
outperform libfaad in AAC decoding, the default in the configuration
file is set to fdk-aac.  Installing the libfdk-aac from the repository is
 *   sudo apt-get install libfdk-aac-dev

While the libfdk-aac-dev package in both Fedora and Ubuntu 24 work
fine, I had some problems with the package from the repository in  Ubuntu 20
and 22.
For the AppImage, built on U20, a library version was created from the sources
 * "https://github.com/mstorsjo/fdk-aac"

The sources contain a "CMakeLists.txt" file, so building is straightforward).

Alternatively, one could configure for libfdk-aac, change the configuration to load fibfaad (and obviously, install libfaad).

 *   sudo apt-get install libfaad-dev

A note on Qt_Audio
-----------------------------------------------------------------

Qt_DAB allows selection between two audio subsystems, i.e. portaudio and
Qt_Audio. It turns out that Qt_Audio does not work well on Ubuntu 20
(it does on fedora and Ubuntu 24 though). Since the AppImage is built
on Ubuntu 20, selecting the Qt_Audio subsystem is ignored.

Step 2
-----------------------------------------------------------------

While there are dozens of configuration options, take note
of the following ones:

:information_source: chose 'CONGIG+=NO_SSE' if you  NOT are compiling on/for an
X86_64 based system.

One may choose between 'CONFIG += single' or 'CONFIG += double'. In the latter
case, all computations in the "front end" are done with double precision
arithmetic.

:information_source: Devices like SDRplay, AIRspy, RTLSDR, Lime, and
HackRf can be included in the configuration even if no support library
is installed. Qt-DAB tries to read in the required library functions
when the device is selected in run time.

:information_source: Note that for including "soapy" in the configuration,
the soapy  software should have been installed.
The current version is developed on an Fedora box, the soapy library used does not seem compatible with the soapy library on Ubuntu, soapy is NOT configured
in the AppImage.

Note that your choice **SHOULD** be `CONFIG += tiiLib` (see step 4),
the alternative option 'CONFIG+=preCompiled' will **NOT** work since it requires
sources not available under an open source license. However, the built-in
libraries allow access to a preconfigured database (see step 4).

Step 3
-----------------------------------------------------------------

run `qmake` (variants of the name are `qt5-qmake`, `qmake-qt5`) which generates a `Makefile` and then run `make`. Compiling may take some time.
Use `make -j XX' for speeding up the build process, with XX the amount
of parallel threads used. Of course, qmake will complain if not all
required libraries can be found.

Step 4
-----------------------------------------------------------------

Unpack file "tiiFile.zip", and copy the resulting file `.txdata.tii` from the *library* subdirectory in the home directory.
(The unpacked file contains a copy of the database for finding the transmitter's name and location.)
If Qt-DAB cannot find the file, decoding input will just function, without showing the names and without "maps" option of course.

:information_source: Note: Building a version on a fresh install of "bullseye" on the RPI gave a version that wouldn't run: The `Qt_PLUGIN_PATH` was not set. Setting it as given below solved - for me - the problem:

```
	Qt_5= /usr/lib/arm-linux-gnueabihf/qt5
	export QT_PLUGIN_PATH=$Qt_5/plugins
```

Using user specified bands
=================================================================

While it is known that the DAB transmissions are now all in Band III, there are situations where it might is desirable to use other frequencies.
Qt-DAB provides (Unix type builds only)  the opportunity to specify
your own band.  Specify in a file a list of channels, e.g.

	jan	227360
	twee	220352
	drie	1294000
	vier	252650

and pass the file on with the `-A` command line switch. The channel name is just any identifier, the channel frequency is given in kHz. Your SDR device obviously has to support the frequencies for these channels.


xml-files and support
=================================================================

**Clemens Schmidt**, author of the QiRX program (https://qirx.softsyst.com/) and me defined a format for storing and exchanging "raw" data: `.xml`-files. Such a file contains in the first bytes - up to 5000 - a description in xml - as source - of the data contents. This xml description describes in detail the coding of the elements. 

As an example, a description of data obtained by dumping Airspy input: 

 ```
	<?xml version="1.0" encoding="utf-8"?>
	<SDR>
	  <Recorder Name="Qt-DAB" Version="6.X"/>
	  <Device Name="AIRspy" Model="I"/>
	  <Time Value="Wed Dec 18 12:39:34 2019" Unit="UTC"/>
	  <!--The Sample information holds for the whole recording-->
	  <Sample>
	    <Samplerate Value="2500000" Unit="Hz"/>
	    <Channels Bits="12" Container="int16" Ordering="LSB">
	      <Channel Value="I"/>
	      <Channel Value="Q"/>
	    </Channels>
	  </Sample>
	  <!--Here follow one or more data blocks-->
	  <Datablocks>
	    <Datablock Number="1" Count="375783424" Unit="Channel">
	      <Frequency Value="227360" Unit="KHz"/>
	      <Modulation Value="DAB"/>
	    </Datablock>
	  </Datablocks>
	</SDR>

 ```

The device handlers in Qt-DAB support both generating and reading
such an `.xml` file.

While the current implementation for reading such files is limited to a single data block, the reader contains a *cont* button that, when touched while playing the data, will cause continuous playing of the data in the data block.

![Qt-DAB with xml input](/qt-dab-xml.png?raw=true)

The picture shows the reader when reading a file, generated from raw data emitted by the HackRF device.

Copyright
=================================================================

	Copyright (C)  2016 .. 2024
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	Copyright of libraries used - all available under a GPL
	or similar licence, is gratefully acknowledged.
	The Qt-DAB software is made available under the GPL-2.0.
	Qt-DAB is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

