# Qt-DAB-7.3.0

-------------------------------------------------------------------

![6.10](/res/read_me/qt-dab-logo.png?raw=true)

About Qt-DAB
======================================================================

*Qt-DAB* is software for Linux and Windows, and can be compiled for MacOS and Raspberry Pi. It is used for listening to terrestrial **Digital Audio Broadcasting (DAB and DAB+)**.

![7.3](/res/read_me/qt-dab-front-picture-1.png?raw=true)
![7.3](/res/read_me/qt-dab-front-picture-2.png?raw=true)

Of course, as for previous versions, for the current version,
*Qt-DAB-7.3.0*, predefined executables - for Linux an AppImage, for Windows
an installer - are (or will be) available.

Table of Contents
=================================================================
* [What is new in Qt-DAB-7.3.0](#What-is-new-in-Qt-DAB-7.3.0)
* [Windows and scopes](#windows-and-scopes)
* [NEW: Managing service lists](#new-managing-service lists_
* [Devices and device support](#devices-and-device-support)
* [Handling packet services](#handling-packet-services)
* [Scan control](#scan-cntrol)
* [Displaying TII data](#displaying-tii-data)
* [Journaline data](#journaline-data)
* [Installation on Windows](#installation-on-Windows)
* [Installation on Linux](#installation-on-Linux)
* [Software for device support](#softare-for-device-support)
* [Building an executable](#building-an-executable)
* [Copyright and acknowledgements](#copyright-and-acknowledgements)

What is new in Qt-DAB-7.3.0
======================================================================

 - Based on user's request in Qt-DAB-7.3.0 the main window is - within limits - resizable.

 - In Qt-DAB-7.3.0 the "technical window" is incorporated in a frame in the main window, with the option to "hide" it. Similarly, the spectrum scope has a "carriers" window that is part of the window with as option to make it (in)visible.

 - in Qt-DAB-7.3.0 the **configuration and control window" was completely redesigned. The **schedule** button is (at last) moved to the main window, the **reset** button on the main window disappeared: it turned out that restarting Qt-DAB with the same of another input device does that job in an excellent way.

 - Since Qt-DAB-7.2  the qwt library is  not used anymore for the "scopes". The "compass" on the display for showing transmitters  was a widget in Qwt and is not (yet) reimplemented in Qt-DAB and not visible.
The feature *clicking with the right hand mouse button* on the scopes remains, i.e. in three clicks you can set  (1) background color, (2) grid color and (3) color of the curve;

 - Qt-DAB-7.3.0 is developed using Qt6 but can be compiled without problems using the Qt5 framework using "qmake -qt5" for generating a makefile. The CMakeLists.txt file  for using cmake contains some Qt6 dependencies;

In QtDAB-7.2 and 7.3 some GUI elements were changed:

 - The "waterfall" disappeared: the GUI of a running version of Qt-DAB is sufficently limited in size that it can be shown - including the other windows - on my laptop screen.

 - added is an option to show the full spectrum of the NULL part of the DAB frame (with options to select the NULL part carrying the TII data) or the mean value or sigmaSE values measured during decoding.

 - the **progressbars** on the technical window are replaced by simple indicators telling just whether the processing is OK (green) or not (red);

 - the technical window is made an optional part of the main window rather than a separate window.

 - the main window contains a button **channel view** that - when touched - shows in a separate window an overview of the channels in the current database.

![7.2](/res/read_me/channelView.png?raw=true)

 - the selector for setting the (maximum) mute time is moved from the configuration window. Click with the right hand mouse button on the speaker and a small window is activated where the time can be set.

Windows and scopes
=========================================================================

In the latest releases of 6.10 the **dump** button was already moved from the configuration window to the main window (the **button** implemented by a click
on the **right hand button** on the mouse on the ensemblename). Furthermore,
when dumpimg,  a small window is shown that remminds the user that dumping is going on.

![7.2](/res/read_me/dumping.png?raw=true)

In Qt-DAB-7.1.1 some buttons were added to the main window.
The selector of the list of audio devices was traditionally shown as combobox in the configuration and control window, 
now it is a separate small window, the visibility of which id controlled by a button on the main window. 
Some other buttons are just replaced on the main window.

![7.3](/res/read_me/qt-dab-7.3-a.png?raw=true)
![7.3](/res/read_me/qt-dab-7.3-b.png?raw=true)

The **main window** is visible as long as the program runs. The two pictures show the main window with and without the technical window visible.
The icon left of the service name (Here **Radio Poland**) controls the visibility of the frame containing the tech window data.


The left (almost) half is mainly reserved for selecting channel and service.
    * the top line, displaying the ensemble name implements as clickable item as well
    * clicking with the left mouse button, controls the visibility of the content table;
![7.0](/res/read_me/content-table.png)

    * clicking with the right mouse button control **dumping** the input.

![7.2](/res/read_me/dumping.png?raw=true)

Clicking on a service name selects the service with that name:

 * in **ensemble mode** and in **favorite mode**, this might involving a channel switch which may take a few seconds;

In ensemble mode, clicking with the **right hand** mouse button on an audio service other than the currently selected one, starts the service as background service;

 * in ensemble mode the bottom part of the left half contains next to the **next** and **previous** selectors for the services, a **channel selector** and a **mode selector**.

 * in **file mode** obviously only the services of the recording are shown and neither a mode selector nor a channel selector are visible. Furthermore, the **channelview button** is meaningless in this mode.

 * the channelview button controls the visibility of the channeldisplay. The display shows an overview of the channels in the current database and is - obviously - meaningless in **file** mode.

![7.2](/res/read_me/channelView.png?raw=true) 

Clicking on a line in the channeldisplay has the same effect has clicking on the channelselector.

============================================================================

The icons on the top line behave more or less as in previous versions
 * clicking on the **copyright symbol** shows some data on the Qt-DAB version and shows acknowledgements;
 * the yellow/blue  icon controls the visibility of the Qt-DAB-files directory (folder); 
 * the small blue icon controls the visibility of the device list;
 * the snr widget controls the visibility of a small window showing the progress of the SNR.

![7.2](/res/read_me/snr-window.png?raw=true)

On the second line:
 * the **book style** icon controls the visibility of the most right part of the main window, housing the technical window.

 * the icon with label **EPG**, is **ONLY** shown in case an EPG/SPI service is detected in the currently selected channel (Note that the EPG/SPI service is NOT shown in the services list.)

On the third line some  technical details on the currently selected service
shows, is shown. At the end on the right hand side the speaker symbol:
 * clicking with the **left** mouse button controls **muting** the signal;
 *clicking with the **right** mouse button shows a small window where the muting time (i.e. the time after which muting stops) can be set.

EPG and timetables
------------------------------------------------------------------------

If an EPG service is detected on starting a channel, Qt-DAB will attempt to start the service as  **background task**.  Data decoded by the EPG/SPI handler will be stored in a separate directory that is itself stored in the user's Qt-DAB-files directory.

EPG/SPI data may consist of two parts: 
   * icons for services. If on selecting a service an Icon is detected in the user's Qt-DAB-files directory, the Icon is made visible and takes the place of the **short name** of the service, both on the main window and the technical details window.

![7.2](/res/read_me/bbc-3.png?raw=true)

 * **Electronic Program Guide  Data**.
Touching the **EPG** icon make a small window visible, the **timetable list**.
The list shows the entries in the ensemblelist, coloured **green** is
timetable data for the service could be detected, **red** otherwide.

![7.2](/res/read_me/timetable-catalog.png?raw=true)

Touching in this list on a green entry, shows yet another window, with
time table data.

![7.2](/res/read_me/timetable.png?raw=true)

(Note: our NPO is not that advanced that it provides EPG/SPI data, so the
development and testing uses file input).

Note however, that in general it may take some time before sufficient EPG/SPI data is read in for the above mentioned functionality to be operational.

The slide
-----------------------------------------------------------------------

In the middle part of the right half of the main window **slides** are shown. By default some slides with old computer or radio stuff are shown. If, however, the service carries MOT data in the PAD part, the slides are shown whenever they are decoded.

 * **clicking** with the mouse on the slide area controls whether or not the incoming slides are saved. If they are saved, they can be found in a subdirectory within the directory where Qt-DAB stores data.

The dynamic label.
-------------------------------------------------------------------------

The dynamic labelis seen below the slide space. Clicking with the right hand mouse button on the text shows a (very) small menu which allows saving (part of) the text. There are two other ways to save the dynamic label text or a part of it

 * by selecting the **dll text** checkbox on the configuration and control window. Selecting this, asks for a filename where the content is to be stored;
 * by selecting the **save titles** option in the configuration and control window. This latter option will work if the service supports DL2. Data is stored in a file **Dl2_titles.csv** in the local storage area.

Technical window
----------------------------------------------------------------------------

The technical window shows - as the name suggests - technical details of
the  selected audio service.
New is that - if available - the icon of the selected service is shown.

![7.2](/res/read_me/technical-window.png)

As  said, the progress bars disappeared:  indicators tell whether
or not the steps in the transformation from raw data to audio are successfull.
Essentially, if all indicators are green there should be sound.

The window contains two buttons
 * the button labeled **AAC/MP2** when touched, asks for a filename where the AAC or MP2 (depending on the type of DAB service) is stored;
 * the button labeled **WAV** when touched asks for a filename where the resulting audio (PCM) is stored.

The buttons
---------------------------------------------------------------------------

Some buttons were moved from the configuration and control window tothe main window, resulting in two rows of buttons.
For each of the buttons a **tooltip** gives more details on the functionality controlled by that button.

The buttons:
 * the buttons labeled **controls**, **spectrum** just control the visibility of the **configuration and control** resp. the **spectrum scope* window;
 * the buttons **scan**, "audio" and "device" control the visibility of windows to control  **scanning**, selection of the audio channel, and controlling the selected device.
 * the button labeled **http** controls the http handler with which the transmitters detected can be made visible on a map;
 * the button labeled **schedule** controls scheduling; if touched a small window shows where a schedule command or a service can be selected for starting at a specified time and date within the next 7 days;
 * the button labeled **eti** controls the **eti-processor**, i.e. a processor with which the input can be transformed into "eti" compliant data files;

The bottom line
-------------------------------------------------------------------------

The bottom line of the right half of the main window may show a transmittername. If the software detects the TII data it will show the name of the transmitter with the strongest signal here.
Clicking on the line shows a table with the names of all transmitters detected.

![7.2](/res/read_me/new-dxDisplay.png?raw=true)

The spectrumscope and its widgets
--------------------------------------------------------------------------

As mentioned, it was felt that the waterfall did not add much to the understanding of the characteritics of the signal, and it was removed from the window.

The spectrumscope show on its left side six different "scopes".
Note that for each of these scopes the coloring may be set
Clicking with the right hand mouse button on the scope, shows a small window on which **three*  colors can be selected:

 * the first  selected color is the one for the background; I prefer black
 * the second selected one is the color for the "grid", 
 * the third selected one is the color for the data line.

The spectrum
-----------------------------------------------------------------------

![7.3](/res/read_me/spectrum-scope-a.png)
![7.3](/res/read_me/spectrum-scope-b.png)

The spectrum of the incoming DAB signal is shown.
To the right of this spectrum, one sees the
**signal constellation**, i.e. the mapping from the complex (i.e. "IQ") signals
resulting from the decoder, onto their real and imaginary components. If the selector labeled "ncp" is set, the centerpoints of the 4 lobs is shown. 

Below the constallation widget the name of the current channel as well as its frequency is shown.

Below the "scope" area, some **quality indicators** of the DAB input signal are shown:
 * the **computed** correction on the frequency of the incoming signal, applied to the signal;
 * the remaining **freq error** (in Hz) after applying the correction;
 * the **SNR**, i.e.  the Signal-Noise Ratio in dB;
 * the **time offset**, the (relative) resulting error in sampling time in Hz;
 * the **clock offset**, telling the offset in the samplerate in Hz;
 * the IQ unbalance, i.e. the (average) difference in strength between the I and the Q part of the DAB  input signal;

At the bottom of the window from left to right
 * a button controlling the visisbility of a small panel showing carriers;
 * a **sync** indicator,  **green** indicates that the software is synchronized with the incoming sample stream (i.e. time synchronization);
 * a **FIC** indicator, telling the (average) successrate of decoding the FIC part of the DAB frames (i.e. the data that implements a kind of "catalog" that describes the payload);
 * a **BER**, Bit Error Rate, telling (on average) how many input bits were
wrong (and were corrected), per 1000 input bits (lower is better) when processing the FIC;
 * a **MER**, Modulation Error Rate, indicating the quality of the input signal (higher is better);

![7.2](/res/read_me/spectrum-ideal.png)

The ideal form of the spectrum and the signal constellation as shown in the
picture above is not often seen with real inputs.

The correlation
------------------------------------------------------------------------

![7.2](/res/read_me/qt-dab-correlation.png)

**Correlation** is used to identify the precise input sample in the input stream
where the (relevant) data of the frame  starts.
The picture shows more than one peaks, i.e. the signal from
more than one transmitter is received (after all, the same DAB (DAB+) signal is transmitted by a pretty large number of transmitters);
The software chooses either the largest peak, or - if selected - the
first peak one larger than a threshold. A setting on the configuration window controls this.

The null scope
---------------------------------------------------------------------------

![7.2](/res/read_me/qt-dab-null-period.png)

A DAB signal is received as a sequence of samples, and can be thought to
be built up from **frames** (DAB frames) where each frame consists of 199608 consecutive samples.
The **amplitude** of the first app. 2500 samples is (almost) zero, the **NULL**
period.  The **NULL scope** shows the samples in the transition from the
**NULL** part to the first samples **with** data of a DAB frame.
It shows that samples 504 and up in the first data block are used.

The TII scope
--------------------------------------------------------------------------

![7.2](/res/read_me/qt-dab-tii-data.png)

In reality the **NULL** period is - in most cases - not completely
without signal, each second  **NULL period** may contain
an encoding of the TII (Transmitter Identification Information) data.
The **TII scope** shows a condensed form of the spectrum of the data
in the  relevant **NULL** period, the TII data is encoded as a 4 out of 8 code. Indeed, four larger (and four smaller) peaks can be seen in the picture. In this picture the pattern shown is 0x1e.

The DAB definition provides tables to map the recognized patterns
on to two 2 digit numbers, these numbers are used to identify the transmitter in a database, available in Qt-DAB.

The channel scope
--------------------------------------------------------------------------

![7.2](/res/read_me/qt-dab-channel.png)

The picture shows the channel response on the amplitude, and
the *red line*, i.e. the channel effects on the phase of the samples.
The picture clearly shows two larger and a few smaller peaks.

The bits scope
-------------------------------------------------------------------------

![7.2](/res/read_me/qt-dab-stddev.png)

The front end part of the software output "soft bits", encoded as values in the range -127 .. 127. Other parts of the software take soft bits in and - using visterbi decoding and in some case reed Solomon error repair - output "hard bit",
i.e. "1" and "0" values.

The carrier scope
-------------------------------------------------------------------------

![7.2](/res/read_me/qt-dab-carriers.png)

As an option - visiblity controlled by the nutton labled **carriers**, a  panel is added with a scope showing carriers.
In the current selection there is
 * the NULL period with TII data;
 * the NULL period without TII data;
 * the carriers of the sync block (i.e. datablock 0);
 * mean values of the carriers during decoding;
 * the sigma (i.e. average squared distance to the center) of the carriers.

Configuration and control
--------------------------------------------------------------------------

![7.3](/res/read_me/configuration-and-control.png)

The **configuration and control** window is completely redesigned and
selectors are grouped.

Some selectors, buttons and checkboxes are removed, as mentioned the functionality is moved to (mainly using right hand mouse clicks) the main window.
The remaining selectors are grouped according to the kind of function they
represent.

While the function for most selectors is quite obvious, there are some that need some explanation
 * **qt-audio**, with as default **set** selects the **sound engine**. The default setting is "on" indicating that Qt_DAB should try to select Qt libraries of  sound handling. If "off" the portaudio library is selected.
 * **save titles**. Some audio services attach to the dynamic label text some more information, information about the interpretation of text fragments. If this selector is  **set**, Qt_DAB extracts title information of the songs and stores this information - together with servicename and time - in a file in the directory where Qt_DAB stored all kinds of data, with a filename **DL2_titles.csv**;
 *  **check updates** - when set - checks on program startup for a newer version on of Qt_DAB executables on the repository;
 *  **input dump in xml** - when set - makes that dumps from the input are in so-called "xml" ("uff*) format, where the content is the input as delivered from
the inpt device. If not set the input dump is in PCM formt.
 * **all tii** - when set - tells the software that - on extracting tii data - the data also should be shown if no transmittername can be found in the database;
 * **save map** - when set - tells the software to save the transitterdata shown on the map to save in a file for use with an external program to re-view the map with the transmitters offline;
 * **show only audio services**. If **set** only audio services are chosen and the names are stored in a database. If **unset** only the services found in the currently selected channel are displayed, including data services (see next section on packet services);
 * **load selection** - when set - shows on program start up a small menu,
showing options to create a new database next to or instead of the default one.

Handling packet and background services
========================================================================

DAB (DAB+) supports next to audio services also packet services. The EPG/SPI
service (if available) is an example: Qt-DAB starts automatically a backend interpreting the EPG/SPI data in a separate thread as **background task**, invisible
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
as for audio services.
To avoid confusion, if the "audio only" selector is "unset", only the services of the currently selected channel are shown, including the data services.

Obviously, when switching to another channel, the service interpretation stops.

Most package services contain some form of TPEG data,
which is usually encoded;  Qt_DAB is  unable to interpret the TPEG data
and Qt-DAB restricts itself to compile the TPEG frames
and send the data as UDP packet to an IP port (default 8888, but can
be set in the configuration window).

The sourcetree contains in the directory helpers/tdc-client a simple
reader for just reading udp packets (with no further interpretation at all).

**Manually** stopping a background service is possible, 
a separate facility was created to **stop** such a service.
For that purpose, a small window can be made visible showing all running
backends (starting an audio service in the background shows the window,
its visibility is controlled by clicking on the number of services as shown
in the configuration and control window)

![7.1](/res/read_me/process-management-2.png)

The number "5", displayed in the process counter,
 tells that  there are 5 backends running.

![7.1](/res/read_me/process-management.png)

(Note that the NPO and other Dutch broadcasters do not provide EPG/SPI
or other packet services, so the example is using a recording).

The figure shows that 5 services are running, the "Mode" indicator set to 1
tells the task is running in the background.

The picture shows an audio service, **Dlf** which carriers a
secondary service, DlfTXT.
Secondary services are also running in the background.
**Touching the name of the service  in this control window  that is running in the background stops that service.**

Note that it is also possible to run audio services **found in the currently
selected channel** in the background. The audio - for DAB mp2 format, for
DAB+ aac format - is written into a file that carries 
name of the service combined with the date.

Click with the **right hand mouse button** on the name of an audio service
in the services list, and the decoding is started.

![7.1](/res/read_me/process-management-3.png)

Manually stopping the service is by clicking again on the servicename.

------------------------------------------------------------------------
New: Managing service lists
========================================================================

In **normal** operation, i.e. with on the **Configuration and control** window
the selector **show only audio services** set, and reading a real device,
the service list contains the (audio) services in all channels encountered.

For me, the list contains just the channels I normally visit, and from which I listen to a selected subset of services.
From time to time I do an experiment, where I want to maintain my service list,
and where I want a separate service list.

Qt_DAB supports maintaining several service lists. If the selector **load selection* is set, a small window appears **at the first subsequent (cold or hot) start.

![7.3](/res/read_me/load-selection.png?raw=true)

The window shows a selection:
 * **Open existing file**, when selected a file selection menu shows on which a file can be selected with a previously built service file;
 * **Open existing file and set as default**, as the previous case, but now the filename is set as default;
 * **Create new servicelist**, when selected a new save file can be selected, the file will be empty of start. During processing it will be filled;
 * **Clear default servicelist** does what the name suggests, the defeult service list is empties;
 * **Forget it** is useful when you have second thoughts


-------------------------------------------------------------------------
Devices and device support
======================================================================

In the current set up, Qt-DAB supports 6 types of (physical) input devices,
some network input, and file input in some common formats.

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

For Linux users, Ubuntu provides the required libraries for Pluto in a repository (i.e. libii0 and libad9361), for e.g. Fedora the support seems to stop at F33.

Qt-DAB also supports input using a network:
  * an rtl_tcp server connected to an RTLSDR device.
  * a **spyServer** (both 8 bit and a 16 bit version), i.e. from AIRSpy devices and RTLSDR devices.
  * (NEW) is support for using the **SDRconnect** program as "input device".

Be aware that when using SDRconnect, Qt-DAB processes input with a rate of 2048000 Samples/second. When connected over a network to the SDRconnect server,
it sends - with a samplerate of 2000000 (which is converted to the required rate of 2048000),  4 bytes per sample over the network. My wifi cannot handle that.

![7.2](/res/read_me/sdrconnect.png?raw=true)

The same restriction applies to using the other servers.
The **rtl_tcp** server sends 2 byte samples, i.e. a payload of 4096000 bytes per second; the 16 bit spyServer sends 4 byte samples (with an even higher rate), all leading to a transmission rate that a regular WiFi cannot handle.

Qt-DAB furthermore supports
  * **Soapy** (Linux only, not included in the AppImage), a renewed Soapy interface driver is even able to handle other samplerates than the required 2048000 (limited to the range 2000000 .. 4000000).

![7.2](/res/read_me/soapy-control.png?raw=true)

In Qt_DAB-6.10 the soapy driver was renewed and now shows a
deviceselector when more than a single soapy-supported device is seen.

![7.2](/res/read_me/soapy-selection.png?raw=true)

Furthermore, soapy now supports xml (i.e. ".uff") files to be written,
although not in native format. 

Qt-DAB obviously supports:
 * reading (and writing) ".sdr" type files from the input, where ".sdr" type is a form of ".wav" file with IQ samples with fixed inputrate of 2048000 samples per second. Qt-DAB generates such files. As an extension to classical **RIFF** files that are limited to 4 Gb, Qt-DAB is able to handle (i.e. generate and read)
".wav" files with a size  **larger than 4 Gb**, their type is **BW64**.

![7.2](/res/read_me/riff-reader-large.png?raw=true)
![7.2](/res/read_me/riff-reader-small.png?raw=true)

The device window gives information on the "type" of the file i.e. RIFF or BW64.
When reading input from an ".sdr" file that was **generated by Qt-DAB** 
both the name of the SDR device as well as the channel frequency of the reception is displayed as shown in the pictures above.

 * reading prerecorded dump rtlsdr type "raw" (8 bits) files. The RTLSDR device handlers show a button "dump" for dumping the raw input into a ".raw" file.

 * reading (and writing) so-called "xml" files, i.e. a file format preserving the precise structure of the input samples. All device handlers show on their device window a button to control dumping the **unmodified** input into an xml file (e.g. for the Airspy this means with a samplerate of 25000000 or 3000000). Note that there is no 4Gb limit for xml type files.

![7.2](/res/read_me/xml-reader.png?raw=true)

Scan control
=======================================================================

A separate window - visible under control of the **scan** button on the
main window - provides full control on scanning. Qt-DAB provides different scanning modes:
 * scan to data, i.ekeep on scanning until a channel with DAB data is detected;
 *  single scan, i.e. a scan from channel 5A to 13F,
 *  and scan continuously.

![7.2](/res/read_me/scan-widget.png?raw=true)

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

In recent versions, a copy of that database is included the precompiled
versions which is automatically loaded if no database can be found in the user's home directory.

A **fresh** copy of that database can be installed in the user's home directory
by a small utility, a **db-loader**, precompiled for Windows and Linux-x64.
The **db-loader** installs the database, file ".txdata.ti" in the user's home directory, that is where Qt-DAB expects a database - with that name - to be found.
The **configuration and control** window contains a button to load the
database from the user's home directory into the program.

![7.2](/res/read_me/db-loader.png?raw=true)

Alternatively (and for other computing environments) one can **download** a
 reasonably up to date copy of the database from the directory **helpers**
in the Qt-DAB repository.
Unpack the zipped file "tiiFile.zip" and name it ".txdata.tii" in the
home directory.

![7.2](/res/read_me/QTmap.png?raw=true)

Qt-DAB has as said - on the main window -  a button labeled **http**,
when touched, a webserver is started  with on it a map centered around the
home position, that - when running - shows the position(s) of the
transmitter(s) received. 
(Note that Qt-DAB is set to have a **default** home location -
somewhere in Amsterdam - that - together with  default database - allows the
software to handle TII data and show the result of decoding on a map).
It is advised to update the user's home location, the **configuration
and control** window contains entries to specify a latitude and a longitude.

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

Using user specified bands
=================================================================

DAB transmissions are now all restricted to Mode I and band III.
Qt-DAB provides (Unix/Linux builds only) the opportunity to specify
one's own band. Specify in a file a list of channels, e.g.
Note however, that in Qt_DAB 7.3.0 only Mode I is supported.

	one	227360
	two	220352
	three	1294000
	four	252650

and pass the file on program start-up with the `-A` command line switch. The channel name is just any identifier, the channel frequency is given in kHz. Your SDR device obviously has to support the frequencies for these channels.

Building an executable
=================================================================

A detailed description on building the executable is in a separate
document,
 - building-dab.txt

to be found in the "docs" directory.

Copyright and acknowledgements
=================================================================

	Copyright (C)  2016 .. 2026
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Computing

	Copyright of libraries used - Qt, fftw, portaudio,
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
