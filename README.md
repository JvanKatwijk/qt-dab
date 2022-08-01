# Qt-DAB-4.4.2 [![Build Status](https://travis-ci.com/JvanKatwijk/qt-dab.svg?branch=master)](https://travis-ci.com/JvanKatwijk/qt-dab)

Qt-DAB-4.4-2 is software for Linux, Windows and Raspberry Pi for listening to terrestrial Digital Audio Broadcasting (DAB and DAB+). Qt-DAB is accompanied by its little sister dabMini, built on the same set of sources.

![4.4](/qt-dab-screen.png?raw=true)

----------------------------------------------------------------

Thanks to Richard Huber, Qt-DAB can be compiled on the Mac

----------------------------------------------------------------
What is new in Qt-DAB 4.4.2
----------------------------------------------------------------

The difference between 4.4 and 4.4.2 is is the map handling.

I was completely bored with searching on a map where the transmitter
location was that was detected by the Qt-DAB software.
So, I modified the http handler and the map from the 1090 software
(all rights acknowledged) and added a map feauture.
(Of course, the map extension only makes sense if the previous extension,
transforming TII data into readbable transmitter information, is on.)

To aid in adjusting the map, the map is selected such that the
center of the map is on the "home" coordinates that you gave
to compute distances etc.

So, if the software detects that the TII to transmitter info is 
part of the implementation, a button shows on the main widget, with
which the map service can be switched on and off.

The html/javascript text of the map file is now encoded as numbers
and included in a table in the sources.
A separate program exists to transform the "javascript/html" into
such a table.

After 4.4.1 was released a pretty large number of small changes was
made to the map, that is why 4.4.1 was declared obsolete and replaced
by 4.4.2

----------------------------------------------------------------
Building an executable for qt-dab: a few notes
----------------------------------------------------------------

While for Linux-x64 and Windows there are precompiled versions, there
may be reasons to build an executable. Building an executable is not
very complicated,  it is described in detail in the manual.
Since it is customary to avoid reading a manual, here are the
basic steps for the build process.

Step 1:	

	Note that the sources for 4.4.2 are now in the subdirectory qt-dab-4
	Install required libraries, see section 5.5.3 (page 29) of the manual.
	It turns out that in recent versions of Debian (and related) distributions
	the lib *qt5-default* does not exist as as separate library.
	It seems to be part of another of the qt5 packages that is installed.
	Be aware that different distributions store qt files on different
	locations, adapt the INCLUDEPATH setting in the ".pro" file if needed.

Step 2:

	While there are dozens of configuration options, take note of the
	following ones:

	for devices "pluto", "pluto-rxtx" and "soapy" software should have
	been installed, so leave them commented out when not available.
	Note that "pluto-2" can be compiled in: when the device is
	selected, it will (try to) read in the functions of the device
	library.
	For other devices, e.g. sdrplay, airspy, etc, configuration does not
	require availability of the drivers (of course using the device does)

	For X64 PC's one may choose the option "CONFIG+=PC" (for selecting SSE
	instructions). If unsure, use "CONFIG+=NO_SSE".

	For letting the software show the transmitter and the azimuth,
	choose  "CONFIG += tiiLib".

	Note that the file "converted_map.h" is a generated file that contains
	a binary version of the HTML/javascript code for the server.

step 3:

	run qmake (variants of the name are qt5-qmake, qmake-qt5)
	which generates a Makefile and then run "Make". 

step 4:

	Install the file "tiiFile.zip" (after unpacking) in the user's home
	directory (filename .txdata.tii). The file contains the
	database data for finding the transmitter's name and location.
	If the file cannot be found, Qt-DAB will just function without
	showing the names.

	If running on an x64 PC or *bullseye* on the RPI you might consider
	to install *libtii-lib.so* in "usr/local/lib" from "dab-maxi/library".
	Note however that that that library needs "curl" to be installed
	and source code for *libtii-lib.so* is not free.
	*libtii-lib.so* contains functionality for uploading
	a new database version (the "load" button on the configuration widget).
	If Qt-DAB cannot find the library, it will just function without
	the additional functionality.

Note:

	Building a version on a fresh install of "bullseye" on the RPI gave
	a version that wouldn't run: The *Qt_PLUGIN_PATH* was not set.
	Setting it as given below solved - for me - the problem

	Qt_5= /usr/lib/arm-linux-gnueabihf/qt5
	export QT_PLUGIN_PATH=$Qt_5/plugins

------------------------------------------------------------------
Table of Contents
------------------------------------------------------------------

