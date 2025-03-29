# Qt-DAB-6.9

-------------------------------------------------------------------

![6.9](/res/read_me/front-picture.png?raw=true)

--------------------------------------------------------------------------
About Qt-DAB
-------------------------------------------------------------------------

*Qt-DAB* is software for Linux, Windows, MacOS and Raspberry Pi for listening to terrestrial **Digital Audio Broadcasting (DAB and DAB+)**.

Yhe GUI of Qt-DAB has a single *main* widget that
contains essentially all that is needed when just selecting a service and listening, while other widgets, visible under user control, if made visible show a myriad of controls, and a tremendous amount of data in the DAB signal and the resulting audio, 

Of course, as for previous versions, for the current version,
*Qt-DAB-6.9*, predefined executables and installers are available.
For Windows  **three** installers are available, and for Linux there is an x64 AppImage.

![6.9](/res/read_me/Qt_DAB-6.9.1.png?raw=true)
![6.9](/res/read_me/Qt_DAB-6.9.2.png?raw=true)

Table of Contents
=================================================================

* [Introduction](#introduction)
* [Features](#features)
* [Widgets and scopes](#widgets-and-scopes-for-qt-dab)
* [A Note on using an RTLSDR device](#a-note-on-using-an-rtlsdr-device]
* [Scan control](#scan-control)
* [Displaying TII data](#displaying-TII-data)
* [EPG Handling and time tables]
* [Documentation](#documentation)
* [Installation on Windows](#installation-on-Windows)
* [Installation on Linux](#installation-on-Linux)
* [Notes on building an executable](#building-an-executable-for-qt-dab-a-few-notes)
* [Using user specified bands](#using-user-specified-bands)
* [xml-files and support](#xml-files-and-support)
* [Copyright](#copyright)

Introduction
=================================================================

*Qt-DAB* is designed to be used  with different SDR devices.
A simple and clean interface is used and Qt-DAB-6.9  has support for a
DABsticks, all models of the SDRplay, Airspy, HackRF, LimeSDR, Adalm Pluto etc.
Precompiled versions for Windows and Linux (x64) are available.
Thanks to Richard Huber, *Qt-DAB* can be compiled on the Mac as well.

*Qt-DAB* is GUI driven, 
the full GUI shows 4+ widgets, one of them, the
*main widget* - one with sufficient controls to select channels and service(s) -
is always visible. This main widget contains selector
for controlling the visibility of (the) other widgets.

Features
=================================================================

  * Qt-DAB supports input device:
   	- SDR DAB sticks (RTL2838U or similar), with separate libraries for the V3 and V4 versions of the stick, 
  	- All SDRplay SDR models (RSP I,  RSP 1A and 1B, RSP II, RSP Duo, RSP Dx and RSPDxR2), with separate entries for the v2 and v3 library,
	- HACKRF One, 
  	- Airspy, including Airspy mini,
	- LimeSDR, 
	- Adalm Pluto,
	- **untested** UHD (anyone wants to help testing?)
	- Soapy, a renewed Soapy interface driver is even able to handle other samplerates than the required 2048000 (limited to the range 2000000 .. 4000000),
	- ExtIO (experimental, Windows only),
	- input from servers. support for a spyServer (both 8 bit and a 16 bit version) and an rtl_tcp server'
  * Always supported input from files:
   	- prerecorded dump (`.raw`, `.iq`, '.sdr`, and '.uff' (xml)) files,
  * with, obviously, the possibility of generating such files from the input.
  * Qt-DAB supports so-called *Favorites* (i.e. channel, service pairs) for easy switching between services in different ensembles (see below),
  * Qt-DAB recognizes and interprets *TII* (Transmitter Identification Information) data of - if the received signal is from multiple transmitters - *all* detectable transmitters, can be made visible simultaeously, and displays the transmitters on a map. A separare tool is available to download the required database.
  * Qt-DAB allows running an arbitrary amount of services from tne current ensemble as *background service*. with the output sent to a file,
  * Qt-DAB offers options to select other bands, i.e. the L-Band, or channel descriptions from a user provided file and it supports obsolete modes (Mode II and Mode IV),
  * and much more ...

Widgets and scopes
=======================================================================

![6.9](/res/read_me/Qt_DAB-6.9.2.png)

The *mainWidget* (see picture) of Qt-DAB (always visible), contains settings for controlling the visibility of other widgets.
It shows - left half - a list of services (either from the currently selected channel or from the favourites), seleting a service is just by clicking on the name.
On the right half of the widget is shows the dynamic label, and the slides - if transmitted as part of the service - or a series of default slides.

* touching the *ensemble name* (NPO (8001) in the picture) makes the
*content table*, i.e. an overview of the content of the ensemble, visible with the possibility of storing the data in a ".csv" format. If  the data is visible, touching will hide it;
 * touching the small icon left on the top of the right half
 will show (or hide) the *technical widget*,
a widget showing all technical details as well as strength indicators and 
a spectrum of the audio of the selected service;
 * touching the small icon next, the folder/directory in which the slides, the tii log and the log file are written is shown;
 * touching the icon showing a *speaker* controls muting the signal and shows
whether or not a signal should be audible.
 * touching the *copyright symbol* shows (or, if visible, hides) a small widget with some acknowledgements for using external libraries;
 * touching with the *right hand mouse button* the text of the dynamic label (Alannah Myles - Black Velvet) shows a small menu to put the text on the clipboard;
 * the button labeled *scan* controls the visibility of a the scan handler widget;
 * the button labeled *http* controls the http handler with which a map )with the transmitters) will be shown;
 * the button labeled *spectrum* controls the visibility of the spectrum widget, a widget that contains views on and information of the DAB signal itself;
 * the button labeled *controls* controls the visibility of the so-called *configuration and control widget*, a widget that contains numerous settings for this software;
 * the button labeled *favourites* controls  whether the list of services
from the currently selected channel of the list of favourites is shown,
 * the button labeled *scanlist* controls the visibility
of the scan list, i.e the list of services seen at the most recent scan.

(Obviously, the colors of the buttons, as well as the *font*, the *font size* and the *font color* of the service list shown can be set by the user, just click with the right hand mouse button on a button).

As mentioned, Qt-DAB supports **Favourites**, i.e. a list of (channel, service pairs),
the list is maintained between program invocations. 
The *services list* is shown in one of two modes, selectable by the button (in the picture labeled favourites). In *ensemble view* mode, the services in the current ensemble are shown, in the *favourites view* mode, the favourites are shown.
In both views, selecting a service is just by clicking on the service name.
Of course, when selecting a service in the list of favourites, it might take some time before the software has switched over to the appropriate channel, and has
received sufficient information on the new ensemble carried in that channel before being able to select the service in that channel.

*Adding* a service to the favourites is by clicking on the field in the right hand column, shown in the services list.
If - in *ensemble mode* view - a service is also part of the favourites, the
field in the right hand column is marked. Clicking on such a mark *removes* the
service from the list, as does clicking on the field in the *favourites view*.

![6.8](/res/read_me/technical-widget.png)

The technical widget shows - as the name suggests - some technical details of
the  selected audio service. If the audio of the service is also transmitted
on FM, the FM frequency - derived from additional data in the DAB data -
is shown as well.

The buttons at the top of the widget control dumping the audio
(".wav" file) resp. the AAC frames into a file. The latter can be processed
by e.g. VLC.
The three progress bars (quality indicators) for DAB+ give success rates of
resp. detecting a DAB+ frame, the RS error repair and the AAC decoding.
(For "old" DAB transmissions only a single quality indicator appears).

Below the progress bars the *rsCorrections/100* indicator show how the amount of corrections that was made by the Reed-Solomon detector in the last 100 AAC frames. Of course, the parity bits used by the RS decoding may contain bit errors themselves, the second indicator shows the amount of CRC errors detected after thecorrection by the RS decosing.

![6.8](/res/read_me/spectrum-scope.png)

The widget for the *spectrum scope* is equipped with a tab for selecting
one of 6 views on the input signal.
In the view shown in the picture above, the spectrum of the incoming DAB signal is shown. To the right of this spectrum, one
sees the *signal constellation*,
i.e. the mapping from the complex signals onto their real
and imaginary components. If the selector labeled
"ncp" is set, the centerpoints of the 4 lobs is shown. 

On the right hand side the widget shows some quality indicators of the DAB signal, such as applied frequency correction, remaining frequency error, SNR, some clock offsets and an estimate of the signal quality (for the latter, higher is better).

![6.8](/res/read_me/spectrum-ideal.png)

The ideal form of the spectrum and the signal constellation as shown in the
picture above is not often seen with real inputs.

![6.8](/res/read_me/qt-dab-correlation.png)

The *correlation* scope shows the correlation between the incoming signal and
predefined  data, i.e. the data as they should be.
*Correlation* is used in identifying the precise start of the
(relevant) data of the frame in the input sample stream.
The picture shows more more peaks, i.e. the signal from
more than one transmitter is received. 

![6.8](/res/read_me/qt-dab-null-period.png)

A DAB signal is received as a sequence of samples, and can be thought to
be built up from *frames*, each frame consisting of 199608 consecutive samples.
The first app. 2500 samples of a frame do not carry a signal, the NULL period.
The *NULL scope* shows the samples in the transition from the NULL period to
the first samples *with* data of a DAB frame.

![6.8](/res/read_me/qt-dab-tii-data.png)

In reality the NULL period is not completely without signal, each second NULL period may contain an encoding of the TII data. The *TII scope* shows (part of) the spectrum of the data in the NULL period, the TII data is encoded as a 4 out of 8 code as the picture clearly shows.
The picture clearly shows twice the pattern 0x1e.
This TII data - when decoded leads to 2 2 digit numbers -  is used to
identify the transmitter of the signal received, these numbers can be mapped upon a name and location of the transmitter.

![6.8](/res/read_me/qt-dab-channel.png)

The *channel scope* shows the *channel response* on the transmitted data, i.e.
the deformation of the transmitted signal on the way from transmitter to receiver.  The picture shows the *cyan colored line*, i.e. the channel response on the amplitude, and the *red line*, i.e.  the channel effects on the phase of the samples.

![6.8](/res/read_me/qt-dab-stddev.png)

The *deviation scope* shows the mean deviation of the carriers
in the decoded signal, ideally their phase is (a multiple of) 90
degrees. The phases are mapped upon (soft) bits, the larger the deviations,
the less reliable these bits are.
The Y-axis is in Hz.

![8.8](/res/read_me/configuration-and-control.png)

The *configuration and control* widget contains  checkboxes, spinboxes and
buttons with which the configuration of the decoding process can be
influenced.

At starting up Qt-DAB for the (very) first time, no device is selected yet, amd the widget is made visible to allow selection of an input device (the combobox at the bottom line right).

For a detailed description of all selectors, see the manual (or read the tooltips).

A Note on using an RTLSDR device
======================================================================

RTLSDR devices are - because of their price - rather popular.
In Qt-DAB  different versions for the support library are provided,
It turns out that for support of the V4 version of the
sticks a new version of the library (".dll") is required. Unfortunately,
using that library for "older" models (i.e. V3) models of the stick
seems the device to make rather "deaf".

Two precompiled versions are available, one with support for the V3
and one with support for the V4 version of the stick.

Scan control
=======================================================================

![6.8](/res/read_me/scan-widget.png?raw=true)

A separate widget - visible under control of the *scan* button on the
main widget - provides full control on scanning. Qt-DAB provides different scanning modes, with single scannning and continuous scanning.

To allow skipping over given channels when scanning, Qt-DAB supports the notion of a *skiptable*, in which channels to be skipped can be marked.
Next to a default skiptable, skiptables can be created as separate files and
read-in when required.

The *show* button controls the visibility of the *skiptable*, skiptables
can be loaded and stored in either the ".ini" file (use the "...default" buttons, or can be kept as xml file on a user defined place (the other load/store buttons).

The small table at the bottom of the widget is just for convenience, on scanning it displays the channel name, the ensemble name encountered and the number of services detected in the ensemble.

Displaying TII data
=======================================================================

As mentioned, transmitters (usually) transmit some identifying data, the TII (Transmitter Identification Information) data. Qt-DAB uses a database to map the decoded TII data to name and location of the transmitter.

DAB transmissions are usually  transmitted by an SFN (Single Frequency Network), where all transmitters transmit the same content on the same frequency (each 
with unique TII data). As was shown in the correlation view in the spectrum widget, one may receive a signal from more than one transmitter.
If the *DX* selector on the *configuration and control* widget is set, Qt-DAB tries to identify as much as possible transmitters from the SFN and shows them.

![6.9](/res/read_me/new-dxDisplay.png?raw=true)

The picture shows that in my environment, on channel 12C, the national network,
I can identify a couple of different transmitters in the received signal.
The left column in the widget shows the transmitter whose data is the data
being processed. New is the addition of a "compass" to show the direction
from which the signal comes from the selected transmitter.

If the DX mode is set - the button on the bottom line of the mainwidget -,
Qt-DAB-6.9 will save the transmitters that are identified in a text file.
By default files are placed in a folder (directory)  *Qt-DAB-files*, a folder that is to be found in the user's home folder.

![6.8](/res/read_me/QTmap.png?raw=true)

Qt-DAB has - on the main widget -  a button labeled *http*,
when touched, a small webserver starts that shows
the position(s) of the transmitter(s) received on the map. 
Note that two preconditions have to be met:
 * a "home" location has to be known (see the button *coordinates*);
 * the TII database is installed (see the button *refresh table*)l

The webbrowser listens to port 8080. By default, the "standard" browser
on the system is activated. The *configuration and control* widget
contains a selector for switching this off, so that one might choose
one's own browser.

See the manual for entering the home position to Qt-DAB.

Documentation
=================================================================

An extensive *user's guide* - in PDF format - for Qt-DAB-6.9 can be found in the "docs" directory in the repository and as a document in the releases section.
The manual contains a fairly complete description of the widgets and on configuring for creating an executable (Linux).

![Qt-DAB documentation](/res/read_me/qt-dab-6-manual.png?raw=true)

Installation on Windows
=================================================================

For Windows  *installer*s can be found in the releases section of this repository
 * https://github.com/JvanKatwijk/qt-dab/releases.
Such an installer will install the executable as well as required libraries,
although for both SDRplay devices (when used) or for the Adaml Pluto (whens
used), one has to install libraries from the provoder of the device.

:information_source: The releases section contains 3 Windows installers. One for a 64 bit version, and 2 for a 32 bit version. The 2 versions for 32 bit differ in theit support for RTLSDR type devices (as mentioned above).

:information_source: Note that the device libraries for the SDRplay devices and the Adalm Pluto device library are **NOT** included in the installer, they require - if used - a separate installation. See below for details.

:information_source: The 64 bit installers now contain the required dll's for 
both the hackrf amd the lime devices. As the title on the main window already suggested, the configuration for the 64 bit versions (noth Qt5 and Qt6) contains the settings for the rtlsdr V4 library.

Installation on Linux-x64
=================================================================

For Linux-x64 systems, an *appImage* can be found in the releases section of
this repository
 * https://github.com/JvanKatwijk/qt-dab/releases.

:information_source:
The appImage contains next to the executable Qt-DAB program, the required libraries **but not the support libraries for the configured devices**. If you want to use a physical device - e.g. a DABstick, an SDRplay, or an AIRspy you need to install the driver libraries for the device as well.

For using an SDRplay device one should download the - proprietary - driver software from the SDRplay site. Note that the "old" 2.13 library does not support
the newer SDRPlay device model such as the SDRPlay 1B. the SDRplayDx
and the SDRPlay Dx-II. Use the 3.XX library instead.
The libraries can be found on the website of SDRplay
 * www.sdrplay.com

For using an AIRspy or a Hackrf device, the Ubuntu repository as well as the bullseye repository (for the RPI) provide a working library.

For using an RTLSDR device (a DABstick), the Ubuntu (and bullseye) repositories
provide a package. However, using that package one needs the
kernel module to be blacklisted, see e.g.
 * https://www.reddit.com/r/RTLSDR/wiki/blacklist_dvb_usb_rtl28xxu/

Personally, I prefer to build a version of the library myself,  installation is easy, see:
 * "https://osmocom.org/projects/rtl-sdr/wiki".

For using the LimeSDR device I went back to the sources and compiled the
support library myself, see:
 * "https://wiki.myriadrf.org/Lime_Suite".

For installing the support software for the Adalm Pluto I followed the instructions on
 * "https://wiki.analog.com/university/tools/pluto/users"

Building an executable for Qt-DAB: a few notes
=================================================================

Note:
I work on Linux, creating both AppImages for Linux and Cross-compiled versions
for Windows. While it is most likely possible to build an executable
on and for Windows, do not ask me, I do not know anything about windows.
=====================================================================

It is strongly advised to use qmake/make for the compilation process,
the *qt-dab-RC.pro* file contains (much) more configuration options
than the *CMakeLists.txt* file that is used with cmake.

Note that the scheme presented below is applied when building the AppImage
on Ubuntu 20, and was tested on the "bullseye" system on the RPI. 
For other distributions (or later Ubuntu versions), names of library
packages may be different. Note that in all cases, the development versions (i.e. the versions with the include (".h") files) are required.

For Windows the easiest approach is to install msys/mingw and follow the process as sketched (I use Mingw64 packages on Fedora to cross-compile).
Using an MS toolchain on Windows was for me not (yet?) successfull.

Step 1
-----------------------------------------------------------------

- :information_source:  In the repository, the sources for 6.9 are in the subdirectory *qt-dab-6.9*. 
The subdirectories contain a *.pro file with configuration information for use with *qmake*, and a *CMakeLists.txt* file with configuration information for use with *cmake*.

For building the AppImage on Ubuntu 20, I load the required libraries as given below:

 *   sudo apt-get update
 *   sudo apt-get install git
 *   sudo apt-get install cmake
 *   sudo apt-get install qmake6
 *   sudo apt-get install build-essential
 *   sudo apt-get install g++
 *   sudo apt-get install pkg-config
 *   sudo apt-get install libfftw3-dev
 *   sudo apt-get install portaudio19-dev 
 *   sudo apt-get install zlib1g-dev 
 *   sudo apt-get install libusb-1.0-0-dev
 *   sudo apt-get install mesa-common-dev
 *   sudo apt-get install qt6-base-dev
 *   sudo apt-get install qt6-multimedia-dev
 *   sudo apt-get install libcurl4-openssl-dev
 *   sudo apt-get install libfdk-aac-dev (read the note below)

- :information_source:  An issue is getting the required qet library. On my Fedora box, a qwt-6.2 version for Qt6 is available, Ubuntu does not provide
the Qt6 version for qwt. I had to install it myself

 *   Download qwt-6.30 from "https://sourceforge.net/projects/qwt/files/qwt/6.3.0/";
 *   follow the instructions (i.e. unzip, *cd* to the unzipped folder) and adapt the config file to your likings;
 *   building is then simple (takes some time though): "qmake6 qwt.pro", "make";
 *   install the library ("sudo make install") and inform the loader "sudo ldconfig";
 *   Note that the default for installation is "/usr/local/qwt/6.3.0", adjust the PATH settings accordingly.

- :information_source: While the *libfdk-aac-dev* package in both Fedora and Ubuntu 24 seems to work fine, I had some problems with the package from the repository in Ubuntu 20 and 22. For the AppImage, built on Ubuntu 20, a library version was created from the sources to be found as github repository:
 * "https://github.com/mstorsjo/fdk-aac"

The sources contain a *CMakeLists.txt* file, building and installing is straightforward.

Alternatively, one could configure for *libfaad*, change the configuration to

 *  CONFIG	+= faad
 *  #CONFIG	+= fdk-aac

and install the libfaad package
 *   sudo apt-get install libfaad-dev

Step 2
-----------------------------------------------------------------

While there are dozens of configuration options, take note
of the following ones:

- :information_source: A new viterbi decoder is part of the sources,

	* CONFIG	+= viterbi-new

If running on a "more or less" modern X86_64 based device, uncomment in the
section *viterbi-new*

        DEFINES         += __ARCH_X86__
 	QMAKE_CFLAGS    +=  -mavx2 -msse4
        QMAKE_CXXFLAGS  +=  -mavx2 -msse4
        QMAKE_LFLAGS    +=  -mavx2 -msse4

thhis informs the software that it should dynamically check for 
 *SSE* and *AVX* support to speed up the cpu intensive computations for the
Viterbi decoding.

If running am AARCH64 based systen, uncomment

	#DEFINES        += __ARCH_AARCH64__

Default is using a *scalar* version of the viterbi decoder, which works just fine, though requires more CPU time.

- :information_source: Choose between *CONFIG += single* or *CONFIG += double*. In the latter case, all computations in the "front end" are done with double precision arithmetic.

- :information_source: Devices like SDRplay, AIRspy, RTLSDR dongle, LimeSDR,
HackRf and Adalm Pluto can be included in the configuration *even if no support library is installed*. (Note that including *Soapy* requires Soapy libraries to be installed, so this does not apply for Soapy). Qt-DAB is designed such that on selecting a device in runtime, the required functions from the device library are linked in.

- :information_source: The Soapy library used in Ubuntu 20 (used for building the AppImage) seems incompatible with Soapy libraries installed on other versions of Ubuntu and other Linux distributions, therefore *Soapy* is **NOT** configured
for inclusion in the AppImage.

- :information_source: Uncomment the line *DEFINES += __THREADED_BACKEND* if you intend to have more than one backend running simultaneously. E.g. activating the automatic search for an EPG service starts a separate service if such a service is found. With this setting each backend will run in its own thread.
 
Step 3
-----------------------------------------------------------------

Run *qmake* (variants of the name are *qmake6*, *qt6-qmake*) which generates a *Makefile* and then run *make*.  *Compiling may take some time*.
Use *make -j XX* for speeding up the build process, with XX the amount
of parallel threads used. Of course, qmake will complain if not all
required libraries can be found.

Step 4
-----------------------------------------------------------------
Use the database downloader (for Windows and Linux available as
precompiled item) to download a fresh copy of the database. Or download a
copy of the database from the repository.

If Qt-DAB does not *see* the database, it will just function without mapping TII data onto names and locations.

Some comments
================================================================

A user compiled Qt-DAB-6.9  ond and for an RPI, met the follwoing issues:

Once I had downloaded the qt-dab-master from the code page,
installed all the libraries listed in the Readme on the code page,
and then installed the new qwt 6.30 as instructed,
I had a problem getting the .pro file to recognise the new qwt version.
This was because on the pi it installs to /usr/include.

So I changed the .pro file as follows:
Line 433
INCLUDEPATH += /usr/include
!mac {
INCLUDEPATH += /usr/include/qwt-6.3.0/lib
#correct this for the correct path to the qwt6 library on your system
#LIBS += -lqwt
equals (QT_MAJOR_VERSION, 6) {
LIBS += -lqwt-qt6
}else{ LIBS += -lqwt-qt5

However I still could not get the qwt recognised so further changes were needed, this time to the Modules folder.

To do this, I went to the qt-dab-6.9 folder in the qt-dab-master, and then opened the folder cmake. Then Modules.
Found the relevant FindQwt file and made the following changes:

find_path(QWT_INCLUDE_DIRS
NAMES qwt_global.h
HINTS
${CMAKE_INSTALL_PREFIX}/include/qwt
${CMAKE_INSTALL_PREFIX}/include/qwt-qt6
PATHS
/usr/local/include/qwt-qt6
/usr/local/include/qwt
/usr/include/qwt6
/usr/include/qwt6-qt6
/usr/include/qt6/qwt
/opt/local/include/qwt
/usr/include/qwt-6.3.0
/sw/include/qwt
/usr/local/lib/qwt.framework/Headers
/usr/local/lib/qwt-qt5/lib/framework/Headers
/usr/include/qwt-6.3.0/include
)
if (APPLE)
set(CMAKE_FIND_LIBRARY_SUFFIXES " " " .dylib" ".so" ".a ")
endif (APPLE)

find_library (QWT_LIBRARIES
NAMES qwt6 qwt6-qt6 qwt-qt6 qwt
HINTS
${CMAKE_INSTALL_PREFIX}/lib
${CMAKE_INSTALL_PREFIX}/lib64
PATHS
/usr/local/lib
/usr/lib
/opt/local/lib
/sw/lib
/usr/local/lib/qwt.framework
/usr/local/lib/qwt-qt6/lib/framework
/usr/include/qwt-6.3.0/lib
)
With these settings Qt-DAB could be compiled

Using user specified bands
=================================================================

While it is known that the DAB transmissions are now all in Band III, there might be situations where it is desirable to use other frequencies.
Qt-DAB provides (Unix/Linux builds only) the opportunity to specify
one's own band. Specify in a file a list of channels, e.g.

	one	227360
	two	220352
	three	1294000
	four	252650

and pass the file on program start-up with the `-A` command line switch. The channel name is just any identifier, the channel frequency is given in kHz. Your SDR device obviously has to support the frequencies for these channels.

xml-files and support
=================================================================

*Clemens Schmidt*, author of the QiRX program (https://qirx.softsyst.com/) and me defined a format for storing and exchanging "raw" data: `.xml`-files for
easier echange of recordings. Such an xml file contains in the first bytes - up to 5000 - a description in xml - as source - of the data contents. This xml description describes in detail the coding of the elements. 

Copyright
=================================================================

	Copyright (C)  2016 .. 2024
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	Copyright of libraries used - Qt-DAB, qwt, fftw, portaudio,
	libsndfile. libsamplerate, ;ibusb-1, libfaad, libfdk-aac,
	- is gratefully acknowledged.
	
	Qt-DAB is distributed under e GPL V2 library,  in the hope that
	it will be useful, but WITHOUT ANY WARRANTY; without even the
	implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE.  See the GNU General Public License for
	more details.
