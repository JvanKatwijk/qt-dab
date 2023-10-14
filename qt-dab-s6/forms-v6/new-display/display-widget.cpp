#
/*
 *    Copyright (C)  2016 .. 2023
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

#include	<QSettings>
#include	"display-widget.h"
#include	"spectrum-scope.h"
#include	"null-scope.h"
#include	"correlation-scope.h"
#include	"channel-scope.h"
#include	"waterfall-scope.h"
#include	"iqdisplay.h"

	displayWidget::displayWidget	(RadioInterface	*mr,
	                                 QSettings	*dabSettings):
	                                         myFrame (nullptr),
	                                         theFFT (4 * 512, false) {

	this	-> dabSettings		= dabSettings;

	dabSettings	-> beginGroup ("displayWidget");
	int x   = dabSettings -> value ("position-x", 100). toInt ();
        int y   = dabSettings -> value ("position-y", 100). toInt ();
	int w	= dabSettings -> value ("width", 150). toInt ();
	int h	= dabSettings -> value ("height", 120). toInt ();
        dabSettings -> endGroup ();
        setupUi (&myFrame);
	myFrame. resize (QSize (w, h));
        myFrame. move (QPoint (x, y));
//
#ifndef	__ESTIMATOR_
	tabWidget -> removeTab (4);
#endif
	myFrame. hide ();

//	the "workers"
	mySpectrumScope		= new spectrumScope	(spectrumDisplay,
	                                                512, dabSettings);
	spectrumAmplitude	-> setValue (50);
	myWaterfallScope	= new waterfallScope	(waterfallDisplay,
	                                                512, 50);
	myNullScope		= new nullScope		(nullDisplay,
	                                                512, dabSettings);
	myCorrelationScope	= new correlationScope (correlationDisplay,
	                                                256, dabSettings);
	correlationLength       -> setValue (500);

	myTII_Scope		= new spectrumScope	(tiiDisplay,
	                                                512, dabSettings);
	myIQDisplay		= new IQDisplay		(iqDisplay, 512);

#ifdef	__ESTIMATOR_
	myChannelScope		= new channelScope	(channelPlot,
	                                                 128, dabSettings);
#endif

	dabSettings		-> beginGroup ("displayWidget");
        currentTab		= dabSettings -> value ("tabSettings", 0). toInt ();
        dabSettings		-> endGroup ();
        tabWidget		-> setCurrentIndex (currentTab);
	correlationsVector 	-> 
	          setStyleSheet (
	                 "QLabel {background-color : green; color: white}");


	connect (tabWidget, SIGNAL (currentChanged (int)),
                 this, SLOT (switch_tab (int)));
	connect (myIQDisplay, SIGNAL (rightMouseClick ()),
	         this, SLOT (rightMouseClick ()));
}

	displayWidget::~displayWidget () {
	dabSettings	-> beginGroup ("displayWidget");
        dabSettings	-> setValue ("position-x", myFrame. pos (). x ());
        dabSettings	-> setValue ("position-y", myFrame. pos (). y ());

	QSize size	= myFrame. size ();
	dabSettings	-> setValue ("width", size. width ());
	dabSettings	-> setValue ("height", size. height ());
	dabSettings	-> endGroup ();
	myFrame. hide	();
	delete		mySpectrumScope;
	delete		myWaterfallScope;
	delete		myNullScope;
	delete		myCorrelationScope;
	delete		myTII_Scope;
#ifdef	__ESTIMATOR_
	delete		myChannelScope;
#endif
	delete		myIQDisplay;
}

void	displayWidget::switch_tab	(int t) {
	currentTab	= t;
	dabSettings	-> beginGroup ("displayWidget");
	dabSettings	-> setValue ("tabSettings", t);
	dabSettings	-> endGroup ();
	myWaterfallScope	-> cleanUp ();
}

int	displayWidget::get_tab		() {
	return currentTab == 0 ? SHOW_SPECTRUM :
	       currentTab == 1 ? SHOW_CORRELATION :
	       currentTab == 2 ? SHOW_NULL : 
	       currentTab == 3 ? SHOW_TII : SHOW_CHANNEL;
}

///////////////////////////////////////////////////////////////////////
//	entries
///////////////////////////////////////////////////////////////////////
//
//	for "spectrum" we get a segment of 2048 timedomain samples
//	we take the fft and average a little
void	displayWidget::showSpectrum	(std::vector<Complex> &v, int freq) {
int	l	= v. size ();
double	X_axis [512];
double  Y_value [512];

static double avg [4 * 512];
	if (currentTab != SHOW_SPECTRUM)
	   return;

	theFFT. fft (v);
	for (int i = 0; i < v. size () / 2; i ++) {
	   avg [i] = 0.5 * avg [i] + 0.5 * abs (v [l / 2 + i]);
	   avg [l / 2 + i] = 0.5 * avg [l / 2 + i] + 0.5 * abs (v [i]);
	}

	for (int i = 0; i < 512; i ++) {
	   X_axis [i] = (int)((freq - 1536000 / 2 + i * 1536000.0 / 512) / 1000);
	   Y_value [i] = 0;
	   for (int j = 0; j < 4; j ++) 
	      Y_value [i] +=  avg [4 * i + j];
	   Y_value [i]	=  get_db (Y_value [i] / 4);
	}

	mySpectrumScope		-> display (X_axis, Y_value, freq, 
	                                    spectrumAmplitude -> value ());
	for (int i = 0; i < 512; i ++)
	   Y_value [i] = (Y_value [i] - get_db (0)) / 6;
	myWaterfallScope	-> display (X_axis, Y_value, 
	                                    waterfallSlider -> value (),
	                                    freq / 1000);
}
//
//
//	for "corr" we get a segment of 1024 float values,
//	with as second parameter a list of indices with maximum values
void	displayWidget::showCorrelation	(std::vector<float> &v,
	                                 QVector<int> &ww) {
	if (currentTab != SHOW_CORRELATION)
	   return;
	myCorrelationScope	-> display (v, correlationLength -> value ());
	if (ww. size () > 0) {
	   int baseOffset = ww [0];
	   QString t = "Matches ";
	   for (int i = 0; i < ww. size (); i ++) {
	      t = t + " " + QString::number (ww [i]);
	      if (i >= 1) {
	         int lO = ww [i] - ww [0];
	         int d = (300000 / 2048 * lO) / 1000;
	         t = t + " (" + QString::number (d) + "km)";
	      }
	   }
	   correlationsVector -> setText (t);
	}

	if (v. size () < 512)
	   return;
	double X_axis [512];
	double Y_value [512];
	float	MMax	= 0;
	for (int i = v. size () / 2 - 256; i < v. size () / 2 + 256; i ++) {
	   X_axis [i - v. size () / 2 + 256] = i;
	   Y_value [i - v. size () / 2 + 256] = v [i];
	   if (v [i] > MMax)
	      MMax = v [i];
	}
	for (int i = 0; i < 512; i ++)
	   Y_value [i] *= 50.0 / MMax;
	myWaterfallScope -> display (X_axis, Y_value, 
	                             waterfallSlider -> value (),
	                             v. size () / 2);
}
//	for "null" we get a segment of 1024 timedomain samples
//	(the amplitudes!)
//	that can be displayed directly
void	displayWidget::show_null	(Complex  *v, int amount) {
	if  (currentTab != SHOW_NULL)
	   return;
	if (amount < 1024)
	   return;
	for (int i = 0; i < 512; i ++)
	   v [i] = (v [2 * i] + v [2 * i + 1]) / 2.0f;
	myNullScope		-> display (v, amount);
	double X_axis [512];
	double Y_value [512];
	float	MMax	= 0;
	for (int i = 0; i < 512; i ++) {
	   X_axis [i] = 256 + i;
	   Y_value [i] = abs (v [i]);
	   if (Y_value [i] > MMax)
	      MMax = Y_value [i];
	}
	for (int i = 0; i < 512; i ++)
	   Y_value [i] *= 50.0 / MMax;
	myWaterfallScope	-> display (X_axis, Y_value, 
	                                    waterfallSlider -> value (), 256);
}
//
//	for "tii" we get a segment of 2048 time domain samples,
//	we take an FFT, do some averaging and display
void	displayWidget::show_tii		(std::vector<Complex> v, int freq) {
int	l	= v. size ();
double	X_axis [512];
double  Y_value [512];

static double avg [4 * 512];
	if (currentTab != SHOW_TII)
	   return;

	theFFT. fft (v);
	for (int i = 0; i < v. size () / 2; i ++) {
	   avg [i] = 0.5 * avg [i] + 0.5 * abs (v [l / 2 + i]);
	   avg [l / 2 + i] = 0.5 * avg [l / 2 + i] + 0.5 * abs (v [i]);
	}

	for (int i = 0; i < 512; i ++) {
	   X_axis [i] = (int) ((freq - 1536000 / 2 + i * 1536000.0 / 512) / 1000);
	   Y_value [i] = 0;
	   for (int j = 0; j < 4; j ++) 
	      Y_value [i] +=  avg [4 * i + j];
	   Y_value [i]	=  get_db (Y_value [i]);
	}

	myTII_Scope		-> display (X_axis, Y_value, freq, 
	                                      spectrumAmplitude -> value ());
	for (int i = 0; i < 512; i ++)
	   Y_value [i] = (Y_value [i] - get_db (0)) / 6;
	myWaterfallScope	-> display (X_axis, Y_value, 
	                                    waterfallSlider -> value (),
	                                    freq / 1000);
}

void	displayWidget::showChannel	(std::vector<Complex> Values) {
double	amplitudeValues [128];
double	phaseValues     [128];
double	X_axis          [128];
double	waterfall_X	[512];
double	waterfall_Y	[512];

#ifdef __ESTIMATOR_
	if (currentTab != SHOW_CHANNEL)
	   return;
	for (int i = 0; i < 128; i ++) {
	   amplitudeValues [i] = 2 * abs (Values [i]);
	   phaseValues     [i] = 2 * arg (Values [i]) + 30;
	   X_axis          [i] = - 1536 / 2 + 12 * i;
	}
	myChannelScope	-> display (X_axis, amplitudeValues, phaseValues, 100);

	for (int i = 0; i < 128; i ++) {
	   for (int j = 0; j < 4; j ++) {
	      waterfall_X [4 * i + j ] = -1536 / 2 + 12 * i + 3 * j;
	      waterfall_Y [4 * i + j] = amplitudeValues [i];
	   }
	}
	myWaterfallScope	-> display (waterfall_X, waterfall_Y, 
	                                    waterfallSlider -> value (),
	                                    0);
#endif
}

//
//	for IQ we get a segment of 512 complex v alues, i.e. the
//	decoded values
void	displayWidget::showIQ	(std::vector<Complex> Values) {
int sliderValue	=  scopeSlider -> value ();
float	avg = 0;

	if (Values. size () < 512)
	   return;
        myIQDisplay -> DisplayIQ (Values. data (), 512, sliderValue * 2);
}

void	displayWidget:: showQuality (float q, float timeOffset,	
	                              float freqOffset) {
	if (myFrame. isHidden ())
	   return;

	quality_display -> display (q);
	timeOffsetDisplay	-> display (timeOffset);
	frequencyOffsetDisplay	-> display (freqOffset);
}

void	displayWidget::show_Corrector (int coarseOffset, float fineOffset) {
	if (myFrame. isHidden ())
	   return;

	coarse_correctorDisplay	-> display (coarseOffset);
	fine_correctorDisplay	-> display (fineOffset);
}

	
void	displayWidget::show_snr	(float snr) {
	if (myFrame. isHidden ())
	   return;
	snrDisplay		-> display (snr);
}

void	displayWidget::show_correction	(int c) {
	if (myFrame. isHidden ())
	   return;
//	correctorDisplay	-> display (c);
}

void	displayWidget::show_clockErr	(int e) {
	if (!myFrame. isHidden ())
	   clockError -> display (e);
}

void	displayWidget::showFrequency (float f) {
	frequencyDisplay	-> display (f);
}

void	displayWidget::show_cpuLoad	(float use) {
}

void	displayWidget::showTransmitters	(QByteArray &tr) {
QString textList;
	for (int i = 0; i < tr. size () / 2; i ++) {
	   QString trId = QString ("(") + QString::number (tr. at (2 * i)) +
	                  " " +QString::number (tr. at (2 * i + 1)) + ") ";
	   textList. append (trId);
	}
	tiiLabel -> setText (textList);
}
	   
void	displayWidget::show () {
	myFrame. show ();
}

void	displayWidget::hide () {
	myFrame. hide ();
}

bool	displayWidget::isHidden () {
	return myFrame. isHidden ();
}

void	displayWidget::rightMouseClick () {
	emit mouseClick ();
}