* [Introduction](#Introduction)
* [Features](#features)
* [Widgets and scopes](#widgets-and-scopes-for-qt-dab)
* [dabMini and duoreceiver](#dabMini-and-duoreceiver)
* [Documentation](#documentation)
* [configuration settings](#Configuration-settings)
* [Presets](#presets-for-qt-dab-and-dab-mini)
* [Saving gain settings](#saving-gain-settings)-
* [Colors](#colors-for-qt-dab)
* [Scanning and the skip table](#scanning-and-the-skip-table)
* [Saving synamic label texts](#Saving-dynamic-label-texts)
* [Scheduling option](#Scheduling-option)
* [Installation on Windows](#Installation-on-windows)
* [Installation on Linux x64](#Installation-on-linux-x64)
* [Interfacing to another device](#Interfacing-to-another-device)
* [Using user-specified bands](#Using-user-specified-bands)
* [xml-files and support](#xml-files-and-support)
* [EPG-Handling](#epg-handling)
* [Recording the SNR](#recording-the-snr)
* [Pluto device and fm transmission](#pluto-device-and-fm-transmission)
* [Copyright](#copyright)

------------------------------------------------------------------
Introduction
------------------------------------------------------------------

**Qt-DAB-4.4** is a rich implementation of a DAB decoder for use on Linux and Windows based PC's, including some ARM based boards, such as the Raspberry PI 2 and up. It can be used with a variety of SDR devices, including DABsticks,
all models of the SDRplay, AIRspy etc.

Precompiled versions for Linux-x64 (AppImage) and Windows (an installer)
are available. 
------------------------------------------------------------------
Features
------------------------------------------------------------------

  * DAB (mp2) and DAB+ (HE-AAC v1, HE-AAC v2 and LC-AAC) decoding
  * MOT SlideShow (SLS)
  * Dynamic Label (DLS) and the possibility of saving dynamic Labels - augmented with channel and time info - in a file,
  * Both DAB bands (and user defined bands) are supported: 
  	* VHF Band III (default),
   	* L-Band (obsolete now),
	* a user defined Band
  * Modes I, II and IV (Mode I default, Modes II and IV obsolete, but can be set in the ".ini" file,
  * Views on the signal: spectrum view incl. constellation diagram, correlation result, TII spectrum and the SNR over time,
  * automatic reconfiguration of services,
  * Detailed information on reception and selected service (SNR, bitrate, frequency, ensemble name, ensemble ID, subchannel ID, used CUs, protection level, CPU usage, program type, language, alternative FM frequency if available, 4 quality bars),
  * If configured, the TII data is mapped upon a transmitter's name,
  and  display of TII (Transmitter Identification Information) data when transmitted,
  * Possibility of displaying a map with position(s) of received transmitter(s),
  * *Presets* for easy switching of programs in different ensembles (see section *Presets*),
  * *Dumping* of the input data of the DAB channel (Warning: produces large raw files!) into \* sdr files or xml file formats and playing them again later (see section on xml format),
  * Saving audio as uncompressed wave files, and Saving aac frames from DAB+ services for processing by e.g. VLC,
  * Saving the ensemble content description: audio and data streams, including almost all technical data) into a text file readable by e.g *LibreOfficeCalc*
  * Advanced scanning function (scan the band, show the results on the screen and save a detailed description of the services found in a file),
  * ip output: when configured the ip data - if selected - is sent to a specificied ip address (default: 127.0.0.1:8888),
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
	- ExtIO (expertimental, Windows only),
	- rtl_tcp servers.
  * Always supported input from:
   	- prerecorded dump (*.raw, *.iq and *.sdr),
	- xml format files.
  * Clean device interface, easy to add other devices.
  * Scheduling the start of (channel:service) pairs or operations as frame dump or audio dump, even for days ahead.
  * Showing the name of the transmitter received as well as the distance to the receiver and the azimuth.
  * background services. Since 4.351 it is possible to run an arbitray number of DAB+ audioservices (from the current ensemble) as background service with the output sent to a file.

Partly implemented:

  * TPEG: when configured, TPEG messages are being sent to a TCP port; sources for a simple client are part of the source distribution.
  * Journaline (an untested Journaline implementation is part of the sources).
  * Other bands than used for terrestrial broadcasting in Europe (like DAB over cable)

Note:
While the 2.13 support for SDRplay devices is able to handle
the RSP 1, RSP II, RSP Ia and RSP duo,
the 3.0X support handles all SDRplay RSP's.
It is recommended to use the 3.0X support library.

------------------------------------------------------------------
Widgets and scopes for Qt-DAB
------------------------------------------------------------------

Qt-DAB always shows a main widget; a number of  **optional**
widgets is visible under user control.
The whole set of widgets is shown below

![Qt-DAB main widget](/qt-dab-screen.png?raw=true)

The buttons and other controls on the main widget are equipped with
*tool tips* briefly explaining the (in most cases obvious) function
of the element (the tooltip on the copyright label shows (a.o) the date the executable was generated.)


![4.](/qt-dab-screen-2.png?raw=true)

The elements in the **left part** of the widget, below the list of services,
 are concerned with selecting a channel and a service. To ease operation the channel selector is augmented with a "-" and a "+" button for selecting the previous resp. next channel.

To ease selection of a service, a second pair of "-" and "+" buttons
is available, now for selecting the previous resp. the next service on the list.

Some general data is displayed on the top right part of the widget, 
such as run time and current time,
frequency offset and (overall) processor load (note that the "current time"
is extracted from the DAB data, playing a recorded transmission shows the
time of the transmission).

In the middle part, name of the ensemble and name of the selected service
are displayed, complemented with the text of the dynamic label.

Some data on the selected service - if any - can be found on
a separate widget, the "Technical Data" widget (*Detail* button).

![Qt-DAB main widget](/qt-dab-technical-widget.png?raw=true)

Most selectors are concentrated on the bottom part of the right side
of the widget. Buttons to make scopes visible, to store input and or
output into a file, to select input device and the audio and to
scan and store a description of the ensemble are in that section.

![qt-dan controls ](/qt-dab-main-controls.png?raw=true)

Other widgets are

  * a widget with controls for the attached device,
  * a widget for additional configuration settings,
  * a widget showing the technical information of the *selected service* as well
as some information on the quality of the decoding, 
  * a widget showing the spectrum of the received radio signal, the constellation of the decoded signal and some quality parameters,,
  * a widget showing the spectrum of the NULL period between successive DAB frames from which the TII is derived,
  * a widget showing the correlations response(s) from different transmitters in the SFN,
  * a widget showing the development - over time - of the SNR,
  * if configured and data is detected, the time table for the current audio service

![Qt-DAB totaal](/qt-dab-scan.png?raw=true)

Another  widget shows when running a *scan*; the widget 
shows the contents of the ensembles found in the selected channels.
Since 3.5 the
possibility exists to save a detailed description of the services
in the different channels, in a format easy to process with LibreOffice
or comparable programs (a csv file).

Depending on a setting in configuration widget, a logo or slide, transmitted
as Program Associated Data with the audio transmission, will be shown here or on a separate widget.

---------------------------------------------------------------------
dabMini and duoreceiver
---------------------------------------------------------------------

**dabMini** is a version of the DAB decoder with a minimal GUI.
dabMini is built from the same sources as Qt-DAB, so
modifications to the Qt-DAB sources are automatically applied to
dabMini as well.

![Qt-DAB dabMini](/dab-mini.png?raw=true)

Other than Qt-DAB, there is *no* device selector. On program start up
the software polls the configured devices, the first one that seems OK
is selected.

The GUI contains some selectors for setting device properties,
depending on the selected device (usually gain, lna and agc).
The picture shows the program using
an SDRplay device, with agc selected and a selector for
lna state. Since the agc is selected. there is no need for the if gain selector.

Current versions of dabMini supports the **dlText**
and **scheduler function** as implemented for Qt-DAB.

**duoreceiver** is derived from dabMini and from other software for
FM decoding. Since the SDR devices are covering both the FM broadcast
band and BAND III, where DAB transmissions are, there were questions
why dabMini could not be extended to cover FM transmissions as well.
**duoreceiver** covers both bands and allows easy switching between FM and DAB.

![overview](/duoreceiver-1.png?raw=true)

--------------------------------------------------------------------
Documentation
--------------------------------------------------------------------

An extensive "user's guide" - in pdf format - can be found in the "docs"
section of the source tree. The documentation contains a complete
description of the widgets, of the values in the ".ini" file,
on configuring for creating an executable (Linux), and even a
complete description on how to add a device to the configuration.

![Qt-DAB documentation](/qt-dab-manual.png?raw=true)

---------------------------------------------------------------------------
Configuration settings
-------------------------------------------------------------------------------

Many settings are maintained between program invocations in a
so-called ".ini" file, ususally "qt-dab.ini", stored in the home directory.
A complete description of the settings can be found in the user's manual
(in the subdirectory *docs*).
Touching the **config** button on the main widget will show (or hide)
a widget where a number of settings can be adapted.

----------------------------------------------------------------------
Presets for Qt-DAB and dabMini
----------------------------------------------------------------------

A *preset* option is available to handle selection of
**favorit** services. Touching the name of the currently selected
audio service with the right hand mouse button will save the
"channel:serviceName" pair in the preset list.
Obviously, selecting a service in the preset list instructs the software
to switch to the channel for the service and the service.

The presets are stored in an xml file, `.qt-dab-presets.xml`.
Removing an entry in the presetlist is by **right-clicking with the
mouse** on the entry.

----------------------------------------------------------------------
Storing gain settings
----------------------------------------------------------------------

In each configuration, Each channel for each device has its
optimal gain setting. That is why the ".ini" file stores the
gain settings per device per channel and - obviously - restores
it when selecting a channel.

----------------------------------------------------------------------
Colors for Qt-DAB
----------------------------------------------------------------------

There are 20 push buttons, 18 on the main GUI, 2 on the
technical data widget. Selecting a color setting for a button is
now made easy: right clock with the mouse on the button, and a
small menu appears on which the color for the button can be
selected (one of a predefined list), and next a similar menu appears
for selecting the color of the text on the button.

The settings are saved in the ".ini" file.

For setting the colors on the scopes, right click on the scope. Three
questions will be asked: what will be the color of the display (background),
what will be the color of the grid and what will be the color of the curve.

----------------------------------------------------------------------
Scanning and the skip table
----------------------------------------------------------------------

As known, Qt-DAB provides a possibility of scanning the band. Band III
contains 39 channels, so - depending on your position - there is
quite a number of channels where no DAB signal is to be found.

Qt-DAB has an extended mechanism to skip specified channels during a scan,
a so-called **skipTable**.
The configuration widget contains a button to make the **skipTable**
visible. Such a skipTable shows all channels in the selected band, 
each channel labeled with a field containing a "+" or a "-" sign.
Double clicking on the field will invert its setting.
Obviously. skipTables will be maintained between program invocations.

When DX-ing, one wants to direct the antenna to different countries
in different directions.
The configuration widget contains a button **skipFile**,
when touched a file selection menu appears where one can select a skipfile.
**If the file does not exist yet, it will be created.**
If one cancels the file selection, the default skipTable will be used,
the same table that is used when no skipFile is selected.

----------------------------------------------------------------------
Saving dynamic label texts
----------------------------------------------------------------------

Based on user requests an option is implemented to store the texts as
emitted as dynamic label texts, into a file. The configuration widget
contains - at the bottom - a button that - when touched - shows a
file selection menu.
The texts are preceded with a time indication and a service name.

	12C.NPO Radio 5  2021-10-08 11:33:00  Rolling Stones - Jumpin' Jack Flash
	12C.NPO Radio 5  2021-10-08 11:36:38  NPO Radio 5 - Arbeidsvitaminen - AVROTROS
	12C.NPO Radio 5  2021-10-08 11:36:00  Kenny Rogers - The Gambler
	12C.NPO Radio 5  2021-10-08 11:40:19  Fools Garden - Lemon Tree

Endless repetitions are avoided in the saved text.
If saving dynamic label texts is selected as "command" in the scheduler,
the texts will be stored in a textfile with a generated name.

-----------------------------------------------------------------------
Scheduling option
-----------------------------------------------------------------------

The "alarm" facility is replaced by a more general *scheduling* facility.
Touching the schedule button on the main widget
shows a list of services to select from (the services in the currently
selected channel, and the list of services in the preset list).
A user specified time today or the next days can be linked to the
selected service.

As additional feature, some operations can be scheduled as well:

 * exit or nothing, with obvious semantics;
 * audiodump, to schedule starting or stopping dumping the audio of the
currently selected audio service;
 * framedump, to schedule starting or stopping dumping the AAC segments
of the currently selected service.
 * dlText, to schedule starting or stopping recording the dynamic label text.

Note that selecting a different audio service will automatically stop
dumping the audiodump and the framedump activity, the recording of
the dynamic label text - if selected - is not affacted.

![scheduler](/scheduler.png?raw=true)

Specifying a time and day is stored in a table, maintained between
program invocations.  On restart
all schedule elements on passed dates are removed.

------------------------------------------------------------------
Installation on Windows
--------------------------------------------------------------------

For windows an  **installer** can be found in the releases section, https://github.com/JvanKatwijk/qt-dab/releases. The installer will install the executable as well as required libraries.

The installer will also call the official installer for the dll implementing
the 2.3 api for getting access to the SDRplay devices.

------------------------------------------------------------------
Installation on Linux-x64
------------------------------------------------------------------

For Linux-x64 systems, an **appImage** can be found in the releases section,
http::github.com/JvanKatwijk/qt-dab/releases. The appImage contains
next to the executable qt-dab program, the required libraries.

Of course it is possible to generate an executable,  the manual
contains a complete script for Ubuntu type Linux versions.

-----------------------------------------------------------------------
Interfacing to another SDR device
-----------------------------------------------------------------------

There exist - obviously - other devices than the ones supported
here. Interfacing another device is not very complicated,
it might be done using the "Soapy" interface, or one might
write a new interface class.

A complete description of how to interface a device to Qt-DAB
is given in the user's manual.

------------------------------------------------------------------------
Using user specified bands
------------------------------------------------------------------------

While it is known that the DAB transmissions are now all in Band III,
there are situations where it might is desirable to use other frequencies.
Specify in a file a list of channels, e.g.

	jan	227360
	twee	220352
	drie	1294000
	vier	252650

and pass the file on with the "-A" command line switch.
The channel name is just any identifier, the channel frequency is
given in KHz. Your SDR device obviously has to support the frequencies
for these channels.

-------------------------------------------------------------------------
xml-files and support
-------------------------------------------------------------------------

Clemens Schmidt, author of the QiRX program and me defined a format
for storing and exchanging "raw" data: xml-files.
Such a file contains in the first bytes - up to 5000 - a description
in xml - as source - of the data contents. This xml description
describes in detail  the coding of the elements.
As an example, a description of data obtained by dumping AIRspy
input. 

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

The device handlers in Qt-DAB support the generation of 
such an xml file.

While the current implementation for reading such files is limited to
a single data block, the reader contains a *cont* button that, when
touched while playing the data, will cause continuous playing of the
data in the data block.

![Qt-DAB with xml input](/qt-dab-xml.png?raw=true)

The picture shows the reader when reading a file, generated from raw
data emitted by the Hackrf device.

----------------------------------------------------------------------
EPG-Handling
----------------------------------------------------------------------

In the current implementation, an EPG Handler is automatically started
as backgroujnd service whenever a service is recognized as carrier of
EPG data.
The handler will collect information about the program guides for the
various services, and when collected, allow the user to view it
(it will take some time to have the relevant data collected).

Whenever the software has collected time table information of
a given service, the technical Data widget will show, when selecting
the service, a button labeled **timeTable** .

![epg data](/qt-dab-epg-data.png?raw=true)

The software is experimental though and - at least here - the times
on the time table in UTC.

-----------------------------------------------------------------------
Recording the SNR
-----------------------------------------------------------------------

A sampled DAB frame starts with a null period of app 2600 samples, followed by
76 blocks (each about 2500 samples) with data. SNR is computed as
the ratio between the amplitudes in the data blocks and the amplitudes of the
samples in the null period.

The development of the SNR over time can be made visible in the SNR widget.
As configuration option, the widget can be equipped with a **dump** button,
touching the button will show a menu for file selection. Once a file
is selected, the results of the computations are not only shown, but recorded
in the file as well.

To view the recording, a simple utility is made to make the contents
of the recording visible. Sources for the utility are to be found
in the (sub)directory **dumpviewer**.

![dumpViewer](/dumpViewer.png?raw=true)

The picture shows the variations in the SNR when moving the antenna
and - the dip - when switching antennas.

-----------------------------------------------------------------------
Pluto device and stereo FM transmission
-----------------------------------------------------------------------

As (probably) known, the **Adalm Pluto** device has receive and transmit
capabilities.  For configuring pluto there are three options:

 * pluto, whichn requires the support libraries to be installed;

 * pluto-2, which - as most other device handlers - will try to read in the required functions from the library when selected, and

 * pluto-rxtx, which supports transmitting the audio as received from a DAB sevrice in FM stereo on a user specified frequency. It requires the libad9361 and libiio to be installed.

The precompiled versions were configured with "pluto-2".

As default, the transmit frequency is 110 MHz, however, the user can specify
the frequency (obviously within the range supported by the Adalm-Pluto)
as command line parameter: starting the program from the command line
with as parameter

	-F XXX

will instruct the software to start the
transmitter of the Adalm pluto - if the pluto-rxtx is the selected device -
on the given frequency (specified in KHz)

![dab2fm](/dab2fm.png?raw=true)

-----------------------------------------------------------------------
Copyright
------------------------------------------------------------------------

	Copyright (C)  2016 .. 2022
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	The Qt-DAB software is made available under the GPL-2.0.
	The SDR-J software, of which the Qt-DAB software is a part, 
	is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

