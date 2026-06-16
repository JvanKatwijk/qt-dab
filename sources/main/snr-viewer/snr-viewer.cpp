#
/*
 *    Copyright (C)  2014 .. 2024
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	<QFileDialog>
#include	<QMessageBox>
#include	"snr-viewer.h"
#include	<QSettings>
#include	<string.h>
#include	"basic-scope.h"
#include	"position-handler.h"
#include	"settings-handler.h"

	snrViewer::snrViewer	(RadioInterface	*mr,
	                         QSettings	*s):
	                             myFrame (nullptr) {
	this		-> myRadioInterface	= mr;
	this		-> dabSettings		= s;

	setupUi (&myFrame);
	setPositionAndSize (s, &myFrame, "snrViewer");
	theDisplay	= new  basicScope (snrPlot, dabSettings,
	                                             1024, "snrViewer");
	plotLength	= 1024;
	Y_buffer. resize (plotLength);
	plotHeight	= value_i (dabSettings, "snrViewer", "snrHeight", 15);
	delayCount	= value_i (dabSettings, "snrViewer", "snrDelay", 5);
	snrSlider		-> setValue (plotHeight);
	snrCompressionSelector	-> setValue (delayCount);
	snrDumpFile. store (nullptr);
	connect (snrDumpButton, &QPushButton::clicked,
	         this, &snrViewer::handle_snrDumpButton);
	connect (snrSlider, &QSlider::valueChanged,
	         this, &snrViewer::set_snrHeight);
	connect (snrCompressionSelector,
	                qOverload<int>(&QSpinBox::valueChanged),
	         this, &snrViewer::set_snrDelay);
}

	snrViewer::~snrViewer () {
	stopDumping 	();
	if (!myFrame. isHidden ())
	   storeWidgetPosition (dabSettings, &myFrame, "snrViewer");
	delete theDisplay;
}

void	snrViewer::show () {
	myFrame. show ();
}

void	snrViewer::hide	() {
	myFrame. hide ();
	stopDumping ();
}

bool	snrViewer::isHidden () {
	return myFrame. isHidden ();
}

void	snrViewer::add_snr	(float snr) {
static float delayBuffer [3];
float	sum	= 0;
	delayBuffer [delayBufferP] = snr;
	delayBufferP = (delayBufferP + 1) % delayCount;
	if (delayBufferP == 0) {
	   for (int i = 0; i < delayCount; i ++)
	      sum += delayBuffer [i];
	   sum /= delayCount;
	   addtoView (sum);
	}
}

#define	VIEWBUFFER_SIZE 5
void	snrViewer::addtoView (float v) {
static float displayBuffer [VIEWBUFFER_SIZE];
static int displayPointer = 0;

	displayBuffer [displayPointer] = v;
	displayPointer = (displayPointer + 1) % VIEWBUFFER_SIZE;
	if (displayPointer == 0) {
	   memmove (&(Y_buffer. data () [VIEWBUFFER_SIZE]),
	                   &(Y_buffer. data () [0]),
	                   (plotLength - VIEWBUFFER_SIZE) * sizeof (double));
	   for (int i = 0; i < VIEWBUFFER_SIZE; i ++)
	      Y_buffer [i] = displayBuffer [i];
	   if (snrDumpFile. load () != nullptr)
	      fwrite (displayBuffer, sizeof (float), 
	                             VIEWBUFFER_SIZE, snrDumpFile. load ());
	}
}

void	snrViewer::add_snr	(float sig, float noise) {
float snr = 20 * log10 ((sig + 0.001) / (noise + 0.001));
	memmove (&(Y_buffer. data () [1]), &(Y_buffer. data () [0]),
	                               (plotLength - 1) * sizeof (double));
	Y_buffer [0]	= snr;
	if (snrDumpFile. load () != nullptr)
	   fwrite (&snr, sizeof (float), 1, snrDumpFile. load ());
}

void	snrViewer::show_snr () {
	theDisplay	-> showSpectrum (Y_buffer. data (), 1024,
	                                 0, 120,
	                                 0, plotHeight);
}

float	snrViewer::get_db (float x) {
	return 20 * log10 ((x + 1) / (float)(4 * 512));
}

void	snrViewer::handle_snrDumpButton () {
	if (snrDumpFile. load () != nullptr) {
	   stopDumping ();
	   return;
	}
	startDumping ();
}

void	snrViewer::stopDumping () {
	if (snrDumpFile. load () != nullptr) {
	   fclose (snrDumpFile. load ());
	   snrDumpFile. store (nullptr);
	   snrDumpButton	-> setText ("dump");
	}
}

void	snrViewer::startDumping () {
	QString fileName = QFileDialog::
	                     getSaveFileName (&myFrame,
                                              tr ("Open file ..."),
                                              QDir::homePath(),
                                              tr ("snr (*.snr)"),
	                                      Q_NULLPTR,
	                                      QFileDialog::DontUseNativeDialog);

	if (fileName == QString (""))  // canceled
	   return;

	fileName = QDir::toNativeSeparators (fileName);

	FILE *file = fopen (fileName. toUtf8 (). data (), "w+b");
	if (file == nullptr) {
	   QMessageBox::warning (&myFrame, tr ("Warning"),
	                            tr ("could not open file"));
	   return;
	}
	snrDumpFile. store (file);
	snrDumpButton -> setText ("dumping");
}

void	snrViewer::set_snrDelay	(int d) {
	if ((0 <= d) && (d <= 10))
	   delayCount = d;
	else
	   delayCount = 5;
	delayBufferP	= 0;
	store (dabSettings, "snrViewer", "snrDelay", d);
}

void	snrViewer::set_snrHeight	(int n) {
	plotHeight	= n;
	store (dabSettings, "snrViewer", "snrHeight", n);
}

