#
/*
 *    Copyright (C) 2020
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
 *    along with Qt-DAB. if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	<QThread>
#include	<QSettings>
#include	<QMessageBox>
#include	<QTime>
#include	<QDate>
#include	<QLabel>
#include	<QDebug>
#include	"colibri-handler.h"

	colibriHandler::colibriHandler  (QSettings *s,
	                                 bool	marzano):
	                                  _I_Buffer (4 * 1024 * 1024),
	                                  myFrame (nullptr) {
	colibriSettings		= s;
	setupUi (&myFrame);
	QString libName = "libcolibrinano_lib.so";
	if (!m_loader. load (libName. toLatin1 () .data ())) {
           QMessageBox::critical (nullptr, "colibri",
	                          tr("Failed to load colibrinano_lib.so"));
	   throw (21);
	}

	fprintf (stderr, "library is loaded\n");
	m_loader. initialize ();

	fprintf (stderr, "... and initialized\n");
	uint32_t t_devices	= m_loader. devices ();
	if (t_devices == 0) {
	   QMessageBox::critical (nullptr, "colibri",
	                          tr ("No device available\n"));
	   throw (22);
	}

	fprintf (stderr, "we found %d device(s)\n", t_devices);
//	set some defaults
	if (!m_loader.open (&m_deskriptor, 0)) {
	   QMessageBox::warning (nullptr, "colibri",
	                         tr("Failed to open ColibriNANO!"));
	   throw (23);
        }

	fprintf (stderr, "and opening device 0 was ok\n");
        m_loader.setFrequency (m_deskriptor, 220000000);

	this		-> lastFrequency	= 220000000;
	fprintf (stderr, "set on %d\n", lastFrequency);
	colibriSettings -> beginGroup ("colibriSettings");
	int gainSetting = colibriSettings -> value ("colibri-gain", 20). toInt ();
	gainSelector	-> setValue (gainSetting);
	colibriSettings -> endGroup ();
        m_loader.setPream (m_deskriptor, gainSelector ->value () * 0.5 + -31.5);
	actualGain	-> display (gainSelector -> value () * 0.5 + -31.5);

//	and be prepared for future changes in the settings
	connect (gainSelector, SIGNAL (valueChanged (int)),
	         this, SLOT (set_gainControl (int)));
	connect (iqSwitchButton, SIGNAL (clicked ()),
	         this, SLOT (handle_iqSwitcher ()));
	if (marzano) {
	   selectedRate	= 1920000;
	}
	else {
	   selectedRate	= 2560000;
	}
	rateLabel	-> setText (QString::number (selectedRate));
//	The sizes of the mapTables follow from the input and output rate
//	(selectedRate / 1000) vs (2048000 / 1000)
//	so we end up with buffers with 1 msec content
	convBufferSize		= selectedRate / 1000;
	for (int i = 0; i < 2048; i ++) {
	   float inVal	= float (selectedRate / 1000);
	   mapTable_int [i]	=  int (floor (i * (inVal / 2048.0)));
	   mapTable_float [i]	= i * (inVal / 2048.0) - mapTable_int [i];
	}
	convIndex	= 0;
	convBuffer. resize (convBufferSize + 1);

	iqSwitcher	= false;
	switchLabel	-> setText ("I/Q");
	nameLabel	-> setText (deviceName ());
	running. store (false);
}

	colibriHandler::~colibriHandler () {
	myFrame. hide ();
	stopReader();
	colibriSettings	-> beginGroup ("colibriSettings");
	colibriSettings	-> setValue ("colibri-gain", 
	                              gainSelector -> value ());
	colibriSettings	-> endGroup ();
}
//

void	colibriHandler::setVFOFrequency	(int32_t newFrequency) {
        m_loader. setFrequency (m_deskriptor, newFrequency);
	this	-> lastFrequency	= newFrequency;
}

int32_t	colibriHandler::getVFOFrequency () {
	return this -> lastFrequency;
}

void	colibriHandler::set_gainControl	(int newGain) {
float	gainValue	= -31.5 + newGain * 0.5;
	if (gainValue <= 6) {
           m_loader.setPream (m_deskriptor, gainValue);
	   actualGain	-> display (gainValue);
	}
}

void	colibriHandler::handle_iqSwitcher	()  {
	iqSwitcher	= !iqSwitcher;
	if (iqSwitcher)
	   switchLabel	-> setText ("Q/I");
	else
	   switchLabel -> setText ("I/Q");
}

static
bool	the_callBackRx (std::complex<float> *buffer, uint32_t len,
	                               bool overload, void *ctx) {
colibriHandler *p = static_cast<colibriHandler *>(ctx);
std::complex<float> temp [2048];

	(void)overload;
	for (int i = 0; i < len; i ++) {
	   p -> convBuffer [p -> convIndex] = buffer [i];
	   p -> convIndex ++;
	   if (p -> convIndex > p -> convBufferSize) {
	      for (int j = 0; j < 2048; j ++) {
	         int16_t  inpBase	= p -> mapTable_int [j];
	         float	inpRatio	= p ->  mapTable_float [j];
                 temp [j]  = cmul (p -> convBuffer [inpBase + 1], inpRatio) +
                             cmul (p -> convBuffer [inpBase], 1 - inpRatio);
              }

              p -> _I_Buffer. putDataIntoBuffer (temp, 2048);
	      p -> convBuffer [0] = p -> convBuffer [p -> convBufferSize];
	      p -> convIndex = 1;
	   }
	}
	return true;
}

bool	colibriHandler::restartReader	(int32_t newFrequency) {
	if (running. load ())
	   return true;		// should not happen

	fprintf (stderr, "restarting at %d\n", newFrequency);
        m_loader. setFrequency (m_deskriptor, newFrequency);
	this	-> lastFrequency	= newFrequency;
	m_loader.start (m_deskriptor,
	                selectedRate == 2560000 ? (SampleRateIndex)Sr_2560kHz:
	                                        (SampleRateIndex)Sr_1920kHz,
                        the_callBackRx,
	                this);
	running. store (true);
	return true;
}

void	colibriHandler::stopReader() {
	if (!running. load())
	   return;
	fprintf (stderr, "stopreader (%d)\n", lastFrequency);
	m_loader. stop (m_deskriptor);
	running. store (false);
}

int32_t	colibriHandler::getSamples (std::complex<float> *V, int32_t size) { 
	if (iqSwitcher) {
	   std::complex<float> xx [size];
	   _I_Buffer. getDataFromBuffer (xx, size);
	   for (int i = 0; i < size; i ++)
	      V[i] = std::complex<float> (imag (xx [i]), real (xx [i]));
	   return size;
	}
	else
	   return _I_Buffer. getDataFromBuffer (V, size);
}

int32_t	colibriHandler::Samples () {
	return _I_Buffer. GetRingBufferReadAvailable ();
}

void	colibriHandler::resetBuffer() {
	_I_Buffer. FlushRingBuffer();
}

int16_t	colibriHandler::bitDepth () {
	return 16;
}

QString	colibriHandler::deviceName	() {

std::string s = m_loader. information ();
	return QString (s. c_str ());
}

int colibriHandler::sampleRate (int index) {
    switch (index) {
        case Sr_48kHz: return 48000;
        case Sr_96kHz: return 96000;
        case Sr_192kHz: return 192000;
        case Sr_384kHz: return 384000;
        case Sr_768kHz: return 768000;
        case Sr_1536kHz: return 1536000;
        case Sr_1920kHz: return 1920000;
        case Sr_2560kHz: return 2560000;
        case Sr_3072kHz: return 3072000;

        default: break;
    }

    return 48000;
}

void    colibriHandler::show    () {
        myFrame. show ();
}

void    colibriHandler::hide    () {
        myFrame. hide ();
}

bool    colibriHandler::isHidden        () {
        return myFrame. isHidden ();
}


