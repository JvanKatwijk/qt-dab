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

#include	"dab-constants.h"
#include	"position-handler.h"
#include	"settingNames.h"
#include	"settings-handler.h"

#define	DISPLAY_WIDGET_SETTINGS	"displayWidget"

	displayWidget::displayWidget	(RadioInterface	*mr,
	                                 QSettings	*dabSettings):
	                                         myFrame (nullptr),
	                                         theFFT (4 * 512, false),
	                                         dabSettings_p (dabSettings) {
	(void)mr;
int	sliderValue;
	QString settingsHeader	= DISPLAY_WIDGET_SETTINGS;
	setupUi (&myFrame);
	set_position_and_size (dabSettings_p, &myFrame, 
	                           DISPLAY_WIDGET_SETTINGS);
	connect (&myFrame, SIGNAL (frameClosed ()),
	         this, SIGNAL (frameClosed ())); 
//
	myFrame. hide ();

	dcOffset_display	-> show ();
	dcOffset_label		-> show ();
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
	IQDisplay_p		= new IQDisplay		(iqDisplay);
	waterfallScope_p	= new waterfallScope	(waterfallDisplay,
	                                                512, 50);
//
//	and the settings for the sliders:
	sliderValue		= value_i (dabSettings_p,
	                                   DISPLAY_WIDGET_SETTINGS,
	                                   "spectrumSlider", 30);
	spectrumSlider		-> setValue (sliderValue);
        currentTab		= value_i (dabSettings_p,
		                           DISPLAY_WIDGET_SETTINGS,
	                                   "tabSettings", 0);
        tabWidget		-> setCurrentIndex (currentTab);
	sliderValue		= value_i (dabSettings_p,
	                                   DISPLAY_WIDGET_SETTINGS,
	                                   "correlationSlider", 50);
	correlationSlider	-> setValue (sliderValue);
	correlationLength       -> setValue (500);
	correlationsVector 	-> 
	          setStyleSheet (
	                 "QLabel {background-color : green; color: white}");

	sliderValue		=
	           value_i (dabSettings_p, DISPLAY_WIDGET_SETTINGS,
	                                "iqSliderValue", 50);
	scopeSlider		-> setValue (sliderValue);
	ncpScope		= 
	           value_i (dabSettings_p, DISPLAY_WIDGET_SETTINGS,
	                                 "ncpScope", 0) != 0;
	if (ncpScope != 0)
	   ncpScope_checkBox -> setChecked (true);
	sliderValue		= value_i (dabSettings_p, 
	                                   DISPLAY_WIDGET_SETTINGS,
	                                   "tiiSlider", 00);
	tiiSlider		-> setValue (sliderValue);
	sliderValue		= value_i (dabSettings_p,
	                                   DISPLAY_WIDGET_SETTINGS,
	                                   "deviationSlider", 0);
	deviationSlider		-> setValue (sliderValue);
	sliderValue		= value_i (dabSettings_p,
	                                   DISPLAY_WIDGET_SETTINGS,
		                           "channelSlider", 20);
	channelSlider		-> setValue (sliderValue);

	connect (tabWidget, SIGNAL (currentChanged (int)),
                 this, SLOT (switch_tab (int)));
	connect (IQDisplay_p, SIGNAL (rightMouseClick ()),
	         this, SLOT (rightMouseClick ()));
	connect (ncpScope_checkBox, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_ncpScope_checkBox (int)));
}

	displayWidget::~displayWidget () {
	store_widget_position (dabSettings_p, &myFrame,
	                                 DISPLAY_WIDGET_SETTINGS);
	store (dabSettings_p, DISPLAY_WIDGET_SETTINGS,
	                       "iqSliderValue", scopeSlider -> value ());
	store (dabSettings_p, DISPLAY_WIDGET_SETTINGS,
	                     "spectrumSlider", spectrumSlider -> value ());
	store (dabSettings_p, DISPLAY_WIDGET_SETTINGS,
	                     "correlationSlider", correlationSlider -> value ());
	store (dabSettings_p, DISPLAY_WIDGET_SETTINGS,
	                     "tiiSlider", tiiSlider -> value ());
	store (dabSettings_p, DISPLAY_WIDGET_SETTINGS,
	                     "deviationSlider", deviationSlider -> value ());
	store (dabSettings_p, DISPLAY_WIDGET_SETTINGS,
	                     "channelSlider", channelSlider -> value ());
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
	store (dabSettings_p, DISPLAY_WIDGET_SETTINGS, "tabSettings", t);
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
floatQwt  X_axis [512];
floatQwt  Y_value [512];

static floatQwt avg [4 * 512];
	if (currentTab != SHOW_SPECTRUM)
	   return;

	theFFT. fft (v);
	for (int i = 0; i < (int)(v. size ()) / 2; i ++) {
	   avg [i] = 0.5 * avg [i] + 0.5 * abs (v [l / 2 + i]);
	   avg [l / 2 + i] = 0.5 * avg [l / 2 + i] + 0.5 * abs (v [i]);
	}

	for (int i = 0; i < 512; i ++) {
	   X_axis [i] = (freq - 1536000 / 2 + i * 1536000.0 / 512) / 1000000.0;
	   Y_value [i] = 0;
	   for (int j = 0; j < 4; j ++) 
	      Y_value [i] +=  avg [4 * i + j];
	   Y_value [i]	=  get_db (Y_value [i] / 4);
	}

	spectrumScope_p -> display (X_axis, Y_value, freq / 1000, 
	                                    spectrumSlider -> value ());
	for (int i = 0; i < 512; i ++)
	   Y_value [i] = (Y_value [i] - get_db (0)) / 6;
	waterfallScope_p	-> display (X_axis, Y_value, 
	                                    waterfallSlider -> value (),
	                                    freq / 1000);
}

