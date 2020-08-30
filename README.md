# Qt-DAB-3.5 [![Build Status](https://travis-ci.org/JvanKatwijk/qt-dab.svg?branch=master)](https://travis-ci.org/JvanKatwijk/qt-dab)

Qt-DAB-3.5 is software for Linux and Raspberry Pi for listening to terrestrial Digital Audio Broadcasting (DAB and DAB+). Qt-DAB is accompanied by its little sister dabMini and the other brother dab-2, all built on the same set of sources.

--------------------------------------------------------------------
DISCLAIMER (for use on Windows)
--------------------------------------------------------------------

While Windows is probably a very good OS, it seems that my way of programming
and Windows are incompatible. I develop under Linux and 
cross-compile for windows. Linux development is simple: if something
is wrong - and that happens from time to time - it is always relatively
easy to detect what is wrong and fix it.

The point is that things that run fine under Linux do not (always) run
fine under Windows: same software, no changes. In case it does not run
correctly under Windows, finding out what happens can be done, but finding
the cause, i.e. why it happens is virtually impossible!

So, yes, I am aware of the fact that sometime things do not go under Windows
as under Linux.

So, while I keep on cross compiling the software for windows, do not
always expect the smooth behaviour as it is under Linux.

---------------------------------------------------------------------
Support for Adalm Pluto!
---------------------------------------------------------------------

I just got the Pluto!! and could work on the support for the
pluto on both the Qt-DAB and dabMini. Support for the pluto is
now standard part of the appImages for qt-dab and dabMini  as well
as for the Windows executables, packed into the Windows installer.

----------------------------------------------------------------------
Differences 3.5.3 <-> 3.4
-----------------------------------------------------------------------

	a. Support for pluto devices,
	b. saving gain settings for different channels when configured,
	c. support for saving detailed output of the scan,
	d. clock error counter and rs corrections counter
	e. improved algorithm for frequency synchronization,
	f. added a "mute" button (moved 2 buttons to technical data)
	g. also for dabMini an appImage (x64) is availablce.

Version 3.5.3. added - apart from a number of internal changes - colors.

------------------------------------------------------------------------
README FIRST  === README FIRST === README FIRST === README FIRST
------------------------------------------------------------------

The directory qt-dab is now the home of *THREE* related, but 
different, versions of a DAB decoder:

 * first of all, there is Qt-DAB, a fairly complete DAB decoder,
based on the idea that *the user is in full control* .

![Qt-DAB with xml input](/qt-dab-1.png?raw=true)

 * second, the *little* sister is *dabMini*, built using the same set
of sources, but with an absolutely minimal GUI.
To assist in creating and installing an
executable on an *RPI* the documentation contains a script with
which all required libraries will be installed, the sources
will be downloaded and compiled into an executable.
dabMini and Qt-DAB share over 95 percent of the sources,
see the subdirectory *dab-mini* for construction details.

![dab-mini](/dab-mini.png?raw=true)

 * third, an *experimental version*, called *dab-2*. While the sources
of *dab-2* and Qt-DAB overlap for more than 90 percent, a completely
different architectural approach is taken for building up
the DAB frames. Note that dab-2 and Qt-DAB share almost all of the GUI, 
dab-2 has a different widget for device control.
See the subdirectory *dab-2* for construction details.

![dab-2](/dab-2-picture.png?raw=true)

------------------------------------------------------------------
Table of Contents
------------------------------------------------------------------

