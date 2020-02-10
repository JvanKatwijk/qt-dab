#
/*
 *    Copyright (C) 2014 .. 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 2 of the License.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	<QThread>
#include	<QSettings>
#include	<QHBoxLayout>
#include	<QLabel>
#include	<QFileDialog>
#include	"sdrplay-handler-v3.h"
#include	"control-queue.h"
#include	"sdrplay-controller.h"
#include	"xml-filewriter.h"


	sdrplayHandler_v3::sdrplayHandler_v3  (QSettings *s,
	                                       QString &recorderVersion) {
	sdrplaySettings			= s;
	this	-> recorderVersion	= recorderVersion;
	myFrame				= new QFrame (nullptr);
	setupUi (this -> myFrame);
	this	-> myFrame	-> show	();
	antennaSelector		-> hide	();
	tunerSelector		-> hide	();
	_I_Buffer		= new RingBuffer<
	                              std::complex<int16_t>>(8 *1024 * 1024);
	theQueue		= new controlQueue ();
	nrBits			= 12;	// default

	xmlDumper		= nullptr;
	dumping. store	(false);
//	See if there are settings from previous incarnations
//	and config stuff

	theQueue	-> add (STOP_REQUEST);
	sdrplaySettings		-> beginGroup ("sdrplaySettings");
	GRdBSelector 		-> setValue (
	            sdrplaySettings -> value ("sdrplay-ifgrdb", 20). toInt());
	theQueue -> add (GRDB_REQUEST, GRdBSelector -> value ());

	lnaGainSetting		-> setValue (
	            sdrplaySettings -> value ("sdrplay-lnastate", 4). toInt());
	theQueue -> add (LNA_REQUEST, lnaGainSetting -> value ());

	ppmControl		-> setValue (
	            sdrplaySettings -> value ("sdrplay-ppm", 0). toInt());
	theQueue -> add (PPM_REQUEST, ppmControl -> value ());

	agcMode		=
	       sdrplaySettings -> value ("sdrplay-agcMode", 0). toInt() != 0;
	if (agcMode) 
	   theQueue -> add (AGC_REQUEST, true, GRdBSelector -> value ());
	else
	   theQueue -> add (AGC_REQUEST, false);
	if (agcMode) {
	   agcControl -> setChecked (true);
	   GRdBSelector         -> hide ();
	   gainsliderLabel      -> hide ();
	}
	sdrplaySettings	-> endGroup	();

//	and be prepared for future changes in the settings
	connect (GRdBSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ifgainReduction (int)));
	connect (lnaGainSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (set_lnagainReduction (int)));
	connect (agcControl, SIGNAL (stateChanged (int)),
	         this, SLOT (set_agcControl (int)));
	connect (ppmControl, SIGNAL (valueChanged (int)),
	         this, SLOT (set_ppmControl (int)));
	connect (antennaSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (set_antennaSelect (const QString &)));
	connect (dumpButton, SIGNAL (clicked ()),
                 this, SLOT (set_xmlDump ()));

//
	theController	= new sdrplayController (this, _I_Buffer, theQueue);
	denominator	= 2048;
	usleep (1000);
}

	sdrplayHandler_v3::~sdrplayHandler_v3 () {
	delete theController;
//
//	thread should be stopped by now
	sdrplaySettings	-> beginGroup ("sdrplaySettings");
	sdrplaySettings -> setValue ("sdrplay-ppm",
	                                           ppmControl -> value ());
	sdrplaySettings -> setValue ("sdrplay-ifgrdb",
	                                           GRdBSelector -> value ());
	sdrplaySettings -> setValue ("sdrplay-lnastate",
	                                           lnaGainSetting -> value ());
	sdrplaySettings	-> setValue ("sdrplay-agcMode",
	                                  agcControl -> isChecked() ? 1 : 0);
	sdrplaySettings	-> endGroup ();
	sdrplaySettings	-> sync();

	myFrame	-> hide ();
	delete	myFrame;
	delete _I_Buffer;
	delete	theQueue;
}

void	sdrplayHandler_v3::show_TotalGain (int g) {
	totalGainDisplay	-> display (g);
}

void	sdrplayHandler_v3::show_DeviceData (const QString &s,
	                                   int hwVersion, float apiVersion) {
	serialNumber	-> setText (s);
	this		-> hwVersion	= hwVersion;
	api_version	-> display (apiVersion);
}

void	sdrplayHandler_v3::show_Error	(int e) {}

void	sdrplayHandler_v3::show_runFlag	(bool b) {
	running. store (b);
	if (!b)
	   serialNumber	-> setText ("something serious went wrong");
}

void	sdrplayHandler_v3::show_lnaGain	(int value) {
	lnaGRdBDisplay	-> display (value);
}

void	sdrplayHandler_v3::set_lnaRange (int low, int high) {
	lnaGainSetting	-> setRange (low, high);
}

void	sdrplayHandler_v3::show_deviceLabel (const QString &s, int nrBits) {
	deviceLabel	-> setText (s);
	this	-> nrBits	= nrBits;
}

void	sdrplayHandler_v3::show_antennaSelector (bool b) {
	if (b)
	   antennaSelector		-> show	();
	else
	   antennaSelector		-> hide	();
}

void	sdrplayHandler_v3::show_tunerSelector	(bool b) {
	if (b)
	   tunerSelector	-> show	();
	else
	   tunerSelector	-> hide	();
}


int32_t	sdrplayHandler_v3::defaultFrequency	() {
	return Mhz (220);
}

int32_t	sdrplayHandler_v3::getVFOFrequency() {
	return vfoFrequency;
}

void	sdrplayHandler_v3::set_ifgainReduction	(int gRdB) {
	theQueue	->  add (GRDB_REQUEST, gRdB);
}

void	sdrplayHandler_v3::set_lnagainReduction (int lnaState) {
	theQueue	-> add (LNA_REQUEST, lnaState);
}

void	sdrplayHandler_v3::set_agcControl (int dummy) {
bool agcMode	= agcControl -> isChecked();

	(void)dummy;
	if (agcMode) 
	   theQueue	-> add (AGC_REQUEST, true, GRdBSelector -> value ());
	else
	   theQueue	-> add (AGC_REQUEST, false);

	if (!agcMode) {
	   GRdBSelector		-> show();
	   gainsliderLabel	-> show();	// old name actually
	}
	else {
	   GRdBSelector		-> hide();
	   gainsliderLabel	-> hide();
	}
}

void	sdrplayHandler_v3::set_ppmControl (int ppm) {
	theQueue	-> add (PPM_REQUEST, ppm);
}

void	sdrplayHandler_v3::set_antennaSelect	(const QString &s) {
        if (s == "Antenna A")
	   theQueue	-> add (ANTENNASELECT_REQUEST, 'A');
        else
	   theQueue	-> add (ANTENNASELECT_REQUEST, 'B');
}

bool	sdrplayHandler_v3::restartReader	(int32_t freq) {
	if (!theController -> is_threadRunning () ||
	    theController -> is_receiverRunning ())
	   return true;

	theQueue	-> add (RESTART_REQUEST, freq);
	vfoFrequency	= freq;
	return true;
}

void	sdrplayHandler_v3::stopReader	() {

	close_xmlDump ();
	if (!theController -> is_threadRunning () ||
	    !theController -> is_receiverRunning ())
	   return;
	theQueue	-> add (STOP_REQUEST);
}

//	The brave old getSamples. For the sdrplay, we get
//	size still in I/Q pairs
//
//	In a next version we just share the _I_Buffer with the
//	dabProcessor
int32_t	sdrplayHandler_v3::getSamples (std::complex<float> *V, int32_t size) { 
std::complex<int16_t> temp [size];
int	i;

	int amount      = _I_Buffer     -> getDataFromBuffer (temp, size);
        for (i = 0; i < amount; i ++)
           V [i] = std::complex<float> (real (temp [i]) / (float) denominator,
                                        imag (temp [i]) / (float) denominator);
        if (dumping. load ())
           xmlWriter -> add (temp, amount);
        return amount;

	return _I_Buffer	-> getDataFromBuffer (V, amount);
}

int32_t	sdrplayHandler_v3::Samples	() {
	return _I_Buffer	-> GetRingBufferReadAvailable();
}

void	sdrplayHandler_v3::resetBuffer	() {
	_I_Buffer	-> FlushRingBuffer();
}

int16_t	sdrplayHandler_v3::bitDepth	() {
	return nrBits;
}

void	sdrplayHandler_v3::set_xmlDump () {
	if (xmlDumper == nullptr) {
	  if (setup_xmlDump ())
	      dumpButton	-> setText ("writing");
	}
	else {
	   close_xmlDump ();
	   dumpButton	-> setText ("Dump");
	}
}

bool	sdrplayHandler_v3::setup_xmlDump () {
	QString fileName = QFileDialog::getSaveFileName (nullptr,
	                                         tr ("Save file ..."),
	                                         QDir::homePath(),
	                                         tr ("Xml (*.uff)"));
        fileName        = QDir::toNativeSeparators (fileName);
        xmlDumper	= fopen (fileName. toUtf8(). data(), "w");
	if (xmlDumper == nullptr)
	   return false;
	
	xmlWriter	= new xml_fileWriter (xmlDumper,
	                                      nrBits,
	                                      "int16",
	                                      2048000,
	                                      vfoFrequency,
	                                      "SDRplay",
	                                      "????",
	                                      recorderVersion);
	dumping. store (true);
	return true;
}

void	sdrplayHandler_v3::close_xmlDump () {
	if (xmlDumper == nullptr)	// this can happen !!
	   return;
	dumping. store (false);
	usleep (1000);
	xmlWriter	-> computeHeader ();
	delete xmlWriter;
	fclose (xmlDumper);
	xmlDumper	= nullptr;
}

