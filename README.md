# Qt-DAB-6 [![Build Status](https://travis-ci.com/JvanKatwijk/qt-dab.svg?branch=master)](https://travis-ci.com/JvanKatwijk/qt-dab)

-------------------------------------------------------------------

![6.1](/qt-dab-logo.png?raw=true)
![6.1](/DABplus_Logo_Colour_sRGB.png?raw=true)

-------------------------------------------------------------------
-------------------------------------------------------------------------

**Qt-DAB-6** is software for Linux, Windows, MacOS and Raspberry Pi for listening to terrestrial **Digital Audio Broadcasting (DAB and DAB+)**.

![6.4](/qt-dab-6.4.png?raw=true)

Differences with the previous versions

------------------------------------------------------------------------
Experimental Spyserver support

As an experiment, support for the spyServer is added. The sources
contain 2 versions, one 8 bit and one 16 bit.
SpyServer is well known and provides remore support for RTLSDR based
devices and the AIRSPYs (Note that while the spyServer itself provides 
support for the AIRspy HF, the AIRSpy HF is not well suited to receive DAB, 
the sample rate is way too low.

Note however: 
Running DAB requires a samplerate of 2048000 Samples/second. The samplerate
used when the spyserver is connected to an Airspy One is 2500000 S.s
In 16 bit mode, each sample is transferred as 4 bytes, i.e 32 bits.
With some additional overhead, it is clear that a connection between
Qt-DAB and spyserver should be capable of transmitting 100 Mb.

It is easy to see that in 8 bit mode one needs half, say up to 50 Mb.

The 8 bit and 16 bit version can coexist in an implementaiton, 
the precompiled AppImage contains both versions, as does the
32-6.40 version of the installer.

Note that the software is still experimental

-----------------------------------------------------------------------
------------------------------------------------------------------------

In Qt-DAB 6.40 there are visible changes to the main widget and to
the configuration and control widget.
In the main widget the *content* button, the *technical data* button
and the *mute* button are removed. The *schedule* button is moved
to the configuration and control widget.
The functionality of the removed buttons is not removed.

 * touch the name of the ensemble on top of the left part of the main widget
and the content table appears (or disappears)

 * touch the name of the selected service on the right half of the widget
and the technical widget will appear (or disappear)

 *  the icon of the loudspeaker will show whether or not a sound channel
is selected. Touching the icon will control the muting

Furthermore, the scanning function is re-implemented and the *scan* button
on the main widget now controls the visibility of a *scan-control widget*.

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


The current version is 6.40. The versions 5.5 and 4.8 are still available
and can be built, using the same set of sources as the current 6.40.

Precompiled versions for Linux-x64 (AppImage) and Windows (an installer) are available. 

Thanks to Richard Huber, **Qt-DAB** can be compiled on the Mac.

Features
=================================================================

  * DAB (mp2) and DAB+ (HE-AAC v1, HE-AAC v2 and AAC-LC) decoding
  * MOT SlideShow (SLS)
  * Dynamic Label (DLS) with the possibility of saving dynamic  the Label text  by right clicking with the mouse, or saving all dynamic label texts  - augmented with channel and time info - in a file,
  * While DAB is now transmitted by default in the VHF Band III, there are options to select the L-Band, and it is possible to receive data from user defined channels.
  * While Mode I is "the" mode for DAB, Qt-DAB offers an option to interpret data modelled in Mode II or Mode IV,
  * There is a focus on viewing the signal: next to showing the spectrum of the received signal and a constellation diagram of the decoded signal,
a view on the correlation of the signal and the TII spectrum can be selected. Furthermore, a view on the transition from the NULL period to the first datablock, a viewe on the impact of the channel on the signal, and a view on the frequency offsets of the carriers in the decoded signal can be selected. Finally, in a separate widget, the development over time of the SNR can be made visible.
  * automatic reconfiguration of services.
  * Detailed information on reception and selected service (SNR, bitrate, frequency, ensemble name, ensemble ID, subchannel ID, used CUs, protection level, CPU usage, program type, language, alternative FM frequency if available, 4 quality bars),
  * Frequency spectrum of the resulting audio as well as "strength" meters of the audio stream are made visible,
  * The TII data is mapped upon a transmitter's name, and display of TII (Transmitter Identification Information) data when transmitted,
  * Possibility of displaying a map with position(s) of received transmitter(s),
  * *Favorites* for easy switching of programs in different ensembles (see section *Favorites*),
  * *Dumping* of the input data of the DAB channel (Warning: produces large raw files!) into `.sdr` files or `.xml` file formats and playing them again later (see section on xml format),
  * Saving audio as uncompressed wave files, and saving *aac* frames from DAB+ services for processing by e.g. VLC,
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
	- untested UHD (anyone wants to help testing?)
	- Soapy, a renewed soapy interface driver is even able to map other samplerates than the required 2048000 (limited to the range 2000000 .. 3000000);
	- ExtIO (experimental, Windows only),
	- rtl_tcp servers.
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
While the 2.13 support for SDRplay devices is able to handle the RSP 1, RSP II, RSP Ia and RSP duo, the 3.0X support handles all SDRplay RSP's. It is recommended to use the 3.0X support library. Note further that when - on Windows - a 3.10 (or higher) library is installed (e.g. by installing SDRuno), the 2.13 library is not reachable.

Widgets and scopes
=======================================================================

![6.2](/qt-dab-6-main-widget.png)

The full GUI for Qt-DAB 6.40 consists of a handful of widgets; 
a single widget, the *main* widget is always visible and visibility of the
other wigets (spectrum widget, technical data widget and configuration-and-control widget) is stricly under user control by settings in the main widget.

The main widget of Qt-DAB provides all means for selecting a channel,
and selecting a service. 

![6.2](/qt-dab-6-slides.png)

Most DAB services carry one or more slides, these are made visible on the
main widget.
The technical widget - the visibility of which depends on the settings
in the main widget - gives full information on the selected audio service, as shown in the picture.

![6.1](/technical-widget.png)

While the main widget shows the services in the **currently selected** channel,
there is a separate widget for displaying **favorites**.
A  service name from the ensemble display can be added to the favorites
list by clicking on it with the right hand mouse button.

![6.2](/favorites-widget.png)

Different from previous versions, a **single widget**, the spectrum widget,
contains (almost) all of the scopes and displays.
It is set up as a tabbed widget - the 6 scopes show the various aspects of the DAB signal. 
Furthermore, it contains an IQscope,
showing the constellation of the decoded data or the constellation
of the data before decoding.
A waterfall scope shows the progress in time of the data
that is displayed in the selected scope.

The spectrum scope shows - in numbers - some quality indicators for the
raw DAB signal as well.

The progress indicator at the bottom shows the quality of the FIC decoding,
where FIC can be seen as the directory data of the contents of the DAB
transmission.

(Note that - obviously - the colors of the scopes can be set to different
colors than shown here).

![6.1](/spectrum-scope.png)

The *spectrum scope* shows the spectrum of the incoming DAB signal. One
sees clearly that the width of the signal is app 1.5 MHz. To the right one
sees the signal constellation.  The latter shows the mapping from the complex signals onto their real and imaginary components.

![6.1](/spectrum-ideal.png)

The ideal form of the spectrum - and the signal constellation as shown in the
IQScope - is not seen often.

![6.1](/qt-dab-correlation.png)

The correlation scope shows the correlation between the incoming signal and
some predefined data. It is helpful in finding the precise start of the
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
New are the selectors on the bottom line left. In previous versions right
clicking on an arbitrary service name in the emsemble display would start
a service with that name as background service. However, the default now is
that right clicking adds the service name to the favorites. Setting the
most left checkbox will revert this.
The second selector, a combobox with only a few items allows selection from
some experimental decoder implementations.

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
to your home directory.
(Due to licencing issues the code to upload a database is proprietary).

Adding the receiver coordinates is simple,  touch the button "coordinates"
on the configuration  and control widget and a small widget appears where the data can be entered.

Documentation
=================================================================

An extensive **user's guide** - in PDF format - for the 6.1 version can be found in the "docs" section of the source tree. The documentation contains a complete description of the widgets, of the values in the `.ini` file, on configuring for creating an executable (Linux), and even a complete description on how to add a device to the configuration.

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

Since it is common to avoid reading a manual, here are
the basic steps for the build process.
It is strongly advised to use qmake/make in the process, since the
number of configuration options is larger and selecting configuration
options is much easier.

Step 1
-----------------------------------------------------------------

- :information_source: Note that the sources for 6.30 are now in the subdirectory `qt-dab-s6` and for qt-dab-5.x in the subdirectory `qt-dab-s5` 
- Install required libraries, 
* :information: It turns out that in recent versions of Debian (and related) distributions the lib `qt5-default` does not exist as as separate library.
- It seems to be part of another of the qt5 packages that is installed.
- Be aware that different distributions store qt files on different locations, adapt the INCLUDEPATH setting in the `.pro` file if needed.

Step 2
-----------------------------------------------------------------

While there are dozens of configuration options, take note
of the following ones:

* Note on configuring DABsticks (i.e. RTLSDR type devices).
The Windows support library does not seem to be capable of closing
the library and reopening it on switching channels. Therefore
different versions exist for Linux and Windows.

:information_source: Note that for including "soapy" in the configuration, soapy  software should have been installed. Be w]aware that the current
version is developed on an Fedora box, the soapy library used does not seem compatible with the soapy library on Ubuntu.

