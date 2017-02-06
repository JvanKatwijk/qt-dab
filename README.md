				QT-DAB


=====================
QT-DAB is the result of merging the dab-rpi and the sdr-j-dab programs of the
author. It became more and more complex to maintain different versions: modifications made in one version, did not always end up in the other versions so the
versions started to diverge.

Since furthermore a separate "command line only" version was (is being) developed (a version not using Qt at all), while much of the sources is also used in the development of a version handling ETI files, there was a real need to
reorganize.

I decided to merge the dab-rpi and sdr-j-dab version and rename
the result - to distinguish from the Qt-free version -  Qt-DAB.

The Qt-free version, the "command line only" version,  is named "dab-cmdline"
and has its own repository.

The software - both the QT-DAB and the dab-cmdline version - supports decoding of terrestrial DAB and DAB+ reception with as input the  samplestream from either an AIRSPY, a SDRplay, a dabstick (rtl_sdr), a rtl_tcp server.
The QT-DAB version also supports input from a (prerecorded) file (and
obviously provides the opportunity of dumping the input into a file).

Since the QT-DAB version has to run on a headless RPI 2, using the home WiFi,
the spectrum display, showing the spectrum and the constellation over the WiFi,
can be left out of the configuration. 

Again, as the QT-DAB version has to run on a headless RPI 2,
an option is included to configure the sound output to deliver its
samples through a TCP connection.

=============================================================================
Configuring using the qt-dab.pro files
--------------------------------------

Options in the configuration are
a. select or unselect devices,
b. select the output: soundcard or tcp connection
c. select or unselect basic MOT data handling,
d. select or unselect basic MSC data handling.

Adding or removing from the configuration is in all cases by commenting or uncommenting a line in the configuration file.

Comment the lines out by prefixing the line with a #
in the qt-pro file (section "unix") for the device(s)
you want to exclude in the configuration.

CONFIG          += dabstick

CONFIG          += sdrplay-exp

CONFIG          += rtl_tcp

CONFIG          += airspy

Input from prerecorded files is always part of the configuration.

Having the spectrum and the constellation shown, uncomment

CONFIG          += spectrum  

For selecting the output to be sent to a RCP port, uncomment

CONFIG         += tcp-streamer         # use for remote listening

For showing some information on the selected program uncomment

DEFINES         += TECHNICAL_DATA

For basic MSC data handling, i.e. pad handling etc, uncomment

DEFINES         += MSC_DATA__           # use at your own risk

The sourcetree contains a directory "sound-client", that contains
sources to generate a simple "listener" for remote listening.

========================================================================
Configuring using CMake
-----------------------

The CMakeLists.txt file has as defaults all devices and the spectrum switched
off.
You can select a device (or more devices) without altering the CMakeLists.txr
file, but by passing on definitions to the command line.

An example

	cmake .. -DDABSTICK=ON -DRTLTCP=ON -DSPECTRUM=ON
	
will generate a makefile with both support for the dabstick and
for the remote dabstick (using the rtl_tcp connection) and for
the spectrum in the configuration.

Devices that can be selected this way are, next to the dabstick and
rtl_tcp, the sdrplay and the airspy.

For other options, see the CMakeLists.txt file.

Note that CMake expects Qt5 to be installed.

=============================================================================
Scanning
--------

The qt-dab software provides a "scanning" facility, pushing the "scan"
button will cause the software to scan the subsequent channels in the
selected band until a channel is encountered where a DAB signal is detected.

===========================================================================
RPI
---

The  qt-dab software runs pretty well on my RPI-2. The average
load on the 4 cores is somewhat over 60 percent.
However, note that the arch system, the one I am running,
provides the gcc 6.XX compilers rather than the GCC 4.9 on Jessie. 
On Jessie the load is higher, up to 75 or 80 percent.

One remark: getting "sound" is not always easy. Be certain that you have
installed the alsa-utils, and that you are - as non-root user - able
to see devices with aplay -L

In arch, it was essential to add the username to the group "audio".

============================================================================
Qt
---

The software uses the Qt library and - for the spectrum and the constellation
diagram - the qwt library.

The CMakeLists.txt assumes Qt5, if you want to use Qt4, and you want
to have the spectrum in the configuration, be aware of the binding 
of the qwt library (i.e. Qt4 and a qwt that uses Qt5 does not work well)

==========================================================================

Ubuntu Linux
---

For generating an executable under Ubuntu, you can put the following
commands into a script. 

1. Fetch the required components
   #!/bin/bash
   sudo apt-get update
   sudo apt-get install qt4-qmake build-essential g++
   sudo apt-get install libsndfile1-dev qt4-default libfftw3-dev portaudio19-dev  libfaad-dev zlib1g-dev rtl-sdr libusb-1.0-0-dev mesa-common-dev libgl1-mesa-dev libqt4-opengl-dev libsamplerate-dev libqwt-dev
   cd
2.a.  Assuming you want to use a dabstick as device,
   fetch a version of the library for the dabstick
   # http://www.sm5bsz.com/linuxdsp/hware/rtlsdr/rtlsdr.htm
   wget http://sm5bsz.com/linuxdsp/hware/rtlsdr/rtl-sdr-linrad4.tbz
   tar xvfj rtl-sdr-linrad4.tbz 
   cd rtl-sdr-linrad4
   sudo autoconf
   sudo autoreconf -i
   ./configure --enable-driver-detach
   make
   sudo make install
   sudo ldconfig
   cd
2.b. Assuming you want to use an Airspy as device,
   fetch a version of the library for the airspy
   sudo apt-get install build-essential cmake libusb-1.0-0-dev pkg-config
   wget https://github.com/airspy/host/archive/master.zip
   unzip master.zip
   cd host-master
   mkdir build
   cd build
   cmake ../ -DINSTALL_UDEV_RULES=ON
   make
   sudo make install
   sudo ldconfig
##Clean CMake temporary files/dirs:
   cd host-master/build
   rm -rf *

3. Get a copy of the dab-rpi sources and use qmake
   git clone https://github.com/JvanKatwijk/qt-dab.git
   cd qt-dab
>>>Edit the qt-dab.pro file for configuring the supported devices
>>> and other options
   qmake qt-dab.pro
   make

============================================================================


	Copyright (C)  2016, 2017
	Jan van Katwijk (J.vanKatwijk@gmail.com)
	Lazy Chair Programming

	The qt-dab software is made available under the GPL-2.0.
	SDR-J is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.


