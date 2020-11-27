# Qt-DAB-3.7 [![Build Status](https://travis-ci.org/JvanKatwijk/qt-dab.svg?branch=master)](https://travis-ci.org/JvanKatwijk/qt-dab)

Qt-DAB-3.7 is software for Linux and Raspberry Pi for listening to terrestrial Digital Audio Broadcasting (DAB and DAB+). Qt-DAB is accompanied by its little sister dabMini and brother dab-2 for experimenting, all built on the same set of sources.

![overview](/qt-dab-screen.png?raw=true)

------------------------------------------------------------------
Table of Contents
------------------------------------------------------------------

* [Introduction](#introduction)
* [Features](#features)
* [Widgets and scopes](#widgets-and-scopes-for-qt-dab)
* [Documentation](#documentation)
* [Presets](#presets-for-qt-dab-dabmini-and-dab-2)
* [Colors](#Colors-for-Qt-DAB)
* [History](maintaining-history-for-qt-dab-and-dab-2)
* [Comment on some settings](#comment-on-some-settings)
* [Obsolete properties](#obsolete-properties)
* [Installation on Windows](#installation-on-windows)
* [Installation on Linux](#installation-on-linux-pc-and-rpi)
* [Configuring](#configuring)
* [Interfacing to another device](#interfacing-to-another-device)
* [Using other bands than Band III or L Band](#using-other-bands-than-band-iii-or-l-band)
* [xml-files and support](#xml-files-and-support)
* [EPG-Handling](#EPG-Handling)
* [Recording the SNR](#Recording-the-SNR)
* [Copyright](#copyright)

------------------------------------------------------------------
Introduction
------------------------------------------------------------------

**Qt-DAB-3.7** is a rich implementation of a DAB decoder for use on Linux and Windows based PC's, including some ARM based boards, such as the Raspberry PI, both 2 and 3.

It provides an abundant amount of selectors and displays, most of
which can be switched off, but are of interest for those who want to see aspects of the DAB signal.

While Qt-DAB provides an extensive **scan** function, 
a separate **dab-scanner** exists as well that allows
for a continuous scanning of selected channels in a given band. Results are 
written in a text file, formatted for use with *LibreOffice Calc* and
comparable programs. (Note that in **Qt-DAB-3.7** the scan function
has an option to save detailed data in a text file, compatible with e.g.
LibreOffice.)

Qt-DAB makes extensive use of a GUI. Personally, grown up in the time of
"command lines", from time to time I just prefer a simple command line
driven device. Therefore, a **terminal-DAB-xxx** version was developed, a
simpler version, just for listening to a DAB service, making use of 
command line

![Qt-DAB terminal DAB](/qt-dab-terminal-dab.png?raw=true)

For those who are nostalgic to the era of nice wooden radios with glowing tubes,
a program exists - for the Pluto - to receive DAB and transmit a selected
service in FM stereo with the "dynamic label" sent as RDS.

------------------------------------------------------------------
Features
------------------------------------------------------------------

  * DAB (mp2) and DAB+ (HE-AAC v1, HE-AAC v2 and LC-AAC) decoding
  * MOT SlideShow (SLS)
  * Dynamic Label (DLS) 
  * Both DAB bands supported): 
  	* VHF Band III (default),
   	* L-Band (obsolete now)
  * Modes I, II and IV (Mode I default, Modes II and IV obsolete, but can be set in the ".ini" file)m
  * Views on the signal: spectrum view incl. constellation diagram, correlation result, TII spectrum and the SNR over time.
  * Detailed information for selected service (SNR, bitrate, frequency, ensemble name, ensemble ID, subchannel ID, used CUs, protection level, CPU usage, program type, language, alternative FM frequency if available, 4 quality bars),
  * Detailed information for *other* services by right-clicking on their name (bitrate, subchannel ID, used CU's protection level, program type),
  * Automatic display of TII (Transmitter Identification Information) data when transmitted,
  * *Presets* for easy switching of programs in different ensembles (see section *Presets*),
  * Dumping of the input data of the DAB channel (Warning: produces large raw files!) into \* sdr files and playing them again later,
  * Dumping the input in xml format (see section on xml format),
  * Saving audio as uncompressed wave files,
  * Saving aac frames from DAB+ services for processing by e.g. VLC,
  * Saving the ensemble content description: audio and data streams, including almost all technical data) into a text file readable by e.g *LibreOfficeCalc*
  * Scanning function (scan the band, show the results on the screen and save a detailed description of the services found in a file),
  * ip output: when configured the ip data - if selected - is sent to a specificied ip address (default: 127.0.0.1:8888),
  * TPEG output: when configured the data is sent to a specified ip address,
  * EPG detection and building up a time table,
  * Supports inputs from 
	- Adalm Pluto,
   	- SDR DAB sticks (RTL2838U or similar), 
	- HACKRF One, 
  	- Airspy, including Airspy mini,
  	- SDRplay ( RSP I, RSP II, RSP Duo and RSP Dx), with separate entries for v2 and v3 library
	- limeSDR, 
	- Soapy (experimental, Linux only), 
	- ExtIO (expertimental, Windows only),
   	- prerecorded dump (*.raw, *.iq and *.sdr),
	- xml format files, and
	- rtl_tcp servers.
  * Clean interface to add other devices, see below.

Not yet or partly implemented:

  * DMB (Audio and Video)
  * TPEG: when configured, TPEG messages are being sent to a TCP port; sources for a simple client are part of the source distribution.
  * Journaline (an untested Journaline implementation is part of the sources).
  * Other bands than used for terrestrial broadcasting in Europe (like DAB over cable)

Note:
While the 2.13 support for SDRplay devices is able to handle
the RSP 1, RSP II, RSP Ia and RSP duo, for handling the RSP-Dx one
needs 3.06 (3.07) support

------------------------------------------------------------------
Widgets and scopes for Qt-DAB
------------------------------------------------------------------

Qt-DAB has a main widget and a number of  **optional**
widgets, visible or invisible under user's control

  * a widget with controls for the attached device,
  * a widget for additional configuration settings.
  * a widget showing the technical information of the *selected service* as well
as some information on the quality of the decoding, 
  * a widget showing the spectrum of the received radio signal and the constellation of the decoded signal,
  * a widget showing the spectrum of the NULL period between successive DAB frames from which the TII is derived,
  * a widget showing the correlations response(s) from different transmitters in the SFN,
  * a widget showing the development - over time - of the SNR
  * if configured and data is detected, the time table for the current audio service

![Qt-DAB totaal](/qt-dab-totaal.png?raw=true)

Another - an eight' - widget shows when running a *scan*; the widget will show the contents of the ensembles found in the selected channel. In 3.5 the
possibility was created to save a detailed description of the services
in the different channels, in a format easily to process with LibreOffice
or comparable programs.

While the main window is **always** shown, visibility of the others is
under user control, the main widget contains a button for each..

![Qt-DAB main widget](/qt-dab-main-widget.png?raw=true)

The buttons and other controls on the main widget are equipped with
*tool tips* briefly explaining the (in most cases obvious) function
of the element. (The tooltip on the copyright label shows (a.o) the date the executable was generated.)

The elements in the **left part** of the widget, below the list of services,
 are concerned with selecting a channel and service. 

To ease operation the channel selector is augmented with a "-" and a "+"
button for selecting the previous resp. next channel.

To ease selection of a service, a second pair of "-" and "+" buttons
is available, now for selecting the previous resp. the next service 
on the list.

Some data on the selected service - if any - is to be found on
a separate widget, the "technical data" widget.
This widget will show where the data for the
service is to be found in the DAB frames, and how it is encoded.

Depending on a setting in the ".ini" file, a logo or slide, transmitted
as Program Associated Data with the audio transmission, will be shown here or on a separate widget
(the setting can be modified from the configuration widget).

The further selectors are concentrated on the bottom part of the right side
of the widget. Buttons to make scopes visible, to store input and or
output into a file, to select input device and the audio and to
scan and store a description of the ensemble are in that section.

![qt-dan controls ](/qt-dab-main-controls.png?raw=true)

During *scanning*, a separate window will be shown with the results
of the scan as shown in the picture. Note that what is shown is a *subset*
of what will be recorded if a choice is made to save the output of the scan. 

![Qt-DAB scan result](/qt-dab-scan-result.png?raw=true)

--------------------------------------------------------------------
Documentation
--------------------------------------------------------------------

An extensive "user's guide" - in pdf format - can be found in the "docs"
section of the source tree. The documentation contains a complete
description of the widgets, of the values in the ".ini" file,
on configuring for creating an executable (Linux), and even a
complete description on how to add a device to the configuration.

![Qt-DAB documentation](/qt-usersguide.png?raw=true)

----------------------------------------------------------------------
Presets for Qt-DAB
----------------------------------------------------------------------

A *preset* selector is available to allow storing and selecting
"favorit" services.
Note that the services in the preset list are encoded as
"channel:serviceName" pair: it sometimes happens that a
service appears in more than one ensemble
(as example the "Omroep West" service appears in channels 5B and 8A.)

The presets are stored in an xml file, `.qt-dab-presets.xml'.

*Adding* a service to the *presets* is simply by *clicking with the right mouse
button on the name of the service that is currently selected in the
servicelist* (recall that clicking with the *left* mouse button
well select the service with that name).

Of course, one is also able to *remove* an entry from the presets.
To do this, select the entry (by putting the curson on it without
clicking any of the mouse buttons) and press the *shift* and the *delete*
button on the keyboard simultaneously.

----------------------------------------------------------------------
Colors for Qt-DAB
----------------------------------------------------------------------

Setting colors is rather personal, that is why Qt-DAB provides
the possibility of setting a color to buttons and displays.
(in total there are 19 push buttons, 16 on the main GUI, 3 on the
technical data widget).

To ease life a little, selecting a color setting for a button is
now made easy: right clock with the mouse on the button, and a
small menu appears on which the color for the button can be
selected (one of a predefined list), and next a similar menu appears
for selecting the color of the text on the button.

The settings are saved in the ".ini" file.

For setting the colors on the scopes, right click on the scope. Three
questions will be asked: what will be the color of the display (background),
what will be the color of the grid and what will be the color of the curve.

---------------------------------------------------------------------------
Maintaining History
---------------------------------------------------------------------------

Qt-DAB-3.7 saves all service names found.
The list of Pairs "Channel:serviceName"
will be made (in)visible when touching the appropriate button (the
one labeled with "list").

The data in stored in a file in xml format.
The *history* can be cleared by a click of the right mouse button,
clicking on a channel:servicename combination with the left
mouse button will cause the QT-DAB software to attempt to set the channel and
select the name.

---------------------------------------------------------------------------
Comment on some  configuration settings
-------------------------------------------------------------------------------

Various settings are maintained between program invocations in a
so-called ".ini" file. 
Typical settings are the channel/service combination at program termination,
values, used to initialize the software at program start up and the colors
for buttons and displays.
A complete description of the settings can be found in the user's manual
(in the subdirectory *docs*).

-------------------------------------------------------------------------
Obsolete properties
-------------------------------------------------------------------------

The current DAB standard eliminated the support for Modes other than Mode 1 and Bands other than VHF Band III. The Qt-DAB implementation still supports these features, however, since they are obsolete, there are no controls on the GUI anymore (the control for the Mode was already removed from earlier versions). 

Explicitly setting the Mode and/or the Band is possible by
including appropriate command lines in the ".qt-dab.ini" file.

For the *Mode*, one will add/set a line

	dabMode=Mode x, where x is either 1, 2 or 4

For the *Band*, one will add/set a line

	dabBand=band, where band is either VHF Band III or L_Band

------------------------------------------------------------------
Installation on Windows
--------------------------------------------------------------------

Development of Qt-DAB and related software is on Linux, 
the cross compilation facilities of MINGW make it possible to
cross compile the software for windows.

For windows *two*  **installers** can be found in the releases section, https://github.com/JvanKatwijk/qt-dab/releases. The installer will install the executable as well as required libraries.

The installer will also call the official installer for the dll implementing
the 2.3 api for getting access to the SDRplay devices.

------------------------------------------------------------------
Installation on Linux-x64
------------------------------------------------------------------

For Linux-x64 systems, an **appImage** is available. appImages
are automatically generated when pushing modifications to the software
in Github.

The appImage can be found in "https://github.com/JvanKatwijk/qt-dab/releases".
The appImage is configured for

	* RTL_TCP
	* AIRSPY
	* SDRPLAY
	* SDRPLAY_V3
	* LIMESDR
	* HACKRF
	* PLUTO

Of course it is possible to generate an executable, the 
aforementioned user's manual
contains a complete script that can be used to install all required
libraries, download the sources and build an executable on an Ubuntu
(Debian) based system.

Since Buster, the current system on the RPI's, has the same roots
as Ubuntu, the script for Ubuntu can be used to install Qt-DAB
on an RPI.

Note that the source tree contains two variants next to the Qt-DAB program,
a **dabMini** program, a dab decoder with a minimal interface, and **dab-2**
an experimental variant. Both use large parts of the same sources as Qt-DAB does.
 
For the dabMini, an appImage is available for use on an x64 based
Linux system, and the user's manual contains a complete
script, tested on an RPI 2 and 3.

**dab-2** is experimental and not - officially - supported

----------------------------------------------------------------------
Configuring 
----------------------------------------------------------------------

The  user's guide contains a detailed description of the configuration
options (mainly choices for including or excluding a device).

-----------------------------------------------------------------------
Interfacing to another device
-----------------------------------------------------------------------

There exist - obviously - other devices than the ones supported
here. Interfacing another device is not very complicated,
it might be done using the "Soapy" interface, or one might
write a new interface class.

While the handling of devices in dabMini and Qt-DAB is - apart
from handling the user interaction - the same,
it differs from the way device handling is done in dab-2. The description
here relates to device interfacing in Qt-DAB (the handler for the dabMini
differs since it lacks a separate widget for the control).

Device handlers are implemented as a class, derived from
the class *deviceHandler*. Only a few functions have to
be implemented.

A complete description of how to interface a device to Qt-DAB
is given in the user's manual.

------------------------------------------------------------------------
Using other bands than Band III or L Band
------------------------------------------------------------------------

While it is known that the DAB transmissions are now all in Band III,
there are situations where it might be desirable to use other frequencies.
If you want to experiment with a modulator, connected to an SDR device
on different frequencies than the default one (or you want just to
have a restricted number of channels from Band III or L Band), Qt-DAB
offers a possibility to specify a list of channels to be used.
Specify in a file a list of channels, e.g.

	jan	227360
	twee	220352
	drie	1294000
	vier	252650

and pass the file on with the "-A" command line switch.
The channel name is just any identifier, the channel frequency is
given in KHz.

-------------------------------------------------------------------------
xml-files and support
-------------------------------------------------------------------------

Clemens Schmidt, author of the QIRX program and me defined a format
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

The device handlers in Qt-DAB-3.X support the generation of 
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

In Qt-DAB-3.7 a first step is made to implement support for
EPG (Electronic Program Guide).

In the current implementation, an EPG Handler is automatically started
whenever within an ensemble a service is recognized as carrier of
EPG data.
The handler will collect information about the program guides for the
various services, and when collected, allow the user to view it.

Whenever the software has collected time table information of
a given service, the technical Data widget will show, when selecting
the service, a button labeled **timeTable** .

![epg data](/qt-dab-epg-data.png?raw=true)

The software is experimental though and - at least here - the times
on the time table are two hours off.

-----------------------------------------------------------------------
Recording the SNR
-----------------------------------------------------------------------

Just as an experiment, a widget was added that shows the development of the 
SNR over time. The main purpose of the widget is to look at the
performance of antennas. 

Computation of the SNR is done with every second DAB frame. As is known,
a sampled DAB frame starts with a null period of app 2600 samples, followed by
76 blocks (each about 2500 samples) with data. SNR is computed as the ratio between the amplitudes in the data blocks and the amplitudes of the
samples in the null period.

For every second DAB frame, such a computation is performed, 
so, about 5 computations per second are performed and shown. To be precise:
each minute there are 312 computations.
The default value for the X-axis of the display in the widget is
therefore 312.

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
# Copyright
------------------------------------------------------------------------

	Copyright (C)  2013, 2014, 2015, 2016, 2017, 2018, 2019, 2020
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	The Qt-DAB software is made available under the GPL-2.0.
	The SDR-J software, of which the Qt-DAB software is a part, 
	is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

