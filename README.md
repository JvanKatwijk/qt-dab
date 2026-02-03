# Qt-DAB-6.10

-------------------------------------------------------------------

![6.10](/res/read_me/qt-dab-logo.png?raw=true)

--------------------------------------------------------------------------
About Qt-DAB
-------------------------------------------------------------------------

*Qt-DAB* is software for Linux, Windows, MacOS and Raspberry Pi for listening to terrestrial **Digital Audio Broadcasting (DAB and DAB+)**.

Qt-DAB is GUI based, for a command line version, see **dab-cmdline**.
Qt-DAB  has a single *main* window that contains essentially all that is needed for selecting channels and services and listening to a service. Other windows, visible under user control, show a myriad of controls, and a tremendous amount of data extracted from the DAB signal and the resulting audio, 

![6.10](/res/read_me/qt-dab-front-picture.png?raw=true)

Of course, as for previous versions, for the current version,
*Qt-DAB-6.10*, predefined executables - for Linux an AppImage, for Windows
an installer - are available.

Table of Contents
=================================================================

* [Introduction](#introduction)
* [Features](#features)
* [Windows and scopes](#windows-and-scopes)
* [Devices and device support](#devices-and-device-support)
* [Scan control](#scan-control)
* [Displaying TII data](#displaying-TII-data)
* [EPG Handling and time tables](#epg-handling-and-time-tables)
* [Journaline data](#journaline-data)
* [Logging errors](#logging-errors)
* [Documentation](#documentation)
* [Installation on Windows](#installation-on-Windows)
* [Installation on Linux](#installation-on-Linux)
* [Notes on building an executable](#building-an-executable-for-qt-dab-a-few-notes)
* [Using user specified bands](#using-user-specified-bands)
* [Copyright and acknowledgements](#copyright-and-acknowledgements)

Introduction
=================================================================

*Qt-DAB* is designed to be used with a variety of SDR devices.
The precompiled versions for Windows and Linux (x64) support
most devices for which Qt-DAB provides support (see below).
Thanks to Richard Huber, *Qt-DAB* can be compiled on the Mac as well, though it is most likely not a trivial exercise.

Features
=================================================================
  
  * Qt-DAB supports most common SDR devices directly (see below). The device interface is quite simple and in a different document it is explained in detail how to use the interface  to implement control for other devices;
  * Qt-DAB supports so-called **favorites** (i.e. channel, service pairs) for easy switching between services in different ensembles (see below),
  * Qt-DAB recognizes and interprets **TII** (Transmitter Identification Information) data of - if the received signal is from multiple transmitters - *all* detectable transmitters, can be made visible simultaneously, and Qt-DAB displays the transmitters on a map in a browser. A separate tool is available to download a database, the precompiled versions contain a copy of the database;
  * Qt-DAB offers different views on the input data, see below,
  * Qt-DAB offers the possibility of **scanning** through all - or a selected subset - channels of the band,
  * Qt-DAB starts **EPG/SPI** services automatically as **background task** and provides means to show resulting time tables;
  * Qt-DAB allows running an arbitrary amount of audio services from the current ensemble as **background service**, with the output sent to a file,
  * Qt-DAB supports a **scanList**, i.e. a list with channel/service pairs encountered. Obviously, selecting a service from this list is possible;
  * Qt-DAB supports **journaline**, often transmitted as subservice, if a journaline (sub)service is detected, auto starts a small journaline window;
  * Qt-DAB offers the possibility of generating an **ETI file** from the ensemble in the currently selected channel,
  * Qt-DAB offers options to select other bands, i.e. the L-Band, or channel descriptions from a user provided file and it supports obsolete modes (Mode II and Mode IV),
  * and much more ...

Widgets and scopes
=======================================================================

![6.10](/res/read_me/Qt_DAB-6.10.png)

The **main window** (see picture) of Qt-DAB (always visible), contains selectors for controlling the visibility of other windows.
The window shows - left half - a list of services (either from the currently selected channel or from **favourites**). Selecting a service is just by clicking on the name.
The right half of the window shows the dynamic label, and the slides - if transmitted as part of the service - or a series of default slides.


![6.10](/res/read_me/topline.png)

The picture above shows the topline op the right half of the main window.

 * touching the small icon left on the top of the right half will show (or hide) the **technical window**, a window showing technical details as well as strength indicators and a spectrum of the audio of the selected service;
 * touching the small icon to the right next, the **folder/directory** in which the slides, the tii log and the log file are written is shown;
 * touching the small **blue** icon causes the device list to be displayed. Touching the icon again hides the icon. After selecting a device, the device list will be hidden as well.

![6.10](/res/read_me/devicelist.png)

 * touching the icon showing a **speaker** controls **muting** the signal and shows whether or not a signal should be audible.
 * touching the **copyright symbol** shows (or, if visible, hides) a small **about** window with acknowledgements for using external libraries;

  * touching the **ensemble name** (NPO (8001) in the picture seen top left) makes the **content table**, i.e. an overview of the content of the ensemble, visible with the possibility of storing the data in a file in ".csv" format. If the content table is visible. Touching the ensemble name again will hide it;

![6.10](/res/read_me/content-table.png)

![6.10](/res/read_me/mainwindow-buttons.png)

At the bottom of the right half of the window, one sees the buttons shown above,
from right to left:

 * the button labeled **scan** controls the visibility of a the scan handler window;
 * the button labeled **http* controls the **http handler** with which a browser is started that shows a map (with the transmitters displayed on their location).  **NEW** is that closing the http handler closes the browser as well (depending on a setting in the configuration window);
 * the button labeled **spectrum** controls the visibility of the spectrum window, a window with options to select among different views on and information of the DAB signal itself;
 * the button labeled **controls** controls the visibility of the so-called **configuration and control window**, a window that provides numerous settings for this software;

The bottom line in the picture above shows the transmitter name from the transmitter with the  strongest data, and - in this case - that the **avx2** instruction extension is used for some computations. (Qt-DAB can be configures with or wirhout the use of avx2 instructions ro spoeed up some computations). **Clicking** on that bottom line controls the visibility of the  **dxDisplay** (see below).

The picture below shows (part of) the left half of the main window. It shows a
list of services, and - at the bottom - a few buttons and selectors.

![6.10](/res/read_me/services.png)

 * the buttons labeled "+" and "-" can be used to scan through the list of services;
 * the up- and down arrow can be used to scan through the channels, a channel can be selected by touching an element of the combobox;
 * the button labeled **favourites** controls whether the list of services
from the currently selected channel or the list of favourites is shown,
 * the button labeled **scanlist** controls the visibility
of the scan list, i.e  *the list of all services seen*, shown below.

![6.10](/res/read_me/scanList.png)

(Obviously, the colors of the buttons, as well as the *font*, the *font size* and the *font color* of the service list shown can be set by the user, just click with the right hand mouse button on a button).

As mentioned, Qt-DAB supports **favorites**,
i.e. a list of (channel, service pairs). The list is maintained between program invocations. 
The **services widget** on the main window is shown in one of two modes, selectable by the button (in the picture labeled) **favourites**.
In **ensemble view** mode, the services in the current ensemble are shown, in the **favourites view** mode, the favourites are shown.

In both views, **selecting a service** is just by clicking on the service name.
Of course, when selecting a service in the list of favourites, it might take a few seconds before the software has switched over to the appropriate channel, and has
received sufficient information on the new ensemble carried in that channel before being able to select the service in that channel.

**Adding** a service to the favourites is by clicking on the field in the right hand column, shown in the services list.
If - in **ensemble mode** view - a service is also part of the favourites, the
field in the right hand column shows a mark. Clicking on such a mark **removes** the service from the list, as does clicking on the field in the *favourites view**.

![6.10](/res/read_me/technical-widget.png)

The technical window shows - as the name suggests - technical details of
the  selected audio service.

The buttons at the top of the window control **dumping** the audio
(".wav" file) resp. the AAC or MP2 frames into a file. AAC amd MP2 encoded files can be processed by e.g. VLC.

The **timeTable** button has only effect if on this channel (ensemble) an EPG 
service is or was active. Data from the EPG service is stored, and the
timetable is an interpretation of that data.

Below the **timeTable** button the static (meta)data of the selected audioservice is shown.

If the audiois also transmitted over FM, an indication of the relevant FM
frequencies is shown.

The three progress bars (quality indicators) for DAB+ give success rates of
resp. detecting a DAB+ frame, the RS error repair and the AAC decoding.
(For "old" DAB transmissions only a single quality indicator appears).

Below the progress bars the **rsCorrections/100** indicator shows how the amount of corrections that was made by the Reed-Solomon detector in the last 100 AAC frames (only used for DAB+ services).
Of course, the parity bits used by the RS decoding may contain bit errors themselves, which mot likely lead to CRC errors, the second indicator shows the amount of CRC errors detected after the correction by the RS decoding.

The third indicator tells the percentage of the AAC (or MP2) data that was
found to be OK.

The audio related data is shown above the spectrum,
the **scope** at the bottom shows the spectrum of the audio.

As mentioned above, a **spectrum window** can be made visible by clicking
on a button on the main window.
That window is equipped with a tab for selecting one of 6 views on the input
signal.

![6.10](/res/read_me/spectrum-scope.png)

In the view shown in the picture above, the spectrum of the incoming
 DAB signal is shown. To the right of this spectrum, one sees the
**signal constellation**, i.e. the mapping from the complex signals
onto their real and imaginary components. If the selector labeled
"ncp" is set, the centerpoints of the 4 lobs is shown. 

Below this constellation widget,  some **quality indicators** of the DAB signal
are shown.
From top to bottom
 * the channel and the frequency of the channel
 * the **computed** correction on the frequency of the incoming signal, applied to the signal;
 * the **freq error**, i.e. the remaining frequency error (in Hz) after
applying the correction;
 * the **SNR**, i.e.  the Signal-Noise Ratio in dB;
 * the **time offset**, the (relative) resulting error in sampling;
 * the **clock offset**, telling the offset in the samplerate;
 * the IQ unbalance, i.e. the (average) difference in strength between the I and the Q part of the DAB signal.

At the bottom of the window from left to right
 * a **sync** indicator, when **green** indicating that the software is synchronized with the incoming sample stream;
 * a **FIC** indicator, telling the (average) successrate of decoding the FIC part of the DAB frames (i.e. the data that specifies the payload);
 * a **BER**, Bit Error Rate, telling (on average) how many input bits were
wrong (and were corrected), per 1000 input bits (lower is better);
 * a **MER**, Modulation Error Rate, indicating the quality of the input signal (higher is better);

![6.10](/res/read_me/spectrum-ideal.png)

The ideal form of the spectrum and the signal constellation as shown in the
picture above is not often seen with real inputs.

![6.10](/res/read_me/qt-dab-correlation.png)

The **correlation** scope shows the correlation between the incoming signal and
predefined  data, i.e. the data as they should be.
**Correlation** is used to identify the input sample in the input stream
where the (relevant) data of the frame  starts.
The picture shows more than one peaks, i.e. the signal from
more than one transmitter is received. 
The software chooses either the largest peak, or - if selected - the
first peak one larger than a threshold.
The picture shows an estimate of the TII numbers near the peaks, it shows
that "(4, 21) Alphen aan den Rijn/Celinex toren" gives the strongest signal.

![6.10](/res/read_me/qt-dab-null-period.png)

A DAB signal is received as a sequence of samples, and can be thought to
be built up from *frames* (DAB frames) where each frame consists of 199608 consecutive samples.
The **amplitude** of the first app. 2500 samples is (almost) zero, the **NULL**
period.  The **NULL scope** shows the samples in the transition from the
**NULL** part to the first samples **with** data of a DAB frame.
It shows that samples 504 and up in the first data block are used.

![6.10](/res/read_me/qt-dab-tii-data.png)

In reality the **NULL** period is - in most cases - not completely
without signal, each second  **NULL period** may contain
an encoding of the TII (Transmitter Identification Information) data.
The **TII scope** shows a condensed form of the spectrum of the data
in the  relevant **NULL** period, the TII data is encoded as a 4 out of 8 code. Indeed, four larger (and four smaller) peaks can be seen in the picture. In this picture the pattern shown is 0x1e.

The DAB definition provides tables to map the recognized patterm
to  two 2 digit numbers, the latter is used to
identify the transmitter in a database, available in Qt-DAB.

![6.10](/res/read_me/qt-dab-channel.png)

The picture shows the channel response on the amplitude, and
the *red line*, i.e. the channel effects on the phase of the samples.
The picture clearly shows two larger and a few smaller peaks.

![6.10](/res/read_me/qt-dab-stddev.png)

The **deviation scope** shows the mean deviation of the carriers
in the decoded signal, ideally their phase is (a multiple of) 90
degrees. The phases are mapped upon (soft) bits, the larger the deviations,
the less reliable these bits are.
The Y-axis is in Hz.

![6.10](/res/read_me/configuration-and-control.png)

The **configuration and control** window contains checkboxes, spinboxes and
buttons with which the configuration of the decoding process can be
influenced.

**New** is a selector on the top line labeled **mapview**. When set, the
transmitter information that is sent to a map (see later on) is stored
into a file for off-line use.

Also  recent is the selector **local DB**. If enabled on
program startup Qt-DAB will load an embedded TII database,
rather than trying to load a TII database from the user's home directory
(i.e. implying that if not selected, the software will try to load a database
from your home directory).

Finally, also **new** is the selector **map close**.
If set, on switching the http handler off, an attempt is made to close
the map, and - if the map is closed - an attempt is made to switch off
the http handler.

![6.10](/res/read_me/service-and-titles.png)

Slightly experimental is the possibility to save **titles** (parts
of the dynamic labels) of (mostly) songs that are transmitted.
**This is by interpreting the so-called DL2 data, note that not all 
ensembles send DL2 data**.
The selector labeled **save titles** controls this, the titles - see picture
above - are stored in a file **DL2_titles.csv** in the Qt-DAB-files directory.

Just as a gadget, the window contains an indicator for the CPU load.
This load is the overall CPU load, not only the load from running
Qt-DAB.
Furthermore, the indicator, left of the CPU load text, here with value 1, tells how many backends are active simultaneously.

For a detailed description of all selectors, see the manual (or read the tooltips).

Devices and device support
======================================================================

In the current set up, Qt-DAB supports 6 types of (physical) input devices,
some network input, and file input:

  * DABsticks (RTL2838U or similar), with separate libraries for the V3 and V4 versions of the stick in the precompiled Windows versions;
  *  **All** SDRplay SDR models (RSP I,  RSP 1A and 1B, RSP II, RSP Duo, RSP Dx and RSPDxR2), with separate entries for the v2 and v3 library. **New** is extended support for the SDRPlay RspDuo. Tuner selection (and tuner 2 is connected to a port with biasT support) is now operational;
  * a special entry exists for the **SDRplay RSPDuo**, one for running both tuners simultaneously;
  * HACKRF One; 
  * Airspy, including Airspy mini (be aware that AirspyHF is **not** able to provide the samplerate required for DAB);
  * LimeSDR; 
  * Adalm Pluto;
  * **untested** UHD (anyone wants to help testing?).

![6.9](/res/read_me/rtlsdr-control.png?raw=true)
![6.9](/res/read_me/rsp-duocontrol.png?raw=true)

Apart from the untested UHD device, support for these 6 device types is
commonly included in the precompiled versions.
It was noted by users that when using the support library for the V4 version of the RTLSDR (aka DABsticks) devices  with V3 devices  the software seemed rather deaf.
To accommodate that, there are **two** device entries, 
one supporting the V3 versions of the DAB sticks, the other for the V4
versions.

In Qt-DAB the approach is to **dynamically** load the functions from the manufacturer's device library as soon as a device is selected (and not sooner).
This approach allows distributing versions that are configured with devices not installed on the user's system.

For the Windows version(s), the device libraries for almost all configured devices are provided in the installer. The exception are the SDRplay and Pluto devices.
For SDRplay devices the user has to install the drivers from the SDRplay site,
for Pluto support one should see the instructions in "https://github.com/analogdevicesinc/plutosdr-m2k-drivers-win".
For Linux users, Ubuntu provides the required libraries in a repository (i.e. libii0 and libad9361, for e.g. Fedora the support seems to stop at F33.

Qt-DAB also supports input using a network:
  * an rtl_tcp server connected to an RTLSDR device.
  * a **spyServer** (both 8 bit and a 16 bit version), i.e. from AIRSpy devices and RTLSDR devices.

Be aware that Qt-DAB processes the input with 2048000 Samples/second. Using the 16 bit version of the spyServer - 4 bytes per sample - requires a bandwidth of at least 8 M. I am the 8 bit version of the spyServer, as well as
the rtl_tcp server using it with a wired connection between two laptops, using the WiFi is not very successful.

![6.9](/res/read_me/spy-server16-control.png?raw=true)

Note that - as the control window suggests - it is possible to "dump" the unprocessed input to an xml type file (as can be seen on the control picture, the selected samplerate is 2500000).

Qt-DAB furthermore supports
  * **Soapy** (Linux only, not included in the AppImage), a renewed Soapy interface driver is even able to handle other samplerates than the required 2048000 (limited to the range 2000000 .. 4000000).

![6.9](/res/read_me/soapy-control.png?raw=true)

Qt-DAB obviously supports:
 * reading (and writing) ".sdr" type files from the input, where ".sdr" type is a form of ".wav" file with IQ samples with fixed inputrate 2048000. Qt-DAB generates such files. As an extension to classical "riff" files that are limited
to 4 Gb, Qt-DAB is able to handle (i.e. generate and read)
".wav" files with a size  **larger than 4 Gb**, their type is BW64.

![6.8](/res/read_me/riff-reader-large.png?raw=true)
![6.8](/res/read_me/riff-reader-small.png?raw=true)

The device window gives information on the "type" of the file i.e. RIFF or BW64.
When reading input from an ".sdr" file that was **generated by Qt-DAB** 
both the name of the SDR device as well as the channel frequency of the reception is displayed as shown in the pictures above.

 * reading prerecorded dump rtlsdr type "raw" (8 bits) files. The RTLSDR device handlers show a button "dump" for dumping the raw input into a ".raw" file.

 * reading (and writing) so-called "xml" files, i.e. a file format preserving the precise structure of the input samples. All device handlers show on their device window a button to control dumping the unmodified input into an xml file (e.g. for the Airspy this means with a samplerate of 25000000 or 3000000).

![6.8](/res/read_me/xml-reader.png?raw=true)

Scan control
=======================================================================

A separate window - visible under control of the **scan** button on the
main window - provides full control on scanning. Qt-DAB provides different scanning modes: scan to data, single scan and scan continuously.

![6.8](/res/read_me/scan-widget.png?raw=true)

**New** is the spinbox, at the left side with the default value 20, that
tells that if a channel is encountered with SOME DAB data, the
software will be tuned to that channel for the number of seconds specified
to collect DAB data. As said, the default value is 20 (seconds),
increments are 20 (seconds).

 * With **single scan**, i.e. a (single) scan over the channels of the band, a listing is produced of (the contents of) all ensembled encountered. In the scan window the channels where data was found, and the transmitters found for these channels, are given, as shown in the above picture.
 * With **scan to data** scanning starts and continues until a channel is detected that carries DAB data (or scanning is stopped by touching the *stop* button).
 * With *scan continuously*, i.e. scanning until stopped by the user, a single line is shown for each ensemble encountered, and - as the name suggests - scanning goes on until stopped by the user.

To allow **skipping** given channels when scanning, Qt-DAB supports the notion of a **scantable**, a table in which channels can be marked for skipping.
Next to a default scantable - which is stored in the users ".ini" file,
scantables can be created as separate files and read-in when required.

The *show* button controls the visibility of the **scantable**, scantables
can be loaded and stored in either the ".ini" file (use the "...default" buttons, or can be kept as xml file on a user defined place (the other load/store buttons).

The table at the bottom of the window is just for convenience, on scanning it displays the channel name being scanned currently, the ensemble name encountered and the number of services detected in the ensemble. Only for *scan single* the
transmitters that were identified are shown as well.

Displaying TII data
=======================================================================

As mentioned, transmitters (usually) transmit some identifying data, the TII (Transmitter Identification Information) data. Qt-DAB uses a database (gratefully made available by "www.fmList.org") to map the decoded TII data to name and location of the transmitter.

In recent versions, a copy of that database is part of the precompiled
versions. The **configuration and control** window contains selector
that by default is set "on", meaning that on program startup that 
database is used.
A **fresh** copy of that database can be installed in the user's home directory
by a small utility, a **db-loader**, precompiled for Windows and Linux-x64.
The **db-loader** installs the database, file ".txdata.ti" in the user's home directory, that is where Qt-DAB expects a database - with that name - to be found.
The **configuration and control** window contains a button to load the
database from the user's home directory into the program.

![6.10](/res/read_me/db-loader.png?raw=true)

Alternatively (and for other computing environments) one can **download** a
 reasonably up to date copy of the database from the directory **helpers**
in the Qt-DAB repository.
Unpack the zipped file "tiiFile.zip" and name it ".txdata.tii" in the
home directory.

As was shown earlier, one usually receives signals from more than
one transmitter.
While the line at the bottom on the right half of the Qt-DAB's main
window  always shows the strongest transmitter,
Qt-DAB can also show data of **all** identified transmitters on a
small separate window (if selected), the **dxDisplay**.

![6.10](/res/read_me/new-dxDisplay.png?raw=true)

The picture shows that the ensemble, transmitted in channel 12C is "NPO",
and it shows for each of the identified transmitters some data.
The first column shows a mark for the strongest transmitter, the pattern is shown as well as the **TII values** (the mainId, and the subId) of
the identified transmitters.
Furthermore, the **azimuth** from and the distance to my home location,
the power of the transmitter, the altitude (we are in a low-laying part of the country), and the height of the transmitting antenna.

The **compass** shows the direction of the signal from the strongest
transmitter.

![6.10](/res/read_me/QTmap.png?raw=true)

Qt-DAB has as said - on the main window -  a button labeled **http**,
when touched, a webserver is started  with on it a map centered around the
home position, that - when running - shows the position(s) of the
transmitter(s) received. 
(Note that since recently, Qt-DAB contains a **default** home location -
somewhere in Amsterdam - that - together with  default database - allows the
software to handle TII data and show the result of decoding on a map).
It is advised to update the user's home location, the **configuration
and control** window contains a button **coordinates** that when touched,
shows a small menu where latitude and longitude can be filled in).

The picture shows the channels I receive with a simple whip next to my "lazy chair". Of course, using a more advanced antenna. more transmitters show, as seen on the picture below (courtesy of Herman Wijnants)

![6.10](/res/read_me/good-antenna.png?raw=true)

The  webbrowser listens to port 8080. By default, the "standard" browser
on the system is activated. The *configuration and control* window
contains selector changing the port as well as turning the automatic
activation off.

**NEW** is the possibility of saving the data of the transmitters that are
shown on the map into a file and show the transmitters (or a selection)
on a map **off-line**.
The configuration window contains a selector that - when set -
tells the software to save the map contents into a file.

![6.10](/res/read_me/selecting-map.png?raw=true)

For that purpose, a small utility is available. The utility allows

![6.10](/res/read_me/map-viewer.png?raw=true)

EPG Handling and time tables
=================================================================

While not here in the Netherlands, in many other countries an ensemble
contains an **EPG** or **SPI** service.
Such a service contains data for service logo's and for  **time tables**.
If such a service is detected within a handful of seconds after the start of the channel, Qt-DAB will attempt to start the service as background task.
If the EPG/SPI service was identified late, the service can be started manually and will also run as background task.

Data decoded by this service will be stored in a separate directory that is itself stored in the user's Qt-DAB-files directory.

![6.10](/res/read_me/bbc-3.png?raw=true)

If sufficient data is read in that directory, the software **might** find a service logo and a time table for the selected service.
The logo - if found - is shown on the main window (picture above) next to the service name.
The time table - if found - is made visible by touching the timeTable button on the technical window.

![6.10](/res/read_me/timetable.png?raw=true)

If no data for the time table is found, the time table will say so

![6.10](/res/read_me/timeTable-notfound.png?raw=true)

The timeTable window contains a **next** and **prev** button to scan through different dates. Older pages are not removed by default, selecting a page and touching the "remove" button will remove the currently selected page.

Journaline data
================================================================

While not in the region where I live, in some countries (Germany) DAB services are sometimes augmented with Journaline data. This data is - at least in the examples I have - transmitted in a subservice as shown in the picture
Qt-DAB uses the *NewsService Journaline (R) Decoder* software from Fraunhofer IIS Erlangen in a slightly modified form (all rights gratefully acknowledged).

![6.10](/res/read_me/journaline-1.png?raw=true)
![6.10](/res/read_me/journaline-2.png?raw=true)

(Categorie names with an asterisks attatched show that new data is available)

Logging errors
=================================================================

Reporting of errors is now done in a (more or less) systematic way.
Especially errors with devices (I often get error messages when
setting gain values in my SDRplay equipment), such as a device that
does not start properly.

While Linux is essentially command line based and error messages
are - usually -printed on the command window, the command window in
the precompiled Windows versions is always switched off and
error messages are "lost".

A class was added to save error messages. These messages are written
into a file with the name *errorlog.txt*,  in the
directory (folder if you wish) *Qt-DAB-files*, in the user's home
directory.

Error messages take the form 

  * dabstick at 2025-09-01T13:35:00 -> No rtlsdr device found 

i.e, an entity (usually a device name) in which the error occurred, the
date and the error message itself.

Documentation
=================================================================

Most selectors on the various windows have a **tool-tip** that explains
theur function.

An introductory text, covering the regular use
is available and includes in the latest **release**.

Installation on Windows
=================================================================

For Windows an  **installer** can be found in the releases section of this repository
 * https://github.com/JvanKatwijk/qt-dab/releases.

Currently, there are two versions, one with the name ending in *-scalar*,
the other one with the name ending in *-avx*. As mesntioned earlier,
the suffix tells whether or not *avx2* instructions are used in
some of the computations or not. Older CPU's do not support these
instructions.

The installer will install the executable as well as required libraries,
although for both SDRplay devices (when used) or for the Adam Pluto (when
used), one has to install libraries from the provider of the device (see below).

Installation on Linux-x64
=================================================================

For Linux-x64 systems, an **appImage** can be found in the releases section of
this repository
 * https://github.com/JvanKatwijk/qt-dab/releases.

As for Windows, two versions are available, one with and one without use of *avx2* instructions.

The appImage contains next to the executable Qt-DAB program, the required interface libraries **but not the support libraries for the configured devices**. If you want to use a physical device - e.g. a DABstick, an SDRplay, or an AIRspy you need to install the driver libraries for the device as well.

Software for device support
===========================================================================

For using an **SDRplay** device one should download the - proprietary - driver software from the SDRplay site. Note that while Qt-DAB has a device entry for
the "old" 2.13 library, that library does not support
the newer SDRPlay device model such as the SDRPlay 1B, the SDRplayDx
and the SDRPlay Dx-II. Use the 3.XX library instead.
The libraries can be found on the website of SDRplay
 * www.sdrplay.com

For using an **AIRspy** or a **Hackrf device** the Windows Installers contain support libraries. For **Linux**, the Ubuntu (and bullseye repository for the RPI)
the repository contains appropriate software. 

For using an **RTLSDR device** the Windows Installers contain support libraries.
For **Linux**, the Ubuntu (and bullseye) repositories do provide a package. However, using that package one needs the
kernel module to be blacklisted, see e.g.
 * https://www.reddit.com/r/RTLSDR/wiki/blacklist_dvb_usb_rtl28xxu/

Personally, I prefer to build a version of the library myself,  installation is easy, see:
 * "https://osmocom.org/projects/rtl-sdr/wiki".

For using the **LimeSDR device** the Windows Installers contain support libraries.
For **Linux** I went back to the sources and compiled the
support library myself, see:
 * "https://wiki.myriadrf.org/Lime_Suite".

For installing the support software for the Adalm Pluto for both Windows and
Linux I followed the instructions on

 * "https://wiki.analog.com/university/tools/pluto/users"

Note that Ubuntu releases provide libraries for supporting the Pluto.

Building an executable: a few notes
=====================================================================

As mentioned, Windows an installer, for Linux an AppImage is available.
Since the sources are available, it is certainly possible to
build and executable.
I am doing the development on a Fedora box, but I am building the AppImage
on an Ubuntu 22 VM.
For Windows, I have installed Mingw64/MSYS and build the Windows version
on it.

It is strongly advised to use qmake/make for the compilation process,
the *qt-dab-6.9.pro* file contains (much) more configuration options
than the *CMakeLists.txt* file that is used when using cmake.

Note that the scheme presented below is applied when building the AppImage
on Ubuntu 22, and the Windows version using MSYS/Mingw64.

For other distributions (or later Ubuntu versions), names of library
packages may be different. Note that in all cases, the development versions (i.e. the versions with the include (".h") files) are required.

For creating an executable on and for Windows the easiest approach is
to install msys/mingw and follow the process as sketched.
Using an MS toolchain on Windows was for me (I tried it once) not successfull
and, since I develop compile and cross compile on a Fedora Linux box, my interest in developing ON windows is less than zero.

Step 1
-----------------------------------------------------------------

- :information_source:  In the repository, the sources for the current Qt-DAB version (6.10) are in the directory "qt-dab/ sources". All sources and include files are found in this directory". The ".pro" file is - as is the CMakeLists.txt file - in the "qt-dab" directory.
The qt-dab repository contains a file "structure.md" in which the structure is explained.

Running with the ".pro" file as in the repository, the resulting Qt-DAB executable is stored in a directory "/linux-bin" when compiled for Linux and in a directory "/d/systems/qt-dab/linux-bin" when compiling
for windows (using mingw64). You probably want to modify it.

For building the AppImage on Ubuntu 22, I load the required libraries as given below:

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

- :information_source:  An issue is getting the required qwt library. On my Fedora box, a qwt-6.2 version for Qt6 is available, Ubuntu does not provide
the Qt6 version for qwt. I had to install it myself as shown below

 *   Download qwt-6.30 from "https://sourceforge.net/projects/qwt/files/qwt/6.3.0/";
 *   follow the instructions (i.e. unzip, *cd* to the unzipped folder) and adapt the config file to your likings;
 *   building is then simple (takes some time though): "qmake6 qwt.pro", "make";
 *   install the library ("sudo make install") and inform the loader "sudo ldconfig";
 *   Note that the default for installation is "/usr/local/qwt/6.3.0", adjust the PATH settings accordingly.

- :information_source: While the *libfdk-aac-dev* package in both Fedora and Ubuntu 24 seems to work fine, I had some problems with the package from the repository in Ubuntu 22. For the AppImage, built on Ubuntu 22, a library version was created from the sources to be found as github repository:

* "https://github.com/mstorsjo/fdk-aac"

The sources contain a *CMakeLists.txt* file, building and installing is straightforward.

Alternatively, one could configure for *libfaad*, change the configuration to

*  CONFIG	+= faad
 *  #CONFIG	+= fdk-aac

and install the libfaad package
 *   sudo apt-get install libfaad-dev

- :information_source: If "soapy" is configured, libsamplerate (both the library and the include files) should be installed on the development system. Of course the various Soapy libraries as well.

Step 2
-----------------------------------------------------------------

While there are dozens of configuration options, take note
of the following ones:

- :information_source: A new viterbi decoder is part of the sources, one may choose between this "new" one and the "older" version derived from the spiral project.

	CONFIG          += viterbi-scalar
	#CONFIG         += viterbi-sse
	#CONFIG         += viterbi-avx2
	#CONFIG         += spiral-sse
	#CONFIG         += spiral-no-sse

If unsure, choose either "viterbi-scalar" or "spiral-no-sse"

While the cou load is somewhat higher when using a scalar version,
the implementation does not use any special CPU specific instructions.
Default is there using a *scalar* version of the viterbi decoder, which works just fine.

- :information_source: Qt-DAB can be compiled with floating numbers as "floats" or as "doubles"

	*CONFIG += single*

	*CONFIG += double*.

In the latter case, all computations in the "front end" are done with double precision arithmetic.

- :information_source: Devices like SDRplay, AIRspy, RTLSDR dongle, LimeSDR,
HackRf and Adalm Pluto can be included in the configuration *even if no support library is installed*. (Note that including *Soapy* requires Soapy libraries to be installed, so this does not apply for Soapy). Qt-DAB is designed such that on selecting a device in runtime, the required functions from the device library are linked in.

- :information_source: The Soapy library used in Ubuntu 22 (used for tessting the AppImage) is incompatible with Soapy libraries installed on other versions of Ubuntu and other Linux distributions, therefore *Soapy* is **NOT** configured
for inclusion in the AppImage.

- :information_source: Uncomment the line *DEFINES += __THREADED_BACKEND* if you intend to have more than one backend running simultaneously. E.g. activating the automatic search for an EPG service starts a separate service if such a service is found. With this setting each backend will run in its own thread.
 
Step 3
-----------------------------------------------------------------

Run *qmake* (variants of the name are *qmake6*, *qt6-qmake*, etc),
which generates a *Makefile* and then run *make*.  *Compiling may take some time*.
Use *make -j XX* for speeding up the build process, with XX the amount
of parallel threads used. Of course, qmake will complain if not all
required libraries can be found.
If all libraries were found, this step should result in an executable.

Step 4
-----------------------------------------------------------------
Use the database downloader (for Windows and Linux available as
precompiled item) to download a fresh copy of the database. Or download a
copy of the database from the repository.

A copy of the database is part of the repository. The directory **helpers**
contains a file "tiiFile.zip", unpack the zipped file and store the result
in your homedirectory (folder).

If Qt-DAB does not *see* the database, it will just function without mapping TII data onto names and locations.

Some comments
================================================================

A user compiled Qt-DAB-6.9, on and for an RPI,  and met the following issues:

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

Copyright and acknowledgements
=================================================================

	Copyright (C)  2016 .. 2025
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	Copyright of libraries used - Qt, qwt, fftw, portaudio,
	libusb-1, libfaad, libfdk-aac, - is gratefully acknowledged.

	In developing Qt-DAB many people have contributed, special thanks
	to 
	* Herman Wijnants,
	* Andreas Mikula,
	* and Jarod Middelman
	for continuous feedback and suggestions, and

	* Rolf Zerr (aka old-dab),
	* Stefan Poeschel,
	for important code contributions.

	Qt-DAB is distributed under the GPL V2 library, in the hope that
	it will be useful, but WITHOUT ANY WARRANTY; without even the
	implied warranty of MERCHANTABILITY or FITNESS FOR A
	PARTICULAR PURPOSE.  See the GNU General Public License for
	more details.