* [Introduction](#introduction)
* [Documentation](#documentation)
* [Features](#features)
* [Widgets and scopes](#widgets-and-scopes-for-qt-dab)
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
* [Copyright](#copyright)

------------------------------------------------------------------
Introduction
------------------------------------------------------------------

**Qt-DAB-3.5** is an implementation of a DAB decoder for use on Linux and Windows based PC's, including some ARM based boards, such as the Raspberry PI, both 2 and 3.

For DX purposes, a separate **dab-scanner** is implemented that allows
for a continuous scanning of selected channels in a given band. Results are 
written in a text file, formatted for use with *LibreOffice Calc* and
comparable programs. Note that in **Qt-DAB-3.5** the scan function
is extended with an option to save detailed data in a text file,
compatible with e.g. LibreOffice.

Derived programs, such as the **dab-scanner** and the Qt-free versions **dab-cmdline** have their own repository on Github.

Since the Qt-DAB program has to run on a headless RPI 2/3 using the home WiFi,
the resulting PCM output can be sent - if so configured - to a TCP port (Sources for a small client are part of the source distribution).

--------------------------------------------------------------------
Documentation
--------------------------------------------------------------------

An extensive "user's guide" - in pdf format - can be found in the "docs"
section of the source tree. The documentation contains a complete
description of the widgets, of the values in the ".ini" file,
on configuring for creating an executable (Linux), and even a
complete description on how to add a device to the configuration.

![Qt-DAB documentation](/qt-usersguide.png?raw=true)

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
  * Views on the signal: spectrum view incl. constellation diagram, correlation result, TII spectrum,
  * Detailed information for selected service (SNR, bitrate, frequency, ensemble name, ensemble ID, subchannel ID, used CUs, protection level, CPU usage, program type, language, 4 quality bars),
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
  * Supports inputs from 
	- Adalm Pluto,
   	- SDR DAB sticks (RTL2838U or similar), 
	- HACKRF One, 
  	- Airspy, including Airspy mini,
  	- SDRplay ( RSP I, RSP II, RSP Duo and RSP Dx), with separate entries for v2 and v3 library
	- limeSDR, 
	- Soapy (experimental, Linux only), 
   	- prerecorded dump (*.raw, *.iq and *.sdr),
	- xml format files, and
	- rtl_tcp servers.
  * Clean interface to add other devices, see below.

Not yet or partly implemented:

  * DMB (Audio and Video)
  * TPEG: when configured, TPEG messages are being sent to a TCP port; sources for a simple client are part of the source distribution.
  * EPG: when configured, the EPG decoding will generate so called EHB files.
  * Journaline (an untested Journaline implementation is part of the sources).
  * Other bands than used for terrestrial broadcasting in Europe (like DAB over cable)

Qt-DAB also supports input from an rtl_tcp server, if such server is used as input device, the connection needs to support the inputrate, i.e. 2,048,000 I/Q samples (i.e. 2 * 2,048,000 bytes/second).

Note:
While the 2.13 support for SDRplay devices is able to handle
the RSP 1, RSP II, RSP Ia and RSP duo, for handling the RSP-Dx one
needs 3.06 (3.07) support

------------------------------------------------------------------
Widgets and scopes for Qt-DAB
------------------------------------------------------------------

The picture on top shows Qt-DAB's main window and the other 5 **optional**
widgets:

  * a widget with controls for the attached device,
  * a widget showing the technical information of the *selected service* as well
as some information on the quality of the decoding, 
  * a widget showing the spectrum of the received radio signal and the constellation of the decoded signal,
  * a widget showing the spectrum of the NULL period between successive DAB frames from which the TII is derived,
  * and a widget showing the response(s) from different transmitters in the SFN,

Another - a sixth - widget shows when running a *scan*; the widget will show the contents of the ensembles found in the selected channel. In 3.5 the
possibility is created to save a detailed description of the services
in the different channels, in a format easily to process with LibreOffice
or comparable programs.

While the main window is always shown, visibility of the others is
under user control, the main widget contains a button for each of those.

![Qt-DAB with SDRplay input](/qt-dab-2.png?raw=true)

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
a separate widget. This widget will show where the data for the
service is to be found in the DAB frames, and how it is encoded.

Depending on a setting in the ".ini" file, a logo or slide, transmitted
with the audio transmission, will be shown here or on a separate widget.

The further selectors are concentrated on the bottom part of the right side
of the widget. Buttons to make scopes visible, to store input and or
output into a file, to select input device and the audio and to
scan and store a description of the ensemble are in that section.

![Qt-DAB scan result](/qt-dab-buttons.png?raw=true)

During *scanning*, a separate window will be shown with the results
of the scan as shown in the picture. Note that what is shown is a *subset*
of what will be recorded if a choice is made to save the output of the scan. 

![Qt-DAB with xml input](/qt-dab-scanner.png?raw=true)

Of course, different people have different tastes when it comes to
colors on a scope.
While the default setting uses black and white, i.e. back for the background
and white for the grid and the signal, others may want more contrasting
colors.
To accomodate that, the background color of the scopes, the grid color,
the color of the curve and the setting of a brush can be adapted

![Qt-DAB scope](/qt-dab-scope-2.png?raw=true)

----------------------------------------------------------------------
Presets for Qt-DAB, dabMini and dab-2
----------------------------------------------------------------------

A *preset* selector is available to allow storing and selecting
"favorit" services.
Note that the services in the preset list are encoded as
"channel:serviceName" pair: it sometimes happens that a
service appears in more than one ensemble
(as example the "Omroep West" service appears in channels 5B and 8A.)

![Qt-DAB with sdrplay input](/qt-dab-presets.png?raw=true)

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

While it has been possible for a long time to set some colors
to the displays, 3.5.3 colors the various buttons (in total
there are 18 push buttons, 16 on the main GUI, two on the
technical data widget).
Since selecting a color is a matter of taste,
and setting all buttons to the same
color seems rather boring, starting in version 3.5.3 there is the possibility
to set a base color for each button as well as the color for the button text.
(Dark base colors require light colored text and vice versa).

The ".ini" file is the place to specify the settings for the button colors.
The :.ini" file contains a special section, *colorSettings*, containing
color settings.
For a button, e.g. "muteButton", "muteButton_color=xxx" is used to set
the base color to "xxx" (provided "xxx" is a valid color name), and
"muteButton_font=xxx" is used to set the color of the text on the button.

The documentation contains a full specification of the *names* of the
buttons and a list of allowable *color names* while the
file  "./dab-maxi/color-scheme" contains a list of possible
settings for the colors. *This file is best included in the ".ini" file*.

For setting in  colors the displays, there are 4 possible settings
(displaycolor, gridcolor, curvecolor and brush), see the documentation.

---------------------------------------------------------------------------
Maintaining History for Qt-DAB and dab-2
---------------------------------------------------------------------------

Qt-DAB-3.5 and dab-2.1 save all service names found.
Pairs "Channel:serviceName"
will be made (in)visible when touching the appropriate button (the
one labeled with "xx").

The data in stored in a file in xml format.
The *history* can be cleared by a click of the right mouse button,
clicking on a channel:servicename combination with the left
mouse button will cause the QT-DAB software to attempt to set the channel and
select the name.

![Qt-DAB with sdrplay input](/qt-dab-history.png?raw=true)

---------------------------------------------------------------------------
Comment on some  configuration settings
-------------------------------------------------------------------------------

All three programs maintain a number of settings in an ".ini" file.
A typical setting is the channel/service combination at program termination,
values, used to initialize the software at program start up.

Next to this kind of settings, there is a myriad of possible settings
that can be done by the user. As an example, the color or the various
scopes can be set.
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
------------------------------------------------------------------

Read carefully what I have said on Windows!

For windows *two*  **installers** can be found in the releases section, https://github.com/JvanKatwijk/qt-dab/releases. The installer will install the executable as well as required libraries.
bit versions for both qt-dab-3.5 and dabMini.

The installer will also call the official installer for the dll implementing the api for getting access to the SDRplay devices.

Note that the Windows versions are cross-compiled from within Linux,
using the mingw64-xxx toolset.

------------------------------------------------------------------
Installation on Linux (PC and RPI)
------------------------------------------------------------------

Unfortunately different Linux distributions have different ways of
naming and ordering libraries. However, for Ubuntu - one of the
more popular distros - the aforementioned user's manual
contains a complete script that can be used to install all required
libraries, download the sources and build an executable.

An alternative is - of course - installing the *appImage* for qt-dab.
The appImage is an executable file, containing all what is needed - apart from
the support libraries for the devices. The appImage for Qt-DAB is
compiled with support for RTL SDR devices, for the 2.13 and 3.06 support
library for the SDRplay, with support for the AIRspy, the limeSDR,
the hackrf, pluto (and of course the various file readers).
The appImage can be found in "https://github.com/JvanKatwijk/qt-dab/releases".

Since Buster, the current system on the RPI's, has the same roots
as Ubuntu, the script for Ubuntu can be used to install Qt-DAB
on an RPI.

For the dabMini, an appImage is available for use on an x64 based
Linux system, and the user's manual contains a complete
script, tested on an RPI 2 and 3.

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
be implemented, to *get* the VFO frequency, 
to inspect the number of samples available and to get a number
of samples, to start and stop operating the device
and to report on the number of bits per sample. This last
function, is used to scale the various spectrum scopes.

A complete description of how to interface a device to Qt-DAB
is given in the user's manual.

------------------------------------------------------------------------
Using other bands than Band III or L Band
------------------------------------------------------------------------

While it is known that the DAB transmissions are now all in Band III,
there are situations where might be desirable to use other frequencies.
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

