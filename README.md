# Qt-DAB-5 [![Build Status](https://travis-ci.com/JvanKatwijk/qt-dab.svg?branch=master)](https://travis-ci.com/JvanKatwijk/qt-dab)

**Qt-DAB-4** and **Qt-DAB-5** is software for Linux, Windows, MacOS and Raspberry Pi for listening to terrestrial **Digital Audio Broadcasting (DAB and DAB+)**.


Table of Contents
=================================================================

* [Introduction](#introduction)
* [Features](#features)
* [Widgets and scopes](#widgets-and-scopes-for-qt-dab)
* [Documentation](#documentation)
* [Installation on Windows](#installation-on-windows)
* [Installation on Linux x64](#installation-on-linux-x64)
* [Interfacing to another SDR device](#interfacing-to-another-sdr-device)
* [Using user-specified bands](#using-user-specified-bands)
* [xml-files and support](#xml-files-and-support)
* [Differences between Qt-DAB 4 and 5](#qt-dab-4-and-qt-dab-5-same-functionality-different-gui)
* [Showing a map for TII](#a-note-on-showing-a-map)
* [Notes on building an executable](#building-an-executable-for-qt-dab-a-few-notes)
* [Copyright](#copyright)

Introduction
=================================================================

**Qt-DAB-XX** is a rich implementation of a DAB decoder for use on Linux and Windows based PCs, including some ARM based boards, such as the Raspberry PI 2 and up. It can be used with a variety of SDR devices, including DABsticks, all models of the SDRplay, Airspy etc.

Precompiled versions for Linux-x64 (AppImage) and Windows (an installer) are available. 

Thanks to Richard Huber, **Qt-DAB** can be compiled on the Mac.

Features
=================================================================

  * DAB (mp2) and DAB+ (HE-AAC v1, HE-AAC v2 and AAC-LC) decoding
  * MOT SlideShow (SLS)
  * Dynamic Label (DLS) and the possibility of saving dynamic Labels - augmented with channel and time info - in a file,
  * Both DAB bands (and user defined bands) are supported: 
  	* VHF Band III (default),
   	* L-Band (obsolete now),
	* a user defined Band
  * Modes I, II and IV (Mode I default, Modes II and IV obsolete, but can be set in the `.ini` file,
  * Views on the signal: spectrum view incl. constellation diagram, correlation result, TII spectrum and the SNR over time,
  * automatic reconfiguration of services,
  * Detailed information on reception and selected service (SNR, bitrate, frequency, ensemble name, ensemble ID, subchannel ID, used CUs, protection level, CPU usage, program type, language, alternative FM frequency if available, 4 quality bars),
  * If configured, the TII data is mapped upon a transmitter's name, and display of TII (Transmitter Identification Information) data when transmitted,
  * Possibility of displaying a map with position(s) of received transmitter(s),
  * *Presets* for easy switching of programs in different ensembles (see section *Presets*),
  * *Dumping* of the input data of the DAB channel (Warning: produces large raw files!) into `.sdr` files or `.xml` file formats and playing them again later (see section on xml format),
  * Saving audio as uncompressed wave files, and Saving aac frames from DAB+ services for processing by e.g. VLC,
  * Saving the ensemble content description: audio and data streams, including almost all technical data into a text file readable by e.g *LibreOfficeCalc*
  * Advanced scanning function (scan the band, show the results on the screen and save a detailed description of the services found in a file),
  * ip output: when configured the ip data - if selected - is sent to a specified ip address (default: 127.0.0.1:8888),
  * TPEG output: when configured the data is sent to a specified ip address,
  * EPG detection and building up a time table,
  * Supports as input device:
   	- SDR DAB sticks (RTL2838U or similar), 
	- HACKRF One, 
  	- Airspy, including Airspy mini,
  	- SDRplay (RSP I, RSP II, RSP Duo and RSP Dx), with separate entries for v2 and v3 library
	- limeSDR, 
	- Adalm Pluto,
	- Soapy (experimental, Linux only), 
	- ExtIO (experimental, Windows only),
	- rtl_tcp servers.
  * Always supported input from:
   	- prerecorded dump (`.raw`, `.iq` and `.sdr`),
	- `.xml` and `.uff` format files.
  * Clean device interface, easy to add other devices.
  * Scheduling the start of (channel:service) pairs or operations as frame dump or audio dump, even for days ahead.
  * Showing the name of the transmitter received as well as the distance to the receiver and the azimuth.
  * background services. Since 4.351 it is possible to run an arbitrary number of DAB+ audioservices (from the current ensemble) as background service with the output sent to a file.

Partly implemented:

  * TPEG: when configured, TPEG messages are being sent to a TCP port; sources for a simple client are part of the source distribution.
  * Journaline (an untested Journaline implementation is part of the sources).
  * Other bands than used for terrestrial broadcasting in Europe (like DAB over cable)

:information_source: Note:
While the 2.13 support for SDRplay devices is able to handle the RSP 1, RSP II, RSP Ia and RSP duo, the 3.0X support handles all SDRplay RSP's. It is recommended to use the 3.0X support library. Note further that when - on Windows - a 3.10 (or higher) library is installed (e.g. by installing SDRuno), the 2.13 library is not reachable.


qt-dab-4 and qt-dab-5, (almost) same functionality, different GUI
=================================================================

Qt-DAB is in two versions, a version 4 and a version 5.

The basic idea behing Qt-5 is to limit the number of controls on the main widget and shift everything, not needed for simple interaction to the configuration widget (which is now called configuration and control)
Both versions of Qt-DAB are built from (mostly) the same set of sources,
the GUI and the software interfacing the GUI to the rest of the program.

**dabMini**, i.e. the small version for just listening to a service, is obsolete, the source can be found as subdirectory in the "obsolete" directory.

![5.2](/qt-dab-5-main-widget.png?raw=true)
![4.4](/qt-dab-4-main-widget.png?raw=true)

Of course, the configuration and control widget in Qt-DAB-5 is larger
and contains more settings than the configuration and control widget
for Qt-DAB-4.

Since the Qt-DAB-5 main widget shows the station labels (if any),
the option - available in Qt-DAB-4 - for displaying the station labels
on a separate widget is not needed.

![4.5](/qt-dab-4-config-widget.png?raw=true)
![5.2](/qt-dab-5-config-widget.png?raw=true)

The functionality of the versions is (almost) the same, version 5.1 has
(most users will not use it though) the possibility of generating "eti" files.
If there is serious interest for adding the feature to the Qt-DAB-4 interface,
then let me know.

The Qt-DAB-5.2 version has a slightly modified "technical window", 
it now contains a small spectrum display of the audio output.

![4.5](/qt-dab-4-technical-widget.png?raw=true)
![5.2](/qt-dab-5-technical-widget.png?raw=true)

----------------------------------------------------------------------
A note on showing a map
---------------------------------------------------------------------

![4.4](/qt-dab-maps.png?raw=true)

Since some time the Qt-DAB versions have a button labeled **http**, when touched, a small webserver starts that can show the position(s) of the transmitter(s) received on the map. 

By default, on starting the server, the "standard" browser on the system will be invoked, listening to port 8080. The configuration (configuration/control) widget contains a selector for switching this off, so that one might choose his/hers own browser.

The feature will not work if

 * handling the TII database is not installed on the system, and/or
 * you did not provide your "home" coordinates.

The latter is easily done by touching the button "coordinates" on the configuration (configuration/control) widget.


Building an executable for qt-dab: a few notes
=================================================================

While for Linux-x64 and Windows there are precompiled versions, there may be reasons to build an executable. Building an executable is not very complicated, it is described in detail in the manual. Since it is customary to avoid reading a manual, here are the basic steps for the build process.

Step 1
-----------------------------------------------------------------

- :information_source: Note that the sources for 4.4.2 are now in the subdirectory `qt-dab-s4` and for qt-dab-5.0 in the subdirectory `qt-dab-s5` 
- Install required libraries, see section 5.5.3 (page 29) of the manual for 4.4.
- :information_source: Note: It turns out that in recent versions of Debian (and related) distributions the lib `qt5-default` does not exist as as separate library.
- It seems to be part of another of the qt5 packages that is installed.
- Be aware that different distributions store qt files on different locations, adapt the INCLUDEPATH setting in the `.pro` file if needed.

Step 2
-----------------------------------------------------------------

While there are dozens of configuration options, take note of the following ones:

For "soapy" software should have been installed, so leave them commented out when not available.

:information_source: Note that "pluto-2" can be compiled in: as the other support programs, when the device is selected, the support program will (try to) read in the functions of the device library.

For X64 PC's one may choose the option `CONFIG+=PC` (for selecting SSE instructions). If unsure, use `CONFIG+=NO_SSE`.

For letting the software show the transmitter and the azimuth, choose `CONFIG += tiiLib` (see step 4).

:information_source: Note that the file `converted_map.h` is a generated file that contains a binary version of the HTML/javascript code for the server.

Step 3
-----------------------------------------------------------------

run `qmake` (variants of the name are `qt5-qmake`, `qmake-qt5`) which generates a `Makefile` and then run `make`. 

Step 4
-----------------------------------------------------------------

Unpack file `.txdata.tii` (which contains the database data for finding the transmitter's name and location) from `tiiFile.zip` into the user's home directory. If Qt-DAB cannot find the file, it will just function without showing the names and without "maps" option.

If running on an x64 PC or *bullseye* on the RPI you might consider to install `libtii-lib.so` in `/usr/local/lib` from `dab-maxi/library`.

:information_source: Note however that this library needs `curl` to be installed and source code for `libtii-lib.so` is not free. `libtii-lib.so` contains functionality for uploading a new database version (the "load" button on the configuration widget). If Qt-DAB cannot find the library, it will just function without the additional functionality.

:information_source: Note: Building a version on a fresh install of "bullseye" on the RPI gave a version that wouldn't run: The `Qt_PLUGIN_PATH` was not set. Setting it as given below solved - for me - the problem:

```
	Qt_5= /usr/lib/arm-linux-gnueabihf/qt5
	export QT_PLUGIN_PATH=$Qt_5/plugins
```

Widgets and scopes for Qt-DAB
=================================================================

Qt-DAB always shows a main widget; a number of **optional** widgets is visible under user control.

Controls, in V4 on the main widget, were in V5 moved to the configuration and control widget.

Some data on the selected service - if any - can be found on a separate widget, the "Technical Data" widget (*Detail* button).

![Qt-DAB main widget](/qt-dab-technical-widget.png?raw=true)

Other widgets are

  * a configuration widget (V4) or a configuration and control widget (V5)
  * a widget with controls for the attached device,
  * a widget for additional configuration settings,
  * a widget showing the spectrum of the received radio signal, the constellation of the decoded signal and some quality parameters,,
  * a widget showing the spectrum of the NULL period between successive DAB frames from which the TII is derived,
  * a widget showing the correlations response(s) from different transmitters in the SFN,
  * a widget showing the development - over time - of the SNR,
  * if configured and data is detected, the time table for the current audio service

![Qt-DAB totaal](/qt-dab-scan.png?raw=true)

Another widget shows when running a *scan*; the widget shows the contents of the ensembles found in the selected channels. Since 3.5 the possibility exists to save a detailed description of the services in the different channels, in a format easy to process with LibreOffice or comparable programs (a `.csv` file).

Depending on a setting in configuration widget, a logo or slide, transmitted as Program Associated Data with the audio transmission, will be shown here or on a separate widget.

Documentation
=================================================================

An extensive **user's guide** - in PDF format - for the 4.4 version can be found in the "docs" section of the source tree. The documentation contains a complete description of the widgets, of the values in the `.ini` file, on configuring for creating an executable (Linux), and even a complete description on how to add a device to the configuration.

![Qt-DAB documentation](/qt-dab-manual.png?raw=true)


Installation on Windows
=================================================================

For Windows an  **installer** can be found in the releases section, https://github.com/JvanKatwijk/qt-dab/releases. The installer will install the executable as well as required libraries.

The installer will also call the official installer for the dll implementing the 2.3 api for getting access to the SDRplay devices.


Installation on Linux-x64
=================================================================

For Linux-x64 systems, an **appImage** can be found in the releases section, http://github.com/JvanKatwijk/qt-dab/releases. The appImage contains
next to the executable qt-dab program, the required libraries.

Of course it is possible to generate an executable, the manual contains a complete script for Ubuntu type Linux versions.


Interfacing to another SDR device
=================================================================

There exist - obviously - other devices than the ones supported here. Interfacing another device is not very complicated, it might be done using the **Soapy** interface, or one might write a new interface class.

A complete description of how to interface a device to Qt-DAB is given in the user's manual.


Using user specified bands
=================================================================

While it is known that the DAB transmissions are now all in Band III, there are situations where it might is desirable to use other frequencies. Specify in a file a list of channels, e.g.

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
	  <Recorder Name="Qt-DAB" Version="3.2-Beta"/>
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

The device handlers in Qt-DAB support the generation of such an `.xml` file.

While the current implementation for reading such files is limited to a single data block, the reader contains a *cont* button that, when touched while playing the data, will cause continuous playing of the data in the data block.

![Qt-DAB with xml input](/qt-dab-xml.png?raw=true)

The picture shows the reader when reading a file, generated from raw data emitted by the HackRF device.


Copyright
=================================================================

	Copyright (C)  2016 .. 2022
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	The Qt-DAB software is made available under the GPL-2.0.
	The SDR-J software, of which the Qt-DAB software is a part, 
	is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

