# Qt-DAB-6 [![Build Status](https://travis-ci.com/JvanKatwijk/qt-dab.svg?branch=master)](https://travis-ci.com/JvanKatwijk/qt-dab)

-------------------------------------------------------------------

![6.1](/front-picture.png?raw=true)

-------------------------------------------------------------------
-------------------------------------------------------------------------

**Qt-DAB-6** is software for Linux, Windows, MacOS and Raspberry Pi for listening to terrestrial **Digital Audio Broadcasting (DAB and DAB+)**.


------------------------------------------------------------------------
Recent changes
------------------------------------------------------------------------

In the development version, Qt-DAB-6.X,  a few errors that still could 
occur in Version 6.5. were corrected.
Furthermore, Qt-DAB-6.X contains a few goodies that will
be incorporated in the "stable" version to come.

First of, one of the problems with the implemented sound handler was
that sound did not always work after restarting the program with a
saved - and therefore selected - audiostream.
6.X - has now  two (sub)systems for the sound handling.
Next to the portaudio interface that was the default
for a long time, the sound interface is provided by Qt itself.
If the katter audio interface is selected, a **volume slider** is shown on
the main widget.

As known, most DAB ensembles are transmitted by more than a single
transmitter. All transmitters in a so-called **Single Frequency Network**
transmit the same ensemble. However, each transmitter adds a little
data with which the transmitter can be identified.
Other than with e.g. FM or AM, where receiving signal at the same time
from different transmitters, transmitting the same audio is hardly enjoyable,
with digital modes, the software is able to select data from
the strongest signal.  In my neighbourhoodm there are - for a few ensembles
- four transmitters within 20 Km.

New is that while Qt-DAB decodes data from the strongest signal,
Qt-DAB-6.X has a special **mode**, dxMode, in which it
tries to identify **all ** transmitetrs that are 
transmitting the same signal. The TII decoding was adapted for this purpose.

Selecting the "dx mode" (with a checkbox on the
configuration widget) causes the software to display all transmitters
the identification of which could be extracted from the signal

![6.X](/transmitters.png?raw=true)

If the dx mode is NOT selected, the transmitter's name and distance
are - as always - shown on the botton line in the main widget.

While Qt-DAB-6.X is still "under development", its sources can be
found in the subdirectory "Qt-DAB-RC"

------------------------------------------------------------------------
About Qt-DAB-6.5
-------------------------------------------------------------------------

Qt-DAB-6.5 is the stable version of Qt-DAB, i.e. a version with
a windows installer (actually, there are two) and a Linux x64 AppImage.

![6.5](/Qt_DAB-6.5-1.png?raw=true)
![6.4](/Qt_DAB-6.5-2.png?raw=true)

Qt-DAB-6.5 has undergone some improvements that were - for a long time - on
my own wishlist, while  some  other modifications are based on user's requests.
Since when using Qt-DAB as regular radio device, I switch audio between
only a few services, their names are listed in my "list of favorites".
The current version therefore shows on start up the list of favorites.
Of course when selecting a service, the software "knows" tall services
in the ensemble of the selection, the widget therefore lets you choose between
the view on the current ensemble and the view on the favorites.

In the **ensembleview**  the services found in the currently active channel are
displayed. In the list, it shows that services that are also belonging to the
favorites, are marked. 
Clicking on the element in the right column
adds or removes a service from the favorites. (Of course, in the list of
favorites, clicking on the right hand column of the service in question
removes the service).

In a previous version, control on the visibility of the "technical widget"
was by clicking on the name of the selected service in the right hand
part of the widget. Of course that is unclear, therefore a small icon
is added instead. 

As in the previous version, control on muting the audio is delegated
to the icon of the loadspeaker. The speaker indication tells whether or
not sound is "on".

Based on user's request, input can be obtained from "spy-servers", i.e.
remote from over the internet.
Such a spy server can handle AIRspy devices and RT2832
based dabsticks. Of course the connection should be able to handle
data with a reasonable (read: high) bandwidth. For 16 bit data, i.e. a sample takes 2 x 2 bytes, one needs 10MB/s (slightly  ore actually since the data is transmitted 
as packages with a header).

