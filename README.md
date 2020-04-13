# Qt-DAB-3.3 [![Build Status](https://travis-ci.org/JvanKatwijk/qt-dab.svg?branch=master)](https://travis-ci.org/JvanKatwijk/qt-dab)

Qt-DAB-3.3 is software for Windows, Linux and Raspberry Pi for listening to terrestrial Digital Audio Broadcasting (DAB and DAB+). It is the successor of both DAB-rpi and sdr-j-DAB, two former programs by the same author.


![Qt-DAB with xml input](/qt-dab-3.3.png?raw=true)

------------------------------------------------------------------
New: some documentation
------------------------------------------------------------------

Since the Qt-DAB program is pretty large and has quite a number
of options for users to influence the behaviour, I added a *preliminary*
piece of documentation in the sections docs

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
* [New in Qt-DAB 3.3](#New-in-Qt-DAB-3.3)
* [Using other bands than Band III or L Band](#other-bands)
* [xml-files and support](#xml-files-and-support)
* [Copyright](#copyright)


------------------------------------------------------------------
Introduction
------------------------------------------------------------------

**Qt-DAB-3.3** is an implementation of a DAB decoder for use on Linux and Windows based PC's, including some ARM based boards, such as the Raspberry PI, both 2 and 3.

For DX purposes, a **dab-scanner** is implemented that allows
for a continuous scanning of selected channels in a given band. Results are 
written in a txt file, formatted for use with *LibreOffice Calc* and
comparable programs.

Derived programs, such as **sdrplayDab**, the **dab-scanner** and the Qt-free versions **dab-cmdline** and the **dab-server** have their own repository on Github.

Qt-DAB-3.3 also supports file input, from pre-recorded files (`*.sdr`, `*.iq` and `*.raw`) and on the aforementionedxml files. Obviously there is a provision for dumping the input into an (\*.sdr)-file and - for most configured devices - as xml file. 

Finally, Qt-DAB supports input from an rtl_tcp server, if such server is used as input device, the connection needs to support the inputrate, i.e. 2,048,000 I/Q samples (i.e. 2 * 2,048,000 bytes/second).

Since the Qt-DAB program has to run on a headless RPI 2/3 using the home WiFi, the resulting PCM output can be sent - if so configured - to a TCP port (Sources for a small client are part of the source distribution).

See also the section of configuring Qt-DAB

-----------------------------------------------------------------
dab-mini
-----------------------------------------------------------------

When having the sound of DAB as background music,  a DAB program often runs
on a (headless) RPI2 or 3. Since the RPI is headless, control is
using my laptop. Quite often I do not need all the whistless and bells 
that are provided on the Qt-DAB GUI, I basically only need a list of
services, and a very few buttons to select channel and service.

When using *dabradio* for this, I really miss goodies like presets, and
previous and next selectors for channels and services.
A choice was made not to upgrade the *dabradio* but to build an
additional restricted GUI on top of the Qt-DAB sources: the *dab-mini*.

The obvious advantage is that changes to the Qt-DAB sources would
be changes to the *dab-mini* sources.
As the picture shows, the *dab-mini* GUI is minimal, a few selectors
for setting the gain of the configured and detected device, a channel
selector, a few buttons to select previous and next channel, to
select previous and next service, a preset selector and a selector for
the audio device.

![dab-mini](/dab-mini.png?raw=true)

See the subdirectory *dab-mini* (or the separate repository)

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

Another - a sixth - widget shows when running a *scan*; the widget will show the contents of the ensembles found in the selected channel.

While the main window is always shown, visibility of the others is
under user control, the main widget contains a button for each of those.

![Qt-DAB with SDRplay input](/qt-dab-3.3-kaal.png?raw=true)

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

Different from previous versions is that now some information, previously
shown on the "technical data" widget is now shown on the main widget.

Some data on the selected service - if any - is to be found on
a separate widget. This widget will show where the data for the
service is to be found in the DAB frames, and how it is encoded.

Furthermore, if the service is accompanied by a logo, that logo will
be shown here.

The further selectors are concentrated on the bottom part of the right side
of the widget. Buttons to make scopes visible, to store input and or
output into a file, to select input device and the audio and to
scan and store a description of the ensemble are in that section.

![Qt-DAB scan result](/qt-dab-buttons.png?raw=true)

During **scanning**, a separate window will be shown with the results
of the scan as shown in the picture.

![Qt-DAB with xml input](/qt-dab-scanner.png?raw=true)

------------------------------------------------------------------
Features
------------------------------------------------------------------

  * DAB (mp2) and DAB+ (HE-AAC v1, HE-AAC v2 and LC-AAC) decoding
  * MOT SlideShow (SLS)
  * Dynamic Label (DLS) 
  * Both DAB bands supported): 
  	* VHF Band III (default),
   	* L-Band (only used in Czech Republic and Vatican) (see "obsolete properties")
  * Modes I, II and IV (Mode I default, oter modes can be set in the ".ini" file)
  * Spectrum view (incl. constellation diagram, correlation result, TII spectrum)
  * Scanning function (scan the band and show the results on the screen)
  * Detailed information for selected service (SNR, bitrate, frequency, ensemble name, ensemble ID, subchannel ID, used CUs, protection level, CPU usage, program type, language, 4 quality bars)
  * Detailed information for other services by right-clicking on their name (bitrate, subchannel ID, used CU's protection level, program type)
  * Automatic display of TII (Transmitter Identification Information) data when transmitted
  * Presets for easy switching of programs in different ensembles (see *Presets*)
  * Dumping of the complete DAB channel (Warning: produces large raw files!) into \* sdr files and playing them again later
  * Dumping the input in the aforementioned xml format
  * Saving audio as uncompressed wave files
  * Saving aac frames from DAB+ services for processing by e.g. VLC
  * Saving the ensemble content (description of audio and data streams, including almost all technical data) into a text file readable by e.g *LibreOfficeCalc*
  * ip output: when configured the ip data - if selected - is sent to a specificied ip address (default: 127.0.0.1:8888)
  * TPEG output: when configured the data is sent to a specified ip address
  * Supports inputs from 
  	- SDRplay (both RSP I and RSP II), with separate entries for v2 and v3 library
  	- Airspy, including Airspy mini,
   	- SDR DAB sticks (RTL2838U or similar), 
	- HACKRF One, 
	- limeSDR, 
	- Soapy (experimental, Linux only), 
   	- prerecorded dump (*.raw, *.iq and *.sdr),
	- xml format files, and
	- rtl_tcp servers
  * Clean interface to add other devices, see below.

Not yet or partly implemented:

  * DMB (Audio and Video)
  * TPEG: when configured, TPEG messages are being sent to a TCP port; sources for a simple client are part of the source distribution.
  * EPG: when configured, the EPG decoding will generate so called EHB files.
  * Journaline (an untested Journaline implementation is part of the sources).
  * Other bands than used for terrestrial broadcasting in Europe (like DAB over cable)

----------------------------------------------------------------------
Presets
----------------------------------------------------------------------

A *preset* selector is available to store and retrieve "favorit" services.
Note that the services are encoded as "channel:serviceName" pair:
it sometimes happens that a service appears in more than one ensemble
(as example the "Omroep West" service appears in channels 5B and 8A.)

![Qt-DAB with sdrplay input](/qt-dab-3.3-presets.png?raw=true)

The presets are stored in an xml file, `.qt-dab-presets.xml'.

*Adding* a service to the *presets* is simply by *clicking with the right mouse
button on the name of the service that is currently selected in the
servicelist* (recall that clicking with the *left* mouse button
well select the service with that name).

Of course, one is also able to *remove* an entry from the presets.
To do this, select the entry (by putting the curson on it without
clicking any of the mouse buttons) and press the *shift* and the *delete*
button on the keyboard simultaneously.

---------------------------------------------------------------------------
Maintaining History 
---------------------------------------------------------------------------

Qt-DAB-3.3 saves all service names found. Pairs Channel:serviceName
will be made (in)visible when touching the appropriate button (the
one labeled with "xx").

The data in stored in a file in xml format.
The *history* can be cleared by a click of the right mouse button,
clicking on a channel:servicename combination with the left
mouse button will cause the QT-DAB software to attempt to set the channel and
select the name.

![Qt-DAB with sdrplay input](/qt-dab-3.3-history.png?raw=true)

---------------------------------------------------------------------------
Comment on some settings
-------------------------------------------------------------------------------

Some settings are preserved between program invocations, they are stored in a file `.qt-dab.ini`, to be found in the home directory.
Settings maintained between program invocations are typically the name 
of the device used, the channel that was used, the name of the service
last used etc.

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
a port can be set in the ".ini" file by setting

`rtl_tcp_port=xxx`

In selecting a service from the presetlist that resides on another
channel implies switching to that channel and waiting until the
ensemble in that channel is recognized.
The duration of the delay can be specified  by setting

`switchTime=xxx`

where xxx is the amount of milliseconds

The result of the correlation is shown ny the correlation viewer. The
correlation takes place on the first datablock of a DAB frame, and if
the estimation of the end of the null period is correct, the maximum
in the correlation is 504. Since for DAB a single frequency network is
used, in general more than one transmitter is received and there are
more "peaks" in the correlation.
The distance between the peaks is in units of 1/2048000 seconds.
While the correlationviewer by default shows the correltion result of
the first 1000 samples, this may be reduced by setting

`plotLength=xxx`

where xxx is the number of samples taken into account.

-------------------------------------------------------------------------
Obsolete properties
-------------------------------------------------------------------------

The current DAB standard eliminated the support for Modes other than Mode 1 and Bands other than VHF Band III. The Qt-DAB implementation still supports these features, however, since they are obsolete, there are no controls on the GUI anymore (the control for the Mode was already removed from earlier versions). 

Explicitly setting the Mode and/or the Band is possible by
including some command lines in the ".qt-dab.ini" file.

For the Mode, one will add/set a line

	dabMode=Mode x, where x is either 1, 2 or 4

For the Band, one will add/set a line

	dabBand=band, where band is either VHF Band III or L_Band

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
CONFIG		+= dabstick
CONFIG		+= sdrplay-v2
CONFIG		+= sdrplay-v3	
CONFIG		+= lime
CONFIG		+= rtl_tcp
CONFIG		+= airspy
CONFIG		+= hackrf
#CONFIG		+= soapy
```

DRM+, for which I am writing a decoder in another project, supports,
next to the "classic" AAC encoding of audio, the xHE-AAC encoding.
Decoding of xHE-AAC encoded audio frames is not supported by the "faad"
library, and it is not clear whether or not support will ever turn up.

The *fdk-aac* (from Fraunhofer institute) turned out to be an excellent
alternative, it supports decoding both the AAC and xHE-AAC segments
from DRM30 and DRM+. Support for fdk-aac was therefore included in the
aforementioned DRM+ decoder.

As an experiment, support for fdk-aac to Qt-DAB is added using the
same code that creates the aac output files whenever selected
(Thanks to Stefan Poeschel for that).

The ".pro" file now contains - in the section for Unix - two lines with which
either fdk-aac or faad can be selected.

	CONFIG	+= fdk-aac
	#CONFIG	+= faad

As far as I can see, the fdk-aac library can be installed from the repositories
of the Linux distribution,  of course, you should have the library and
the include files ("development package") installed when using it.


The Qt-DAB software does not interpret the TPEG streams, however,
the data of these streams can be output. If so configured, the
software acts as a simple server.

```
#CONFIG		+= datastreamer
```

To handle output of embedded an IP data stream, uncomment

```
CONFIG		+= send_datagram
```

#if you want to listen remote, uncomment

```
CONFIG		+= tcp-streamer		# use for remote listening
```

otherwise, if you want to use the default qt way of sound out

```
#CONFIG		+= qt-audio
```

Comment both out if you just want to use the "normal" way.
The source tree contains a directory `sound-client`, that contains sources to generate a simple "listener" for remote listening.

By default, the name of the service last used is kept between
program invocations and it is the one the Qt-DAB software will
try to open on the next invocation.

```
DEFINES		+= PRESET_NAME
```

The performance of an RPI 2 is such that running the program in a single
thread overloads the processor. Uncommenting

```
DEFINES		+= __THREADED_BACKEND
```
configures the program to have the *backend* run in a separate thread.


In the Windows configuration one may also choose
````
CONFIG		+= extio
````

for use with (appropriate) extio handlers

Remark: Input from pre-recorded files (8 bit unsigned `*.raw` and `*.iq` as well as 16-bit wav `*.sdr` files, and the new xml-files) is configured by default.

------------------------------------------------------------------
Configuring using CMake
------------------------------------------------------------------

The `CMakeLists.txt` file has all devices and the spectrum switched off per default. You can select a device (or more devices) without altering the `CMakeLists.txt` file, but by passing on definitions to the command line.

An example:
```
cmake .. -DRTLSDR=ON -DSDRPLAY=ON 
```
	
will generate a makefile with support for 

 a) the RTLSDR (dabstick) device, 
 
 b) the SDRPlay device (with 2.13 library).
 
Other devices that can be selected are sdrplay for library version 3.06, HackRF and airspy. Use `-DHACKRF=ON`, `-DSDRPLAY_V3=ON`, or `-DAIRSPY=ON` after the `cmake` command if you want to configure them.

The default location for installation depends on your system, mostly `/usr/local/bin` or something like that. Set your own location by adding
```
-DCMAKE_INSTALL_PREFIX=your installation prefix
```

For other options, see the `CMakeLists.txt` file.

Important: Note that `CMakeLists.txt` file expects the appropriate Qt version (and - if configured - the qwt library) to be installed.

-----------------------------------------------------------------
SDRplay
-----------------------------------------------------------------

The current set of sources provides support for both the RSP-I and the RSP-II.
Note that - when so configured - there are two entries for the SDRplay in
the device selector.

 - the v2 version uses the 2.13 library,
 - the v3 version uses the 3.06 library.

Note that running against the v3 library requires the "daemon" (or
service) to run. 

------------------------------------------------------------------
Qt
------------------------------------------------------------------

The software uses the Qt library and - for the spectrum and the constellation diagram - the qwt library.

The `CMakeLists.txt` assumes Qt5. If you want to use Qt4, and you want to have the spectrum in the configuration, be aware of the binding of the qwt library (i.e. Qt4 and a qwt that uses Qt5 does not work well).  

Note that Qt4 is considered to be obsolete and its use will - slowly -
be phased out.

-----------------------------------------------------------------
Raspberry PI
------------------------------------------------------------------

The Qt-DAB software runs pretty well on the author's RPI-2 and 3.
When configured for running the backend in a separate thread, the average load on the 4 cores is slightly higher than avg 50 percent on the RPI2 and
less than 50 percent on the RPI3.

One remark: getting "sound" is not always easy. Be certain that you have installed the alsa-utils, and that you are - as non-root user - able to see devices with `aplay -L`

In arch, it was essential to add the username to the group "audio".

The most recent distributions, Raspbian Stretch and Raspbian Buster
supports both Qt5 and a qwt compiled against Qt5.

Since Raspbian Stretch is a Debian derivate, the description for creating a version under Ubuntu applies, a script to download all required packages, compile
and build a full executable for qt-dab is part of the source tree.

Before running this script, script *script-for-devian.txt*, copy the script
to the parent directory (executing it will load a whole bunch of files).

Since it is possible to divert the sound output to bluetooth, it
is certainly possible to run the program and listen using bluetooth headphones.

---------------------------------------------------------------------------
appImage for x64 Linux systems
---------------------------------------------------------------------------

https://github.com/JvanKatwijk/qt-dab/releases contains a generated appImage, **Qt-DAB-x64.Appimage**, which is created on Ubuntu 16.04 (Xenial).

*Different from previous versions, this version assumes you have installed
the support library for the devices you want to use.*

If you want to run with an SDRplay, follow the installation instructions for the library from http://www.sdrplay.com . All further dependencies are included.

If you want to run an RTLSDR based dabstick, please note that
the appImage may complain with some Linux distros with a
"librtlsdr.so" pre-installed that the kernel module has to be blacklisted. Depending on the distribution, blacklisting is in
/etc/modprobe.d/local-blaclist

For compiling and installing a support library for an RTLSDR device, 
follow the instruction on https://osmocom.org/projects/rtl-sdr/wiki/Rtl-sdr.
One of the instructions is how to configure such that blacklisting the
kernel module is not needed.

For compiling and installing a support library for an AIRspy device,
find the sources on https://github.com/airspy/airspyone_host/tree/master/libairspy. The sourcetree contains a description of how to generate the
library, using the Gnu tools or cmake.

The appImage itself is just a self-contained single file which you have to make executable in order to run.

For more information see http://appimage.org/

-----------------------------------------------------------------------
Interfacing to another device
-----------------------------------------------------------------------

There are - obviously - more devices than supported
here. Interfacing another device is not very complicated,
it might be done using the "Soapy" interface, or one might
write a new interface class.

Device handlers are implemented as a class, derived from
the class *virtualInput*. Only a few functions have to
be implemented, to *set* and *get* the VFO frequency, 
to inspect the number of samples available and to get a number
of samples, to start and stop operating the device
and to report on the number of bits per sample. This last
function, is used to scale the various spectrum scopes.

A complete description is given in the file "interfacing.txt",
in the sourcetree

--------------------------------------------------------------------
New in Qt-DAB 3.3
--------------------------------------------------------------------

Two of the (many) areas that still needed addressing in the handling of
DAB+ contents were handling

  * secondary audio services* and reacting upon a
  * change in  configuration*.

While not encountered here (the Netherlands), *Secondary audio services*
are seen in BBC transmissions. They are now visible as service
and can be selected.

A *change in configuration* may be that bitrates of channels change,
that protection changes, and even that *secondary audio services*
appear or disappear.

Especially interesting is of course  what the software should do
when - after a *change in configuration* - the selected secondary
audio service is gone.

In Qt-DAB 3.3 a mechanism is included to make secondary audio services
visible and selectabe, and to handle a change in configuration.
Such a change will manifest itself as a minor disruption (app 20 msec)
in the signal of the selected service.

------------------------------------------------------------------------
Using other bands than Band III or L Band
------------------------------------------------------------------------

While it is known that the DAB transmissions are in Band III, there are
situations where it is desirable to use other frequencies.
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

The device handlers in Qt-DAB-3.3 support the generation of 
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