:information_source: Note that "pluto" can be compiled in: as the other support programs, when the device is selected, the support program will (try to) read in the functions of the device library.

:-information_source: While device handler sources for UHD, Elad and Colibri
arfe available, the status of these handlers is different: for UHD it 
is experimental though not tested lately, for Elad and Colibri they were
in development once and incomplete.

For X64 PC's one may choose the option `CONFIG+=PC` (for selecting SSE instructions). If unsure, use `CONFIG+=NO_SSE`.

For letting the software show the transmitter and the azimuth, choose `CONFIG += tiiLib` (see step 4).

Step 3
-----------------------------------------------------------------

run `qmake` (variants of the name are `qt5-qmake`, `qmake-qt5`) which generates a `Makefile` and then run `make`. 

Step 4
-----------------------------------------------------------------

Unpack file "tiiFile.zip", and copy the resulting file `.txdata.tii` from the *library* subdirectory (which contains the database data for finding the transmitter's name and location) into the user's home directory.) If Qt-DAB cannot find the file, it will just function without showing the names and without "maps" option.

If running on an x64 PC or *bullseye* on the RPI you might consider to install `libtii-lib.so` (to be found in the *library* subdirectory) in `/usr/local/lib`. Note that to avoid confusion, it named "libtii-lib.so-rpi" and should be renamed to the libtii-lib.so.

:information_source: Note however that this library needs `curl` to be installed and source code for `libtii-lib.so` is not free. `libtii-lib.so` contains functionality for uploading a new database version (the "load" button on the configuration widget). If Qt-DAB cannot find the library, it will just function without the additional functionality.

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

A Note on soapy
================================================================
Soapy is not configured for the AppImage. There is an incompatibility
between the Soapy library on Ubuntu 20 - where the AppImages are built - and my development environment.

A Note on previous versions
=================================================================
Previous versions V5.X and V4.X will - for the time being - be maintained. 
Note that the different versions use the same sourcetree, the - almost -
only difference being the GUI and its control.

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

