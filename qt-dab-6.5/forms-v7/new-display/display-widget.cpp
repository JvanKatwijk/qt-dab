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
#include	"dev-scope.h"
#include	"waterfall-scope.h"
#include	"iqdisplay.h"

#include	"position-handler.h"

#define	DISPLAY_WIDGET_SETTINGS	"displayWidget"

	displayWidget::displayWidget	(RadioInterface	*mr,
	                                 QSettings	*dabSettings):
	                                         myFrame (nullptr),
	                                         theFFT (4 * 512, false),
	                                         dabSettings_p (dabSettings) {
	(void)mr;
int	sliderValue;
        setupUi (&myFrame);
	set_position_and_size (dabSettings_p, &myFrame,
	                                     DISPLAY_WIDGET_SETTINGS);
	connect (&myFrame, SIGNAL (frameClosed ()),
	         this, SIGNAL (frameClosed ())); 
//
	myFrame. hide ();

	dcOffset_display	-> hide ();
	dcOffset_label		-> hide ();
//	the "workers"
	spectrumScope_p		= new spectrumScope	(spectrumDisplay,
	                                                512, dabSettings_p);
	nullScope_p		= new nullScope		(nullDisplay,
	                                                512, dabSettings_p);
	correlationScope_p	= new correlationScope (correlationDisplay,
	                                                256, dabSettings_p);
	TII_Scope_p		= new spectrumScope	(tiiDisplay,
	                                                512, dabSettings_p);
	channelScope_p		= new channelScope	(channelPlot,
	                                                 NR_TAPS,
	                                                 dabSettings_p);
	devScope_p		= new devScope		(devPlot,
	                                                 512, dabSettings_p);
	IQDisplay_p		= new IQDisplay		(iqDisplay, 512);
	waterfallScope_p	= new waterfallScope	(waterfallDisplay,
	                                                512, 50);
//
//	and the settings for the sliders:
	dabSettings_p		-> beginGroup (DISPLAY_WIDGET_SETTINGS);
	sliderValue		= dabSettings_p -> value ("spectrumSlider",
	                                                   30). toInt ();
	spectrumSlider		-> setValue (sliderValue);
        currentTab		= dabSettings_p -> value ("tabSettings", 0). toInt ();
        tabWidget		-> setCurrentIndex (currentTab);
	sliderValue		= dabSettings_p -> value ("correlationSlider",
	                                                   50). toInt ();
	correlationSlider	-> setValue (sliderValue);
	correlationLength       -> setValue (500);
        dabSettings_p		-> endGroup ();
	correlationsVector 	-> 
	          setStyleSheet (
	                 "QLabel {background-color : green; color: white}");

	dabSettings_p	-> beginGroup ("displayWidget");
	sliderValue		=
	           dabSettings_p -> value ("iqSliderValue", 50). toInt ();
	scopeSlider		-> setValue (sliderValue);
	logScope		= 
	           dabSettings_p -> value ("logScope", 0). toInt () == 1;
	if (logScope != 0)
	   logScope_checkBox -> setChecked (true);
	sliderValue		= dabSettings_p -> value ("tiiSlider",
	                                                   00). toInt ();
	tiiSlider		-> setValue (sliderValue);
	sliderValue		= dabSettings_p	-> value ("deviationSlider",
	                                                  0). toInt ();
	deviationSlider		-> setValue (sliderValue);
	sliderValue		= dabSettings_p -> value ("channelSlider",
	                                                  20). toInt ();
	channelSlider		-> setValue (sliderValue);
	dabSettings_p	-> endGroup ();

	connect (tabWidget, SIGNAL (currentChanged (int)),
                 this, SLOT (switch_tab (int)));
	connect (IQDisplay_p, SIGNAL (rightMouseClick ()),
	         this, SLOT (rightMouseClick ()));
	connect (logScope_checkBox, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_logScope_checkBox (int)));
}

	displayWidget::~displayWidget () {
	store_widget_position (dabSettings_p, &myFrame,
	                                   DISPLAY_WIDGET_SETTINGS);
	                          
	dabSettings_p	-> beginGroup (DISPLAY_WIDGET_SETTINGS);
	dabSettings_p	-> setValue ("iqSliderValue",
	                               scopeSlider -> value ());
	dabSettings_p	-> setValue ("spectrumSlider",
	                               spectrumSlider -> value ());
	dabSettings_p	-> setValue ("correlationSlider",
	                               correlationSlider -> value ());
	dabSettings_p	-> setValue ("tiiSlider",
	                               tiiSlider -> value ());
	dabSettings_p	-> setValue ("deviationSlider",
	                               deviationSlider -> value ());
	dabSettings_p	-> setValue ("channelSlider",
	                               channelSlider -> value ());
	dabSettings_p	-> endGroup ();
	myFrame. hide	();
	delete		spectrumScope_p;
	delete		waterfallScope_p;
	delete		nullScope_p;
	delete		correlationScope_p;
	delete		TII_Scope_p;
	delete		channelScope_p;
	delete		devScope_p;
	delete		IQDisplay_p;
}

