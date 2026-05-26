# Qt-DAB-7.1

-------------------------------------------------------------------

![6.10](/res/read_me/qt-dab-logo.png?raw=true)


About Qt-DAB
======================================================================

*Qt-DAB* is software for Linux, Windows, MacOS and Raspberry Pi for listening to terrestrial **Digital Audio Broadcasting (DAB and DAB+)**.

![6.10](/res/read_me/qt-dab-front-picture.png?raw=true)

Of course, as for previous versions, for the current version,
*Qt-DAB-7.1*, predefined executables - for Linux an AppImage, for Windows
an installer - are available.

Table of Contents
=================================================================
* [What is new in Qt-DAB-7.1](#What-is-new-in-Qt-DAB-7.1)
* [Handling packet services](#handling-packet-services)
* [Windows and scopes](#windows-and-scopes)
* [Devices and device support](#devices-and-device-support)
* [Scan control](#scan-cntrol)
* [Displaying TII data](#displaying-tii-data)
* [EPG Handling and time tables](#epg-handling-and-time-tables)
* [Journaline data](#journaline-data)
* [Installation on Windows](#installation-on-Windows)
* [Installation on Linux](#installation-on-Linux)
* [Software for device support](#softare-for-device-support)
* [Building an executable: a few notes](#building-an-executable-a-few-notes)
* [Copyright and acknowledgements](#copyright-and-acknowledgements)


What is new in Qt-DAB-7.1
======================================================================

Since the fourth quarter of 2023 the major version number for Qt-DAB was "6",
the GUI had changed a lot since the "5" version, and the basic idea was to
increment **major** version numbers when the GUI changed significantly.

Following the Beta version of 7.0, a number of important changes was made,
so it felt better to update to 7.1.

Changes in functionality show in the **ensemble display** which is
completely redesigned, and in the way **packet** services and
services in the background are dealt with.
For the latter, see a next section.

The major visible change in the main window has to do with the way services
are shown.
  *
![7.1](/res/read_me/qt-dab-7.1.png)

The ensemble display is in one of a three modes:
 * **ensemble** mode, where  services of the (current) channel(s) are shown, 
 * **favorite** mode, where only the favorites are shown, and
 * **file** mode, where the services from a file are shown.

 **ensemble mode** - the most common mode - a list is compiled of
services encountered, and - depending on the settings - the services
of the currently selected channel. or all encountered services
are shown.
 
On  (normal) **program termination**, the list is stored in a file with the
name ".qt-dab-serviceList.mxl", stored in the user's home directory
1n program startup, the list is read-in again.

Alternatively, the **configuration and control** window contains an
entry that - when set - shows on program termination a  dialog with options
to read on the NEXT program invocation either an existing ensemble file, create a new one, clear the current
ensemble file, or just stop with selecting.
In the last two cases, the setting on the configuration window is "undone".

In **ensemble** mode, a service in the list can be made **favorite**, by
clicking on the left column.

![7.1](/res/read_me/ensemble-dialog.png)

The window contains at the bottow - below the channel selector - a button
with which a switch can be made between **ensemble** mode and **favorite** mode.

![7.1](/res/read_me/favorite-mode.png)

In **favorite** mode, the list of favorites is shown. Selecting this mode makes channel selection (and scanning)  rather useless and in Qt-DAB-7.0 channel selection and scanning is blocked in this mode.

![7.1](/res/read_me/file-mode.png)

In **file** mode, i.e. after reading a file, the services are shown, and
- obviously - may be selected.

On starting the program with a **device** the **ensemble** mode is
set, when starting with a file as input, the mode is set to **file** mode.

The configuration window has a checkbox for choosing between **only audio services** or **all services**. If **all** services is selected, data services
are shown as well, and only the services of the current channel are shown.
x
Eliminating buttons and replacing some
=======================================================================
It is always nice if settings to **start** or **stop** activities related to
the processing are "within reach", i.e. directly reachable from the main window.
A number of such settings were put on the configuration and control window,
for some of them the functionality is moved back to the main window

 * selecting an audio channel was as combobox on the configuration window; since it is a real "control" item rather than a "configuration" item, its functionality is moved to the main window, and implemented by **clicking** on the speaker symbol (clicking with the right hand mouse button on the speaker now controls the muting);
 * as meantioned earlier, the "dump" button on the configuration window was removed. Dump functionality is obtained by clicking with the **right hand mouse button** on the ensemble name;
 * similarly, the button on the configuration window to control the visibility od the device control window is removed. Clicking with the (again) **right hand mouse button** on the blue icon gives the same functionality;
 * the selector for the audio channel was removed from the configuration window.Clicking with the mouse on the **speaker symbol** shows the available channels in a small separate window.

The buttons for selection between Portaudio and QtAudio as well as for "skin" selection are removed, their functionality is not considered very useful.

Handling packet and background services
========================================================================

DAB (DAB+) supports next to audio services also packet services, the EPG/SPI
service (if available) is an example: Qt-DAB starts a backend interpreting the
EPG/SPI data in a separate thread as **background task**, invisible
to the user.

While the default setting in the configuration window in Qt-DAB is to show
only audio services, the "audio only" setting can be "unset", and 
the audio services and - if available - the data services are shown.

Selecting a packet service is the same as selecting an audio service,
just clicking on it starts processing.
However, running a packet service is  usually rather boring,
since there is not much to be seen or heard, that is why
Qt-DAB akways starts a packet service as background task.

**Starting** a packet service is simply by clicking on the name, same
as for audio services, there is a restriction however,
the packet service will only be started if the service is defined in
the currently selected channel and the service is started in a thread running in the background.

Obviously, when switching to another channel, either implicitly by selecting
a service in a channel different from the currently selected one or explicitly
the service interpretation will be stopped.

Most package services contain some form of TPEG data,
which is usually encoded;  Qt_DAB is  unable to interpret these packets and 
Qt-DAB restricts itself to compile the frames
and send the data as UDP packet (default 8888, but can be set in the
configuration window).
(In case more than one TPEG service is chosen, for now, the data of
all selected services is sent through the same port.

The sourcetree contains in the directory helpers/tdc-client a simple
reader for udp packets.

**Manually** stopping a background service is possible, 
a separate facility was created to **stop** such a service.
For that purpose, a small window can be made visible showing all running
backends.

![7.1](/res/read_me/process-management.png)

(Note that the NPO and other Dutch broadcasters do not provide EPG/SPI
or other packet services, so the example is using a recording).

The figure shows that 5 services are running, the "Mode" indicator set to 1
tells the task is running in the background.

The picture shows an audio service, **Dlf** which carriers a
secondary service, DlfTXT.
Secondary services are also running in the background.

**Touching the name of the service running in the background stops
that service.**

The visibility of the **process monitoring** window is controlled by the
process counter on the configuration window.

![7.1](/res/read_me/process-management-2.png)

The number "5", displayed in the process counter,
 tells that  there are 5 backends running.

Note that it is also possible to run audio services **found in the currently
selected channel** in the background. The audio - for DAB mp2 format, for
DAB+ aac format - is written into a file that carries 
name of the service combined with the date.

Click with the **right hand mouse button** on the name of an audio service
in the services list, and the decoding is started.

![7.1](/res/read_me/process-management-3.png)

Manually stopping the service is by clicking again on the servicename.

-------------------------------------------------------------------------

Windows and scopes
=========================================================================


In the latest releases of 6.10 the **dump** button was already moved from the configuration window to the main window (the **button** implemented by a click
on the **right hand button** on the mouse on the ensemblename). Furthermore,
when dumpimg,  a small window is shown that remminds the user that dumping is going on.

![7.1](/res/read_me/dumping.png?raw=true)

In Qt-DAB-7.0 the **reset** button also moved to the main window (it was felt rather clumsy to first have to activate the **configuration and control** window before being able to press the **reset** button).

![7.1](/res/read_me/resetbutton.png?raw=true)

The reset button is located in the top line of the right half
of the main window.

![7.1](/res/read_me/speakerButton.png?raw=true)

The list of audio devices was traditionally shown as combobox, it was felt better to be able to access the list from the main window. Clicking with the
mouse of the speaker symbol shows (or hides) the list, a list that appears
in a separate window. Clicking with the right hand mouse button controls
the muting.

The other icons on the top line behave as in previous versions, i.e. the yellow one controls the visibility of the Qt-DAB-files directory (folder), and the
small blue one controls the visibility of the device list.
On the second line the "book style" icon controls the visibility of the
technical window, and the icon with label **EPG**, is shown in case
an EPG/SPI service is detected in the currently selected channel (Note that
the EPG/SPI service is NOT shown in the services list.)

Touching the **EPG** icon make a small window visible, the **timetable list**.
The list shows the entries in the ensemblelist, coloured **green** is
timetable data for the service could be detected, **red** otherwide.

![7.0](/res/read_me/timetable-catalog.png?raw=true)

Touching in this list on a green entry, shows yet another window, with
time table data.

![7.0](/res/read_me/timetable.png?raw=true)

(Note: our NPO is not that advanced that it provides EPG/SPI data, so the
development and testing uses file input).

As in previous versions, touching the **ensemble name** on the top left,
controls the visibility of the content table, i.e. a window that shows
the details of all services in the current ensemble.

![7.0](/res/read_me/content-table.png)

The technical window shows - as the name suggests - technical details of
the  selected audio service.
New is that - if available - the icon of the selected service is shown.

![6.10](/res/read_me/technical-widget.png)

The spectrum widget is not modfied

![7.0](/res/read_me/spectrum-scope.png)

In the view shown in the picture above, the spectrum of the incoming
 DAB signal is shown. To the right of this spectrum, one sees the
**signal constellation**, i.e. the mapping from the complex signals
onto their real and imaginary components. If the selector labeled
"ncp" is set, the centerpoints of the 4 lobs is shown. 

Below this **constellation widget**, 
some **quality indicators** of the DAB signal are shown.
From top to bottom
 * the channel and the frequency of the channel
 * the **computed** correction on the frequency of the incoming signal, applied to the signal;
 * the remaining **freq error** (in Hz) after applying the correction;
 * the **SNR**, i.e.  the Signal-Noise Ratio in dB;
 * the **time offset**, the (relative) resulting error in sampling;
 * the **clock offset**, telling the offset in the samplerate in Hz;
 * the IQ unbalance, i.e. the (average) difference in strength between the I and the Q part of the DAB signal.

At the bottom of the window from left to right
 * a **sync** indicator,  **green** indicates that the software is synchronized with the incoming sample stream;
 * a **FIC** indicator, telling the (average) successrate of decoding the FIC part of the DAB frames (i.e. the data that specifies the payload);
 * a **BER**, Bit Error Rate, telling (on average) how many input bits were
wrong (and were corrected), per 1000 input bits (lower is better) when processing the FIC;
 * a **MER**, Modulation Error Rate, indicating the quality of the input signal (higher is better);

![6.10](/res/read_me/spectrum-ideal.png)

The ideal form of the spectrum and the signal constellation as shown in the
picture above is not often seen with real inputs.

![6.10](/res/read_me/qt-dab-correlation.png)

The **correlation** scope shows the correlation between the incoming signal and
predefined  data, i.e. the data as they should be.
**Correlation** is used to identify the precise input sample in the input stream
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
to two 2 digit numbers, the numbers are used to identify the transmitter in a database, available in Qt-DAB.

![6.10](/res/read_me/qt-dab-channel.png)

The picture shows the channel response on the amplitude, and
the *red line*, i.e. the channel effects on the phase of the samples.
The picture clearly shows two larger and a few smaller peaks.

![6.10](/res/read_me/qt-dab-stddev.png)

The last scope shows some of the output of the ofdm decoding process, the
resulting bits have values between -127 to 127.

![6.10](/res/read_me/configuration-and-control.png)

The **configuration and control** window is completely redesigned.
Some selectors, buttons and checkboxes are removed, as mentioned the functionality is moved to (mainly using right hand mouse clicks) the main window.

Settings for the http and tpreg port, as well as the setting for the home position can now be set on the configuration window.

In the previous version, a selector **mapview** was added, that selector 
is now placed in a line with other http related settings.

Since the software - on startup - always loads a database, i.e. 
if one can be found in the user's how directory that one, otherwise a default version, there was no need for  selector for loading a database.

New are selectors labeled  **load selection** and **update check**.
If the **load selection**  selector is set, on program startup 
the user is asked for a filename to load a selection. If
  * no name is specified, the default is loaded,
  * a name is given and a file with that name exists, its content is loaded,
  * a name is given and no file with that name exists (yet), an empty file with that name is created.

Also new is a checkbox, bottom line right end, that - when set - instructs the software to checkon program startup on the availability of a new version of Qt-DAB.

Devices and device support
======================================================================

In the current set up, Qt-DAB supports 6 types of (physical) input devices,
some network input, and file input:

  * DABsticks (RTL2838U or similar), with separate libraries for the V3 and V4 versions of the stick in the precompiled Windows versions;
  * **All** SDRplay SDR models (RSP I,  RSP 1A and 1B, RSP II, RSP Duo, RSP Dx and RSPDxR2), with separate entries for the v2 and v3 library. **New** is extended support for the SDRPlay RspDuo. Tuner selection (and tuner 2 is connected to a port with biasT support) is now operational;
  * a special entry exists for the **SDRplay RSPDuo**, one for running both tuners simultaneously;
  * HACKRF One; 
  * Airspy, including Airspy mini (be aware that AirspyHF is **not** able to provide the samplerate required for DAB);
  * LimeSDR; 
  * Adalm Pluto;
  * **untested** UHD (due to lack of equipment)

![6.9](/res/read_me/rtlsdr-control.png?raw=true)
![6.9](/res/read_me/rsp-duocontrol.png?raw=true)

Apart from the **untested UHD device**, support for these 6 device types is
commonly included in the precompiled versions.
It was noted by users running Qt-DAB on Windows that,
when using the support library for the V4 version of the RTLSDR (aka DABsticks) devices  with a V3 device connected,  the software seemed rather deaf.
To accommodate that, there are in the Windows peecompiled version  **two** device entries, one supporting the V3 versions of the DAB sticks, the other for the V4 versions.

For the Windows version(s), the device libraries for almost all configured devices are provided in the installer. 
The exceptions are the SDRplay and Pluto devices.
 * For **SDRplay** devices the user has to install the drivers from the SDRplay site,
 * for Pluto support one should see the instructions in "https://github.com/analogdevicesinc/plutosdr-m2k-drivers-win".

For Linux users, Ubuntu provides the required libraries for Pluto in a repository (i.e. libii0 and libad9361, for e.g. Fedora the support seems to stop at F33.

Qt-DAB also supports input using a network:
  * an rtl_tcp server connected to an RTLSDR device.
  * a **spyServer** (both 8 bit and a 16 bit version), i.e. from AIRSpy devices and RTLSDR devices.
  * (NEW) is support for using the **SDRconnect** program as "input device".

Be aware that Qt-DAB processes the input with 2048000 Samples/second.
When connected over a network to the SDRconnect server, it x sends -
with a samplerate of 2000000 (which is converted to the required rate of 2048000),  4 bytes per sample over the network. My wifi cannot handle that.

The same applies to  using the other servers.
The rtl_tcp server sends 2 byte samples, i.e. a payload of 4096000 bytes per second; the 16 bit spyServer sends 4 byte samples (with an even higher rate), all leading to a transmission rate that a regular WiFi cannot handle.

![6.10](/res/read_me/sdrconnect.png?raw=true)

Qt-DAB furthermore supports
  * **Soapy** (Linux only, not included in the AppImage), a renewed Soapy interface driver is even able to handle other samplerates than the required 2048000 (limited to the range 2000000 .. 4000000).

![6.10](/res/read_me/soapy-control.png?raw=true)

In 6.10 soapy is renewed and now shows a deviceselector when more than
a single soapy-supported device is seen.

![6.10](/res/read_me/soapy-selection.png?raw=true)

Furthermore, soapy now supports xml (i.e. ".uff") files to be written,
although not in native format. 

Qt-DAB obviously supports:
 * reading (and writing) ".sdr" type files from the input, where ".sdr" type is a form of ".wav" file with IQ samples with fixed inputrate of 2048000 samples per secnd. Qt-DAB generates such files. As an extension to classical **RIFF** files that are limited to 4 Gb, Qt-DAB is able to handle (i.e. generate and read)
".wav" files with a size  **larger than 4 Gb**, their type is **BW64**.

![6.8](/res/read_me/riff-reader-large.png?raw=true)
![6.8](/res/read_me/riff-reader-small.png?raw=true)

The device window gives information on the "type" of the file i.e. RIFF or BW64.
When reading input from an ".sdr" file that was **generated by Qt-DAB** 
both the name of the SDR device as well as the channel frequency of the reception is displayed as shown in the pictures above.

 * reading prerecorded dump rtlsdr type "raw" (8 bits) files. The RTLSDR device handlers show a button "dump" for dumping the raw input into a ".raw" file.

 * reading (and writing) so-called "xml" files, i.e. a file format preserving the precise structure of the input samples. All device handlers show on their device window a button to control dumping the **unmodified** input into an xml file (e.g. for the Airspy this means with a samplerate of 25000000 or 3000000). Note that there is no 4Gb limit for xml type files.

![6.8](/res/read_me/xml-reader.png?raw=true)

Scan control
=======================================================================

A separate window - visible under control of the **scan** button on the
main window - provides full control on scanning. Qt-DAB provides different scanning modes: scan to data, single scan and scan continuously.

![6.8](/res/read_me/scan-widget.png?raw=true)

**New** is the spinbox, at the left side with the default value 20. The value of this spinbox tells that if a channel is encountered with SOME DAB data, the
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
versions which is automatically loaded if no database can be found in the user's home directory.

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
reading in a generated map file, and displaying it, per country,
per channel, per ensemble or completely.

![6.10](/res/read_me/map-viewer.png?raw=true)

EPG Handling and time tables
=================================================================

While not here in the Netherlands, in many other countries an ensemble
contains an **EPG** or **SPI** service.
Such a service contains data for service logo's and  uduslly for **time tables**.
If such a service is detected within a handful of seconds after the start of the channel, Qt-DAB will attempt to start the service as  **background task**.
If the EPG/SPI service was identified late, the service can be started manually and will also run as background task.

Data decoded by this service will be stored in a separate directory that is itself stored in the user's Qt-DAB-files directory.

![6.10](/res/read_me/bbc-3.png?raw=true)

If sufficient data is read in that directory, the software **might** find a service logo and a time table for the selected service.
The logo - if found - is shown on the main window (picture above) next to the service name.
The time table - if found - is made visible by touching the small icon
on the main window labeled "EPG".
(This selector is chosen since it is certain that if the icon is visible, there is at least an EPG/SPI service)

![6.10](/res/read_me/timetable-catalog.png?raw=true)

The timetable catalog shows for which channels their is **SOME** timetable data.

![6.10](/res/read_me/timetable.png?raw=true)

If no data for the time table is found, the time table will say so

![6.10](/res/read_me/timeTable-notfound.png?raw=true)

The timeTable window contains a **next** and **prev** button to scan through different dates. Older pages are not removed by default, selecting a page and touching the "remove" button will remove the currently selected page.

Journaline data
================================================================

While (again) not in the region where I live, in some countries (Germany) DAB services are sometimes augmented with Journaline data. This data is - at least in the examples I have - transmitted in a subservice as shown in the picture
Qt-DAB uses the *NewsService Journaline (R) Decoder* software from Fraunhofer IIS Erlangen in a slightly modified form (all rights gratefully acknowledged).

![6.10](/res/read_me/journaline-1.png?raw=true)
![6.10](/res/read_me/journaline-2.png?raw=true)

(Categorie names with an asterisks attatched show that new data is available)

Installation on Windows
=================================================================

For Windows an  **installer** can be found in the releases section of this repository
 * https://github.com/JvanKatwijk/qt-dab/releases.

Currently, there might be two versions, one with the name ending in *-scalar*,
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

As for Windows, two versions might be available, one with and one without use of *avx2* instructions.

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