std::vector<corrElement> sort (std::vector<corrElement> in) {
std::vector<corrElement> res;
	return res;
}

//	for "corr" we get a segment of 1024 float values,
//	with as second parameter a list of indices with maximum values
//	and a list of transmitters
void	displayWidget::show_correlation	(const std::vector<float> &v,
	                                 QVector<int> &maxVals,
	                                 int T_g,
	                                 std::vector<transmitterDesc> &theTr) {
std::vector<corrElement> showData;
	if (currentTab != SHOW_CORRELATION)
	   return;

	for (auto &theTransm : theTr) {
	   corrElement t;
	   t. mainId	= theTransm. theTransmitter. mainId;
	   t. subId	= theTransm. theTransmitter. subId;
	   t. phase	= theTransm. theTransmitter. phase;
	   t. Name	= theTransm. theTransmitter. transmitterName;
	   t. strength	= theTransm. theTransmitter. strength;
	   t. norm	= theTransm. theTransmitter. norm;

	  showData. push_back (t);
	}
	float max	= 0;
	int maxInd	= -1;
	for (int i = 0; i < showData. size (); i ++) {
	   if (showData [i]. strength > max) {
	      maxInd = i;
	      max = showData [i]. strength;
	   }
	}
	
	if (maxInd >= 0) {
	   int mainId	= showData [maxInd]. mainId;
	   int subId	= showData [maxInd]. subId;
	   QString name	= showData [maxInd]. Name;
	   QString ss = "(" + QString::number (mainId) +
	               " " + QString::number (subId) + ") " + name;
	   correlationsVector -> setText (ss);
	}

	if (value_i (dabSettings_p, CONFIG_HANDLER, "show_lines", 0) == 0)
	   showData. resize (0);
	correlationScope_p	-> display (v, T_g,
	                                    correlationLength -> value (),
	                                    correlationSlider -> value (),
	                                    showData);
//	and now for the waterfall scope
	if (v. size () < 512)
	   return;
	floatQwt X_axis [512];
	floatQwt Y_value [512];
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
void	displayWidget::show_null	(Complex *v, int amount,
	                                      int startIndex) {
	if  (currentTab != SHOW_NULL)
	   return;
	if (amount < 1024)
	   return;
	for (int i = 0; i < 512; i ++)
	   v [i] = (v [2 * i] + v [2 * i + 1]) / (DABFLOAT)2.0;

	nullScope_p	-> display (v, amount, startIndex);
	floatQwt X_axis [512];
	floatQwt Y_value [512];
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
floatQwt	X_axis [512];
floatQwt  Y_value [512];

static floatQwt avg [4 * 512];
	if (currentTab != SHOW_TII)
	   return;

	theFFT. fft (v);
	for (int i = 0; i < (int)(v. size ()) / 2; i ++) {
	   avg [i] = 0.5 * avg [i] + 0.5 * abs (v [l / 2 + i]);
	   avg [l / 2 + i] = 0.5 * avg [l / 2 + i] + 0.5 * abs (v [i]);
	}

	for (int i = 0; i < 512; i ++) {
	   X_axis [i] = (freq - 1536000 / 2 + i * 1536000.0 / 512) / 1000000.0;
	   Y_value [i] = 0;
	   for (int j = 0; j < 4; j ++) 
	      Y_value [i] +=  avg [4 * i + j];
	   Y_value [i]	=  get_db (Y_value [i]);
	}

	TII_Scope_p		-> display (X_axis, Y_value, freq / 1000, 
	                                      tiiSlider -> value ());
	for (int i = 0; i < 512; i ++)
	   Y_value [i] = (Y_value [i] - get_db (0)) / 6;
	waterfallScope_p	-> display (X_axis, Y_value, 
	                                    waterfallSlider -> value (),
	                                    freq / 1000);
}

void	displayWidget::show_channel	(const std::vector<Complex> Values) {
floatQwt	amplitudeValues	[NR_TAPS];
floatQwt	phaseValues	[NR_TAPS];
floatQwt	X_axis		[NR_TAPS];
floatQwt	waterfall_X	[512];
floatQwt	waterfall_Y	[512];

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
floatQwt X_axis [512];
floatQwt Y_value [512];

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

	if (!ncpScope) {
	   float	logNorm	= std::log10 (2.0f);
	   for (int i = 0; i < 512; i ++) {
	      float phi	= arg (Values [i]);
	      float amp	= log10 (1.0f + abs (Values [i])) / logNorm;
	      std::complex<float> temp =
	                       amp * std::exp (std::complex<float> (0, phi));
	      tempDisplay [i] = Complex (real (temp), imag (temp));
	   }

           IQDisplay_p -> displayIQ (tempDisplay, sliderValue * 2);
	}
	else
           IQDisplay_p -> display_centerPoints (Values, sliderValue * 2);
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
	coarse_correctorDisplay	-> display (coarseOffset + fineOffset);
//	fine_correctorDisplay	-> display (fineOffset);
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

void	displayWidget::showFrequency (const QString &channel, int freq) {
	freq	/= 1000;
	QString p1	= QString::number ((int)((freq) / 1000));
	QString p2	= QString::number ((freq) % 1000);
	if (freq % 1000 < 100)
	   p2 = "0" + p2;
	frequencyDisplay	-> display (p1 + '.' + p2);
	QFont font	= channelDisplay -> font ();
	font. setPointSize (18);
	channelDisplay		-> setFont (font);
	channelDisplay		-> setText (channel);
}

void	displayWidget::show_cpuLoad	(float use) {
	(void)use;
}

void	displayWidget::show_transmitters (std::vector<transmitterDesc> &tr) {
QString textList;
	for (int i = 0; i < tr. size (); i ++) {
	   uint16_t mainId	= tr [i]. theTransmitter. mainId;
	   uint16_t subId	= tr [i]. theTransmitter. subId;
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

void	displayWidget::show_ficBER	(float ber) {
	ber_display	-> display (ber);
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

void	displayWidget::handle_ncpScope_checkBox	(int d) {
	(void)d;
	ncpScope	= ncpScope_checkBox -> isChecked ();
	
	store (dabSettings_p, DISPLAY_WIDGET_SETTINGS,"ncpScope", ncpScope ? 1 : 0);
}

void	displayWidget::setSilent	() {
	if (currentTab == SHOW_SPECTRUM)
	   spectrumScope_p	-> clean ();

	if (currentTab == SHOW_NULL) 
	   nullScope_p		-> clean ();

	if (currentTab == SHOW_CORRELATION)
	   correlationScope_p	-> clean ();

	if (currentTab == SHOW_TII)
	   TII_Scope_p		-> clean ();

	if (currentTab == SHOW_CHANNEL)
	   channelScope_p	-> clean ();

	if (currentTab == SHOW_STDDEV)
	   devScope_p		-> clean ();
}