void	displayWidget::switch_tab	(int t) {
	currentTab	= t;
	dabSettings_p	-> beginGroup ("displayWidget");
	dabSettings_p	-> setValue ("tabSettings", t);
	dabSettings_p	-> endGroup ();
	waterfallScope_p	-> cleanUp ();
}

int	displayWidget::get_tab		() {
	return currentTab == 0 ? SHOW_SPECTRUM :
	       currentTab == 1 ? SHOW_CORRELATION :
	       currentTab == 2 ? SHOW_NULL : 
	       currentTab == 3 ? SHOW_TII :
	       currentTab == 4 ? SHOW_CHANNEL : SHOW_STDDEV;
}

///////////////////////////////////////////////////////////////////////
//	entries
///////////////////////////////////////////////////////////////////////
//
//	for "spectrum" we get a segment of 2048 timedomain samples
//	we take the fft and average a little
void	displayWidget::show_spectrum	(std::vector<Complex> &v,
	                                                   int freq) {
int	l	= v. size ();
double	X_axis [512];
double  Y_value [512];

static double avg [4 * 512];
	if (currentTab != SHOW_SPECTRUM)
	   return;

	theFFT. fft (v);
	for (int i = 0; i < (int)(v. size ()) / 2; i ++) {
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

	spectrumScope_p -> display (X_axis, Y_value, freq, 
	                                    spectrumSlider -> value ());
	for (int i = 0; i < 512; i ++)
	   Y_value [i] = (Y_value [i] - get_db (0)) / 6;
	waterfallScope_p	-> display (X_axis, Y_value, 
	                                    waterfallSlider -> value (),
	                                    freq / 1000);
}
//
//	for "corr" we get a segment of 1024 float values,
//	with as second parameter a list of indices with maximum values
void	displayWidget::show_correlation	(const std::vector<float> &v, int T_g,
	                                 QVector<int> &ww, int baseDistance) {
	if (currentTab != SHOW_CORRELATION)
	   return;
	correlationScope_p	-> display (v, T_g,
	                                    correlationLength -> value (),
	                                    correlationSlider -> value ());
	if (ww. size () > 0) {
	   QString t = "Matches ";
	   for (int i = 0; i < ww. size (); i ++) {
	      if (i == 0) {
	         t = t + " " + QString::number (ww [i]);
	         if (baseDistance > 0)
	            t = t + " (" + QString::number (baseDistance) + ")";
	      }
	      if (i >= 1) {
	         int lO = ww [i] - ww [0];
	         if (baseDistance <= 0)
	            t = t + " " + QString::number (lO);
	         else {
	            int d = M_PER_SAMPLE * lO / 1000 + baseDistance;
	            t = t + " (" + QString::number (d) + "km)";
	         }
	      }
	   }
	   correlationsVector -> setText (t);
	}

	if (v. size () < 512)
	   return;
	double X_axis [512];
	double Y_value [512];
	float	MMax	= 0;
	for (int i = v. size () / 2 - 256;
	                       i < (int)( v. size ()) / 2 + 256; i ++) {
	   X_axis [i - v. size () / 2 + 256] = i;
	   Y_value [i - v. size () / 2 + 256] = v [i];
	   if (v [i] > MMax)
	      MMax = v [i];
	}
	for (int i = 0; i < 512; i ++)
	   Y_value [i] *= 50.0 / MMax;
	waterfallScope_p -> display (X_axis, Y_value, 
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
	   v [i] = (v [2 * i] + v [2 * i + 1]) / (DABFLOAT)2.0;
	nullScope_p	-> display (v, amount);
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
	waterfallScope_p	-> display (X_axis, Y_value, 
	                                    waterfallSlider -> value (), 256);
}
//
//	for "tii" we get a segment of 2048 time domain samples,
//	we take an FFT, do some averaging and display
void	displayWidget::show_tii	(std::vector<Complex> v, int freq) {
int	l	= v. size ();
double	X_axis [512];
double  Y_value [512];

static double avg [4 * 512];
	if (currentTab != SHOW_TII)
	   return;

	theFFT. fft (v);
	for (int i = 0; i < (int)(v. size ()) / 2; i ++) {
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

	TII_Scope_p		-> display (X_axis, Y_value, freq, 
	                                      tiiSlider -> value ());
	for (int i = 0; i < 512; i ++)
	   Y_value [i] = (Y_value [i] - get_db (0)) / 6;
	waterfallScope_p	-> display (X_axis, Y_value, 
	                                    waterfallSlider -> value (),
	                                    freq / 1000);
}

void	displayWidget::show_channel	(const std::vector<Complex> Values) {
double	amplitudeValues	[NR_TAPS];
double	phaseValues	[NR_TAPS];
double	X_axis		[NR_TAPS];
double	waterfall_X	[512];
double	waterfall_Y	[512];

int	length	= Values. size () < NR_TAPS ? Values. size () : NR_TAPS;
	if (currentTab != SHOW_CHANNEL)
	   return;
	for (int i = 0; i < length; i ++) {
	   amplitudeValues [i] = 4 * abs (Values [i]);
	   phaseValues     [i] = arg (Values [i]) + 20;
	   X_axis          [i] = i;
	}
	for (int i = length; i < NR_TAPS; i ++) {
	   amplitudeValues [i] = 1;
	   phaseValues     [i] = 0;
	   X_axis	   [i] = i;
	}
	channelScope_p	-> display (X_axis,
	                            amplitudeValues,
	                            phaseValues, channelSlider -> value ());

	for (int i = 0; i < NR_TAPS; i ++) {
	   int f = 512 / NR_TAPS;
	   for (int j = 0; j < f; j ++) {
	      waterfall_X [f * i + j] = f * i + j;
	      waterfall_Y [f * i + j] = arg (Values [i]);
	   }
	}
	waterfallScope_p	-> display (waterfall_X, waterfall_Y, 
	                                    waterfallSlider -> value (),
	                                    0);
}

void	displayWidget::show_stdDev	(const std::vector<float> stdDevVector) {
double X_axis [512];
double Y_value [512];

	if (currentTab != SHOW_STDDEV)
	   return;
	devScope_p -> display (stdDevVector);
	for (int i = 0; i < 512; i ++) {
	   X_axis [i] = -768 + 3 * i;
	   Y_value [i] = stdDevVector [3 * i] * 5;
	}
	waterfallScope_p	-> display (X_axis, Y_value, 
	                                    waterfallSlider -> value (),
	                                    0);
}

//
//	for IQ we get a segment of 512 complex values, i.e. the
//	decoded values
void	displayWidget::showIQ	(const std::vector<Complex> Values) {
int sliderValue	=  scopeSlider -> value ();
std::vector<Complex> tempDisplay (512);

	if (Values. size () < 512)
	   return;

	if (logScope) {
	   float	logNorm	= std::log10 (2.0f);
	   for (int i = 0; i < 512; i ++) {
	      float phi	= arg (Values [i]);
	      float amp	= log10 (1.0f + abs (Values [i])) / logNorm;
	      std::complex<float> temp =
	                       amp * std::exp (std::complex<float> (0, phi));
	      tempDisplay [i] = Complex (real (temp), imag (temp));
	   }

           IQDisplay_p -> DisplayIQ (tempDisplay, 512, sliderValue * 2);
	}
	else
           IQDisplay_p -> DisplayIQ (Values, 512, sliderValue * 2);
}

void	displayWidget:: show_quality (float q, float timeOffset,	
	                              float freqOffset) {
	if (myFrame. isHidden ())
	   return;

	quality_display		-> display (QString ("%1").
	                                   arg (q, 0, 'f', 2));
	timeOffsetDisplay	-> display (QString ("%1").
	                                   arg (timeOffset, 0, 'f', 2));
	frequencyOffsetDisplay	-> display (QString ("%1"). 
	                                   arg (freqOffset, 0, 'f', 2));
}

void	displayWidget::show_corrector (int coarseOffset, float fineOffset) {
	if (myFrame. isHidden ())
	   return;

	coarse_correctorDisplay	-> display (coarseOffset);
	fine_correctorDisplay	-> display (fineOffset);
}

	
void	displayWidget::show_snr	(float snr) {
	if (myFrame. isHidden ())
	   return;
	snrDisplay		-> display (QString ("%1").arg (snr, 0, 'f', 2));
}

void	displayWidget::show_correction	(int c) {
	if (myFrame. isHidden ())
	   return;
	(void)c;
//	correctorDisplay	-> display (c);
}

void	displayWidget::show_clock_err	(int e) {
	if (!myFrame. isHidden ())
	   clock_errorDisplay -> display (e);
}

void	displayWidget::showFrequency (int freq) {
	freq	/= 1000;
	QString p1	= QString::number ((int)((freq) / 1000));
	QString p2	= QString::number ((freq) % 1000);
	if (freq % 1000 < 100)
	   p2 = "0" + p2;
	frequencyDisplay	-> display (p1 + '.' + p2);
}

void	displayWidget::show_cpuLoad	(float use) {
	(void)use;
}

void	displayWidget::show_transmitters	(QByteArray &tr) {
QString textList;
	for (int i = 0; i < tr. size () / 2; i ++) {
	   uint16_t mainId	= tr. at (2 * i);
	   uint16_t subId	= tr. at (2 * i + 1);
	   QString trId = QString ("(") + QString::number (mainId) +
	                  " " + QString::number (subId) + ") ";
	   textList. append (trId);
	}
	tiiLabel -> setText (textList);
}

void	displayWidget::set_syncLabel	(bool b) {
	if (b)
	   syncLabel    -> setStyleSheet ("QLabel {background-color : green}");
	else
	   syncLabel    -> setStyleSheet ("QLabel {background-color : yellow}");              
}

void	displayWidget::show_dcOffset	(float dcOffset) {
	dcOffset_display	-> display (dcOffset);
}

void	displayWidget::set_dcRemoval	(bool b) {
	if (b) {
	   dcOffset_display	-> show ();
	   dcOffset_label	-> show ();
	}
	else {
	   dcOffset_display	-> hide ();
	   dcOffset_label	-> hide ();
	}
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

void	displayWidget::set_bitDepth	(int d) {
	spectrumScope_p	-> set_bitDepth (d);
}

void	displayWidget::handle_logScope_checkBox	(int d) {
	(void)d;
	logScope	= logScope_checkBox -> isChecked ();
	
	dabSettings_p	-> beginGroup ("displayWidget");
	dabSettings_p	-> setValue ("logScope", logScope ? 1 : 0);
	dabSettings_p	-> endGroup ();
}
