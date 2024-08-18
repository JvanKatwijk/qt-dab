#
/*
 *    Copyright (C)  2016 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB 
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
#include	"techdata.h"
#include	"radio.h"
#include	"audio-display.h"
#include	"dab-tables.h"
#include	<QColorDialog>

#include	"settingNames.h"
#include	"position-handler.h"

#define	TECHDATA_SETTING	"techDataSettings"

#define FRAMEDUMP_BUTTON        QString ("framedumpButton") 
#define AUDIODUMP_BUTTON        QString ("audiodumpButton")

		techData::techData	(RadioInterface *mr,
	                                 QSettings	*s,
	                                 RingBuffer<std::complex<int16_t>> *audioData):
	                                         myFrame (nullptr) {
	myRadioInterface	= mr;
	dabSettings		= s;
	this	-> audioData	= audioData;

        setupUi (&myFrame);
	QString settingsHeader	= TECHDATA_SETTING;

	set_position_and_size (dabSettings, &myFrame, TECHDATA_SETTING);

	formLayout -> setLabelAlignment (Qt::AlignLeft);
//	myFrame. setWindowFlag (Qt::Tool, true);
	myFrame. hide ();
	timeTable_button	-> setEnabled (false);
	the_audioDisplay	= new audioDisplay (mr, audio, dabSettings);

	dabSettings	-> beginGroup (COLOR_SETTINGS);
	QString framedumpButton_color = 
           dabSettings -> value (FRAMEDUMP_BUTTON + "_color",
                                                      "white"). toString ();
	QString framedumpButton_font =  
           dabSettings -> value (FRAMEDUMP_BUTTON + "_font",
                                                      "black"). toString ();
	QString audiodumpButton_color = 
           dabSettings -> value (AUDIODUMP_BUTTON + "_color",
                                                      "white"). toString ();
	QString audiodumpButton_font =  
           dabSettings -> value (AUDIODUMP_BUTTON + "_font",
                                                      "black"). toString ();
	dabSettings	-> endGroup ();

	QString temp = "QPushButton {background-color: %1; color: %2}";

	framedumpButton ->
                      setStyleSheet (temp. arg (framedumpButton_color,
                                                framedumpButton_font));

	audiodumpButton ->
                      setStyleSheet (temp. arg (audiodumpButton_color,
                                                audiodumpButton_font));
	peakLeftDamped		= -100;
	peakRightDamped         = -100;

	thermoLeft		-> setFillBrush (Qt::darkBlue);
        thermoRight		-> setFillBrush (Qt::darkBlue);
        thermoLeft		-> setAlarmBrush (Qt::red);
        thermoRight		-> setAlarmBrush (Qt::red);
        thermoLeft		-> setAlarmEnabled (true);
        thermoRight		-> setAlarmEnabled(true);

	connect (&myFrame, &superFrame::frameClosed,
	         this,  &techData::frameClosed);
	connect (framedumpButton, &smallPushButton::rightClicked,
                 this, &techData::color_framedumpButton);
        connect (audiodumpButton, &smallPushButton::rightClicked,
                 this, &techData::color_audiodumpButton); 

	connect	(framedumpButton, &QPushButton::clicked,
	         this, &techData::handle_frameDumping);
	connect (audiodumpButton, &QPushButton::clicked,
	         this, &techData::handle_audioDumping);

	connect (timeTable_button, &QPushButton::clicked,
	         this, &techData::handle_timeTable);
}

		techData::~techData	() {
	store_widget_position (dabSettings, &myFrame, TECHDATA_SETTING);
	delete the_audioDisplay;
}

void	techData::cleanUp	() {
	const QString ee ("-");
	programName		-> setText (ee);
	rsCorrections		-> display (0);
	frameError_display	-> setValue (0);
	rsError_display		-> setValue (0);
	aacError_display	-> setValue (0);
	bitrateDisplay		-> display (0);
	startAddressDisplay	-> display (0);
	lengthDisplay		-> display (0);
	subChIdDisplay		-> display (0);
	uepField		-> setText (ee);
	codeRate		-> setText (ee);
	ASCTy			-> setText (ee);
	language		-> setText (ee);
	motAvailable		-> 
	               setStyleSheet ("QLabel {background-color : red}");
	timeTable_button	-> setEnabled (false);
	audioRate		-> display (0);
}

void	techData::show_serviceData	(audiodata *ad) {
	show_serviceName	(ad -> serviceName, ad -> shortName);
	show_serviceId		(ad -> SId);
	show_bitRate		(ad -> bitRate);
	show_startAddress	(ad -> startAddr);
	show_length		(ad -> length);
	show_subChId		(ad -> subchId);
	show_uep		(ad -> shortForm, ad -> protLevel);
	show_ASCTy		(ad -> ASCTy);
	show_codeRate		(ad -> shortForm, ad -> protLevel);
	show_language		(ad -> language);
	show_fm			(ad -> fmFrequency);
}

void	techData::show		() {
	myFrame. show ();
}

void	techData::hide		() {
	myFrame. hide ();
}

bool	techData::isHidden	() {
	return myFrame. isHidden ();
}

void	techData::show_frameErrors	(int e) {
	QPalette p	= frameError_display -> palette();
	if (100 - 4 * e < 80)
	   p. setColor (QPalette::Highlight, Qt::red);
	else
	   p. setColor (QPalette::Highlight, Qt::green);

	frameError_display	-> setPalette (p);
	frameError_display	-> setValue (100 - 4 * e);
}

void	techData::show_aacErrors	(int e) {
	QPalette p      = aacError_display -> palette();
        if (100 - 4 * e < 80)
           p. setColor (QPalette::Highlight, Qt::red);
        else
           p. setColor (QPalette::Highlight, Qt::green);
        aacError_display      -> setPalette (p);
        aacError_display      -> setValue (100 - 4 * e);
}

void	techData::show_rsErrors		(int e) {
	QPalette p	= rsError_display -> palette();
	if (100 - 4 * e < 80)
	   p. setColor (QPalette::Highlight, Qt::red);
	else
	   p. setColor (QPalette::Highlight, Qt::green);
	rsError_display	-> setPalette (p);
	rsError_display	-> setValue (100 - 4 * e);
}

void	techData::show_rsCorrections	(int c, int ec) {
	rsCorrections -> display (c);
	ecCorrections -> display (ec);
}

void	techData::show_motHandling	(bool b) {

	motAvailable -> setStyleSheet (b ?  
                           "QLabel {background-color : green; color: white}":
                           "QLabel {background-color : red; color : white}");
}

void	techData::show_timetableButton	(bool b) {
	if (b)
	   timeTable_button	-> setEnabled (true);
	else
	   timeTable_button	-> setEnabled (false);
}

void	techData::show_frameDumpButton	(bool b) {
	if (b)
	   framedumpButton	-> show ();
	else
	   framedumpButton	-> hide ();
}

void	techData::show_serviceName	(const QString &s1, const QString &s2) {
	if ((s2 != "") && (s1 != s2)) 
	   programName	-> setText (s1 + "(" + s2 + ")");
	else
	   programName -> setText (s1);
}

void	techData::show_serviceId		(int SId) {
	serviceIdDisplay -> display (SId);
}

void	techData::show_bitRate		(int br) {
	bitrateDisplay	-> display (br);
}

void	techData::show_startAddress	(int sa) {
	startAddressDisplay	-> display (sa);
}

void	techData::show_length		(int l) {
	lengthDisplay	-> display (l);
}

void	techData::show_subChId		(int subChId) {
	subChIdDisplay	-> display (subChId);
}

void	techData::show_language		(int l) {
	language	-> setText (getLanguage (l));
}

void	techData::show_ASCTy		(int a) {
	ASCTy	-> setText (a == 077 ? "DAB+"  : "DAB");
	if (a == 077) {
	   rsError_display	-> show ();
	   aacError_display	-> show ();
	}
	else {
	   rsError_display	-> hide ();
	   aacError_display	-> hide ();
	}
}

void	techData::show_uep		(int shortForm, int protLevel) {
	QString protL = getProtectionLevel (shortForm, protLevel);
	uepField	-> setText (protL);
}

void	techData::show_codeRate		(int shortForm, int protLevel) {
	codeRate -> setText (getCodeRate (shortForm, protLevel));
}

void	techData::show_fm			(int freq) {
	if (freq == -1) {
	   fmFrequency	-> hide ();
	   fmLabel	-> hide ();
	}
	else {
	   fmLabel            -> show ();
           fmFrequency        -> show ();
           QString f = QString::number (freq);
           f. append (" Khz");
           fmFrequency        -> setText (f);
	}
}

void	techData::audioDataAvailable	(int amount, int rate) {
std::complex<int16_t> buffer [amount];

	audioData -> getDataFromBuffer (buffer, amount);
	if (myFrame. isHidden ())
	   return;
	the_audioDisplay -> createSpectrum (buffer, amount, rate);
	
}

void    techData::color_framedumpButton   ()      {
        set_buttonColors (framedumpButton, FRAMEDUMP_BUTTON);
}

void    techData::color_audiodumpButton   ()      {
        set_buttonColors (audiodumpButton, AUDIODUMP_BUTTON);
}

void	techData::set_buttonColors	(QPushButton *b,
	                                         const QString &buttonName) {
QColor	baseColor;
QColor	textColor;
QColor	color;

	color = QColorDialog::getColor (baseColor, nullptr, "baseColor");
	if (!color. isValid ())
	   return;
	baseColor	= color;
	color = QColorDialog::getColor (textColor, nullptr, "textColor");
	if (!color. isValid ())
	   return;
	textColor	= color;
	QString temp = "QPushButton {background-color: %1; color: %2}";
	b	-> setStyleSheet (temp. arg (baseColor. name (),
	                                     textColor. name ()));

	QString buttonColor	= buttonName + "_color";
	QString buttonFont	= buttonName + "_font";

	dabSettings	-> beginGroup (COLOR_SETTINGS);
	dabSettings	-> setValue (buttonColor, baseColor. name ());
	dabSettings	-> setValue (buttonFont, textColor. name ());
	dabSettings	-> endGroup ();
}

void	techData::framedumpButton_text	(const QString &text, int size) {
	QFont font	= framedumpButton -> font ();
	font. setPointSize (size);
	framedumpButton	-> setFont (font);
	framedumpButton	-> setText (text);
	framedumpButton	-> update ();
}

void	techData::audiodumpButton_text	(const QString &text, int size) {
	QFont font	= audiodumpButton -> font ();
	font. setPointSize (size);
	audiodumpButton	-> setFont (font);
	audiodumpButton	-> setText (text);
	audiodumpButton	-> update ();
}

void	techData::show_rate	(int rate, bool ps, bool sbr) {
	if (!ps)
	   psLabel -> setText (" ");
	else {
	   psLabel -> setStyleSheet ("QLabel {color : white}");
	   psLabel -> setText ("ps");
	}
	if (!sbr)
	   sbrLabel -> setText ("  ");
	else {
	   sbrLabel -> setStyleSheet ("QLabel {color : white}");
	   sbrLabel -> setText ("sbr");
	}
	audioRate	-> display (rate);
}

void	techData::showMissed	(int missed) {
	missedSamples	-> display (missed);
}

void	techData::hideMissed	() {
	missedLabel	-> hide ();
	missedSamples	-> hide ();
}

void	techData::showPeakLevel (const float iPeakLeft,
                                       const float iPeakRight) {
	auto peak_avr = [](float iPeak, float & ioPeakAvr) -> void {
	   ioPeakAvr = (iPeak > ioPeakAvr ? iPeak : ioPeakAvr - 0.5f /*decay*/);
	};

	peak_avr (iPeakLeft,  peakLeftDamped);
	peak_avr (iPeakRight, peakRightDamped);

	thermoLeft		-> setFillBrush (Qt::cyan);
        thermoRight		-> setFillBrush (Qt::cyan);
        thermoLeft		-> setValue (peakLeftDamped);
        thermoRight		-> setValue (peakRightDamped);
}

void	techData::is_DAB_plus	(bool b) {
	if (b) {	// yes it is DAB+
	   rsErrorLabel		-> show ();
	   aacErrorLabel	-> show ();
	   rsError_display	-> show ();
	   aacError_display	-> show ();
	}
	else {
	   rsErrorLabel		-> hide ();
	   aacErrorLabel	-> hide ();
	   rsError_display	-> hide ();
	   aacError_display	-> hide ();
	}
}

	