Also, the Soapy interface was rewritten and (seems to) work(s) fine.

While the possibility of saving DLS data was implemented some time ago, there
is now also a possibility of putting the current DLS text on the clipboard
and saving it. Just click with the right hand mouse button on the text.

Table of Contents
=================================================================

* [Introduction](#introduction)
* [Features](#features)
* [Widgets and scopes](#widgets-and-scopes-for-qt-dab)
* [Scan control](#scan-control)
* [Showing a map for TII](#a-note-on-showing-a-map)
* [Documentation](#Documentation)
* [Installation on Windows](#Installation-on-Windows)
* [Installation on Linux](#Installation-on-Linux)
* [Notes on building an executable](#building-an-executable-for-qt-dab-a-few-notes)
* [Using user specified bands](#Using-user-specified-bands)
* [xml-files and support](#xml-files-and-support)
* [A Note on previous versions](#A-Note-on-previous-versions)
* [Copyright](#Copyright)

Introduction
=================================================================

**Qt-DAB-XX** is a rich implementation of a DAB decoder for use on Linux and Windows based PCs, including some ARM based boards, such as the Raspberry PI 2 and up. It can be used with a variety of SDR devices, including DABsticks, all models of the SDRplay, Airspy etc.

The current version is 6.5. Versions 5.5 is still available
and can be built, using the same set of sources as the current one.
A development version, named Qt-BAB-6X,  can be found
in the "qt-dab-RC" directory.

Precompiled versions for Linux-x64 (AppImage) and Windows (an installer) are
- as usual -available. 

Thanks to Richard Huber, **Qt-DAB** can be compiled on the Mac.

Features
=================================================================

  * DAB (mp2) and DAB+ (HE-AAC v1, HE-AAC v2 and AAC-LC) decoding
  * MOT SlideShow (SLS)
  * Dynamic Label (DLS) with the possibility of saving dynamic  the Label text  by right clicking with the mouse, or saving all dynamic label texts  - augmented with channel and time info - in a file,
  * DAB is now transmitted by default in the VHF Band III, there are options to select the L-Band, and it is possible to receive data from user defined channels.
  * While Mode I is *the* mode for DAB, Qt-DAB offers an option to interpret data modelled in Mode II or Mode IV,
  * There is a focus on viewing the signal: next to showing the spectrum of the received signal and a constellation diagram of the decoded signal,
a view on the correlation of the signal and the TII spectrum can be selected. Furthermore, a view on the transition from the NULL period to the first datablock, a viewe on the impact of the channel on the signal, and a view on the frequency offsets of the carriers in the decoded signal can be selected. Finally, in a separate widget, the development over time of the SNR can be made visible.
  * automatic reconfiguration of services.
  * Detailed information on reception and selected service (SNR, bitrate, frequency, ensemble name, ensemble ID, subchannel ID, used CUs, protection level, CPU usage, program type, language, alternative FM frequency if available, 4 quality bars),
  * Frequency spectrum of the resulting audio as well as "strength" meters of the audio stream are made visible,
  * The TII data is mapped upon a transmitter's name, and display of TII (Transmitter Identification Information) data when transmitted,
  * Possibility of displaying a map with position(s) of received transmitter(s),
  * *Favorites* for easy switching of programs in different ensembles (see section *Favorites*),
  * *Dumping* of the input data of the DAB channel (Warning: produces large raw files!) into `.sdr` files or `.xml` file formats and playing them again later (see section on xml format),
  * Saving audio as uncompressed ".wav" files (48000 two channels), and saving *aac* frames from DAB+ services for processing by e.g. VLC,
  * Saving the ensemble content description: audio and data streams, including almost all technical data into a text file readable by e.g *LibreOfficeCalc*
  * Advanced scanning function (scan the band, show the results on the screen and save a detailed description of the services found in a file),
  * ip output: when configured the ip data - if selected - is sent to a specified ip address (default: 127.0.0.1:8888),
  * TPEG output: when configured the data is sent as datagrams to port 8888;
  * EPG detection and building up a time table,
  * Supports as input device:
   	- SDR DAB sticks (RTL2838U or similar), 
	- HACKRF One, 
  	- Airspy, including Airspy mini,
  	- SDRplay (RSP I,  RSP 1A and 1B, RSP II, RSP Duo and RSP Dx), with separate entries for v2 and v3 library 
	- limeSDR, 
	- Adalm Pluto,
	- **untested** UHD (anyone wants to help testing?)
	- Soapy, a renewed soapy interface driver is even able to map other samplerates than the required 2048000 (limited to the range 2000000 .. 4000000);
	- input from a spyServer,
	- ExtIO (experimental, Windows only),
	- rtl_tcp servers,
	= spy servers.
  * Always supported input from:
   	- prerecorded dump (`.raw`, `.iq` and `.sdr`),
	- `.xml` and `.uff` format files.
  * *Scheduling* the start of (channel:service) pairs or operations as frame dump or audio dump, for up to 7 days ahead.
  * background services. Since 4.351 it is possible to run an arbitrary number of DAB+ audioservices (from the current ensemble) as background service with the output sent to a file.
  * Clean device interface, easy to add other devices.

Partly implemented:

  * TPEG: when configured, TPEG messages are being sent to a TCP port; sources for a simple client are part of the source distribution.
  * Journaline (an untested Journaline implementation is part of the sources).
  * Other bands than used for terrestrial broadcasting in Europe (like DAB over cable)

:information_source: Note:
While the 2.13 support for SDRplay devices is able to handle the RSP 1, RSP II, RSP Ia and RSP duo, the 3.XX support handles all SDRplay RSP's **except the original RSP 1**. It is recommended to use the 3.0X support library. 

Widgets and scopes
=======================================================================

![6.2](/Qt_DAB-6.5-1.png)

The full GUI for Qt-DAB 6.5 consists of 4 (four) larger widgets,
The **mainWidget** (see picture) is always visible, various elements in
the widget control the visibility of the other 3 (three).
The main widget of Qt-DAB provides all means for selecting a channel,
and selecting a service. 

Transmission of most services  carryone or more *slide*s,
such slides are shown, if no slide is shown a **default slide** is shown.

If configured, the main widget displays the name of the transmitter received, 
and the distance and azimuth to the (specification of the) receiving station.s
Furthermore, the TII elements  (i.e. the identifiction of the transmitter, here 04 05, are shown. New is the addition of the height from which the transmitter
is working.

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
 * touching the button labeled *scan* controls the visibility of a the scan handler widget;
 * touching the button labeled *http* controls the http handler with which a map on which rhe transmitters will be shown;
 * touching the button labeled *spectrum* controls the visibility of the spectrum widget, a widget that contains information on the DAB signal itself;
 * touching the button labeled *show controls* controls the visibility of the so-called *configuration and control widget*, a widget that contains numerous settings for this software;
 * touching the button labeled *favorites* will switch the visibility of the list of services shown between the *ensemble view* and the *favorites view*;
 *touching the button labeled "scanlist* shows (or hides) the scan list, i.e
the list of services seen at the most recent scan.

Obviously, the colors of the buttons, as well as the *font*, the *font size* and the *font color* of the service list shown can be set by the user.

Selecting a service (just clicking with the left mouse button on its name)
 is possible in both views, i.e. both the ensemble view
and the favorites view. Of course when selecting a service in the favorites
view it might take some time before the software has switch over to the
appropriate channel, has received the esemble information of that channel
and has selected the service in that channel.

Selecting itself is simply by clicking on the service name. The effect of
clicking into the right hand column depends on the viewmode.
In the **ensembleview ** mode, clicking in the right column indicates adding the service to or removing it from the favorites. In the favorites column it just means removing the element from the favorites.

![6.1](/technical-widget.png)

The technical widget shows - as the name suggests - all technical details of
the selected audio service. If the service is also transmitted on FM
the FM frequency - derived from additional data in the DAB data - is shown as well.

![6.1](/spectrum-scope.png)

The *spectrum scope* shows the spectrum of the incoming DAB signal. One
sees clearly that the width of the signal is app 1.5 MHz. To the right one
sees the signal constellation.  The latter shows the mapping from the complex signals onto their real and imaginary components.

![6.1](/spectrum-ideal.png)

The ideal form of the spectrum - and the signal constellation as shown in the
IQScope - is not seen often.

![6.1](/qt-dab-correlation.png)

The correlation scope shows the correlation between the incoming signal and
some predefined data. Correlation is helpful in finding the precise start of the
(relevant) data in the input stream. The picture shows that the signal
from more than one transmitter is received. 
If the distance to the current transmitter is known, the display
shows the estimated distances - from the receiver location - to the
other peaks as well.

![6.1](/qt-dab-null-period.png)

The NULL scope shows the samples in the transition from NULL period to
the first data block of a DAB frame. The software detects (computes) the first sample following the  NULL period, needed for collecing the data for decoding.

![6.1](/qt-dab-tii-data.png)

The TII scope shows the spectrum of the data in the NULL period, since that data contains the TII (Transmitter Identification Information) data in an encoded form;

![6.1](/qt-dab-channel.png)

The channel scope shows the *channel response* on the transmitted data, i.e.
the deformation of the transmitted signal. The
picture shows the *cyan colored line*,
i.e. the channel response on the amplitude,
and the *red line*, i.e.  the channel effects on the phase of
the samples.

![6.1](/qt-dab-stddev.png)

The deviation scope shows the mean deviation of the carriers
in the decoded signal, before mapping the carriers to bits.
The Y-axis is in Hz.

![8.1](/configuration-and-control.png)

The configuration and control widget contains check boxes and
buttons with which the configuration of the decoding process can be
influenced.
The top rightshows the 3 selectors for the font with which the services
in the service list are dsiplayed.

At starting up Qt-DAB for the (very) first time, the widget will show
to allow selection of an input device (the combobox at the bottom line right).


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

Since some time the Qt-DAB versions have a button labeled **http**,
when touched, a small webserver starts that can show
the position(s) of the transmitter(s) received on the map. 

By default, on starting the server, the "standard" browser on the system will be invoked, listening to port 8080. The configuration (configuration/control) widget contains a selector for switching this off, so that one might choose his/hers own browser.

The feature will *not* work if

 * handling the TII database is not installed on the system, and/or
 * you did not provide your "home" coordinates.

When running a precompiled version of Qt-DAB (i.e. an AppImage or
a Windows installer from this repository), installing the database
is simple, just touch the button labeled "load table".

Otherwise, you have to install a small library first (for Linux x64 or
the RPI), just see the manual.
If that fails a precomputed database is available in this repository, 
just unpack the file "tiiFile.zip", copy the resulting ".txdata.tii" file
to your **home** directory.
(Due to licencing issues the code to upload a database is proprietary).

Adding the receiver coordinates is simple,  touch the button "coordinates"
on the configuration  and control widget and a small widget appears where the data can be entered.

Documentation
=================================================================

An extensive **user's guide** - in PDF format - for the 6.4 version can be found in the "docs" section of the source tree. The documentation contains a complete description of the widgets, of the values in the `.ini` file, on configuring for creating an executable (Linux), and even a complete description on how to add a device to the configuration.
Note that not all changes done in 6.5 are covered in the current manual.

![Qt-DAB documentation](/qt-dab-6-manual.png?raw=true)


Installation on Windows
=================================================================

For Windows an  **installer** can be found in the releases section, https://github.com/JvanKatwijk/qt-dab/releases. The installer will install the executable as well as required libraries.

The installer will also call the official installer for the dll implementing the 2.3 api for getting access to the SDRplay devices.


Installation on Linux-x64
=================================================================

For Linux-x64 systems, an **appImage** can be found in the releases section, https://github.com/JvanKatwijk/qt-dab/releases. The appImage contains
next to the executable qt-dab program, the required libraries **but not
the support libraries for the configured devices**

Building an executable for Qt-DAB: a few notes
=================================================================

Of course it is possible to generate an executable, a separate document
is available that contains a complete script for Ubuntu type Linux versions.

The basis steps for generating an executable are straightforward,
It is strongly advised to use qmake/make for the compilation process,
the **qt-dab-6.X.pro** file contains (much) more configuration options
than the **CMakeLists.txt" file does.

Note that the scheme presented below applied to "bullseye" on the RPI,
i.e. a Linux variant. While for Debian derived distributions (e.g. Ubuntu)
this scheme probably works more or less directly, for other distributions
names of the library packahes may be different. Note that in all cases, 
the development versions are required.

For Windows the easiest approach is to install msys/mingw.

Step 1
-----------------------------------------------------------------

- :information_source:  In the qt-dab sourcetree, the sources for 6.5 are in the subdirectory `qt-dab-6.5` and for qt-dab-5.x in the subdirectory `qt-dab-s5` 

For compiling and installing Qt-DAB on an old RPI3 (64 bits bullseye),
I load the required libraries as given below:

 *   sudo apt-get update
 *   sudo apt-get install git cmake
 *   sudo apt-get install qt5-qmake build-essential g++
 *   sudo apt-get install pkg-config
 *   sudo apt-get install libsndfile1-dev
 *   sudo apt-get install libfftw3-dev portaudio19-dev 
 *   sudo apt-get install zlib1g-dev 
 *   sudo apt-get install libusb-1.0-0-dev mesa-common-dev
 *   sudo apt-get install libgl1-mesa-dev libqt5opengl5-dev
 *   sudo apt-get install libsamplerate0-dev libqwt-qt5-dev qtmultimedia5-dev
 *   sudo apt-get install qtbase5-dev libqt5svg5-dev
 *   sudo apt-get install libfdk-aac-dev

If you want to use a physical device - e.g. a DABstick, an SDRplay, or an AIRspy
you need to install the driver libraries for these devices as well.
For most common devices repositories of common Linux distributions contain
a driver library.

for SDRplay devices one should download the - proprietary - driver software
from the SDRplay site. For the AIRspy the bullseye repository provides a
library.

Step 2
-----------------------------------------------------------------

While there are dozens of configuration options, take note
of the following ones:

:information_source: Note that for including "soapy" in the configuration, soapy  software should have been installed. Be aware that the current
version is developed on an Fedora box, the soapy library used does not seem compatible with the soapy library on Ubuntu.

:information_source: chose 'CONGIG+=NO_SSE' if you  NOT are compiling on/for an
X86_64 based system.

One may choose between 'CONFIG += single' or 'CONFIG += double'. In the latter
case, all computations in the "front end" are done with double precision
arithmetic.

Note that by default a choice is made for `CONFIG += tiiLib` (see step 4),
the alternative opyion 'CONFIG+=preCompiled' will **NOT** work since it requires
sources not available under an open source license. However, the built-in
libraries allow access to a preconfigured database (see step 4).

Step 3
-----------------------------------------------------------------

run `qmake` (variants of the name are `qt5-qmake`, `qmake-qt5`) which generates a `Makefile` and then run `make`. Compiling may take some time.

Step 4
-----------------------------------------------------------------

Unpack file "tiiFile.zip", and copy the resulting file `.txdata.tii` from the *library* subdirectory in the home directory.
The unpacked file contains a copy of the database for finding the transmitter's name and location.
If Qt-DAB cannot find the file, decoding input will just function, without showing the names and without "maps" option of course.

:information_source: Note: Building a version on a fresh install of "bullseye" on the RPI gave a version that wouldn't run: The `Qt_PLUGIN_PATH` was not set. Setting it as given below solved - for me - the problem:

```
	Qt_5= /usr/lib/arm-linux-gnueabihf/qt5
	export QT_PLUGIN_PATH=$Qt_5/plugins
```

Using user specified bands
=================================================================

While it is known that the DAB transmissions are now all in Band III, there are situations where it might is desirable to use other frequencies.
Qt-DAB provides the opportunity to specify your own band.
Specify in a file a list of channels, e.g.

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
	  <Recorder Name="Qt-DAB" Version="6.5"/>
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

A Note on soapy
================================================================
Soapy is not configured for the AppImage. There is an incompatibility
between the Soapy library on Ubuntu 20 - where the AppImages are built - and my development environment.

A Note on previous versions
=================================================================
The previous version V5.X is still maintained in a separate subtree 
and can be compiled with the recent modifications applied as well.

Copyright
=================================================================

	Copyright (C)  2016 .. 2024
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	The Qt-DAB software is made available under the GPL-2.0.
	The SDR-J software, of which the Qt-DAB software is a part, 
	is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

