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
#include	"settings-handler.h"
#include	"position-handler.h"

#define	TECHDATA_SETTING	"techDataSettings"

#define	DARK	"#00007f"
#define	WHITE	"#ffffff"

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

	setPositionAndSize (dabSettings, &myFrame, TECHDATA_SETTING);

//	formLayout -> setLabelAlignment (Qt::AlignLeft);
//	myFrame. setWindowFlag (Qt::Tool, true);
	myFrame. hide ();
	timeTable_button	-> setEnabled (false);
	theAudioDisplay	= new audioDisplay (mr, audio, dabSettings);

	QString framedumpButton_color = 
           value_s (dabSettings, COLOR_SETTINGS, 
	                              FRAMEDUMP_BUTTON + "_color", DARK);
	QString framedumpButton_font =  
           value_s (dabSettings, COLOR_SETTINGS,
	                              FRAMEDUMP_BUTTON + "_font", WHITE);
	QString audiodumpButton_color = 
           value_s (dabSettings, COLOR_SETTINGS,
	                              AUDIODUMP_BUTTON + "_color", DARK);
	QString audiodumpButton_font =  
           value_s (dabSettings, COLOR_SETTINGS,
	                              AUDIODUMP_BUTTON + "_font", WHITE);

	QString temp = "QPushButton {background-color: %1; color: %2}";

	framedumpButton ->
                      setStyleSheet (temp. arg (framedumpButton_color,
                                                framedumpButton_font));

	audiodumpButton ->
                      setStyleSheet (temp. arg (audiodumpButton_color,
                                                audiodumpButton_font));

	connect (&myFrame, &superFrame::frameClosed,
	         this,  &techData::frameClosed);
	connect (framedumpButton, &smallPushButton::rightClicked,
                 this, &techData::colorFramedumpButton);
        connect (audiodumpButton, &smallPushButton::rightClicked,
                 this, &techData::colorAudiodumpButton); 

	connect (audiodumpButton, &QPushButton::clicked,
	         mr, &RadioInterface::handleAudiodumpButton);
	connect	(framedumpButton, &QPushButton::clicked,
	         mr, &RadioInterface::handleFramedumpButton);

	connect (timeTable_button, &QPushButton::clicked,
	         this, &techData::handleTimeTable);
}

		techData::~techData	() {
	if (!myFrame. isHidden ())
	   storeWidgetPosition (dabSettings, &myFrame, TECHDATA_SETTING);
	delete theAudioDisplay;
}

void	techData::cleanUp	() {
	const QString ee ("-");
	programName		-> setText (ee);
	rsCorrections		-> display (0);
	frameError_display	-> setValue (0);
	rsError_display		-> setValue (0);
	aacError_display	-> setValue (0);
	startAddressDisplay	-> setText (QString::number (0));
	lengthDisplay		-> setText (QString::number (0));
	subChIdDisplay		-> setText (QString::number (0));
	uepField		-> setText (ee);
	codeRate		-> setText (ee);
	ASCTy			-> setText (ee);
	language		-> setText (ee);
	timeTable_button	-> setEnabled (false);
	audioRate		-> display (0);
}

void	techData::showServiceData	(audiodata *ad) {
	showServiceName		(ad -> serviceName, ad -> shortName);
	showServiceId		(ad -> SId);
	showStartAddress	(ad -> startAddr);
	showLength		(ad -> length);
	showSubChId		(ad -> subchId);
	showUep			(ad -> shortForm, ad -> protLevel);
	showCodeRate		(ad -> shortForm, ad -> protLevel);
	showLanguage		(ad -> language);
	showFm			(ad -> fmFrequencies);
	bitRateLabel		-> setText (QString::number (ad -> bitRate) + " kbits");
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

void	techData::showFrameErrors	(int e) {
	QPalette p	= frameError_display -> palette();
	if (100 - 4 * e < 80)
	   p. setColor (QPalette::Highlight, Qt::red);
	else
	   p. setColor (QPalette::Highlight, Qt::green);

	frameError_display	-> setPalette (p);
	frameError_display	-> setValue (100 - 4 * e);
}

void	techData::showAacErrors	(int e) {
	QPalette p      = aacError_display -> palette();
        if (100 - 4 * e < 80)
           p. setColor (QPalette::Highlight, Qt::red);
        else
           p. setColor (QPalette::Highlight, Qt::green);
        aacError_display      -> setPalette (p);
        aacError_display      -> setValue (100 - 4 * e);
}

void	techData::showRsErrors		(int e) {
	QPalette p	= rsError_display -> palette();
	if (100 - 4 * e < 80)
	   p. setColor (QPalette::Highlight, Qt::red);
	else
	   p. setColor (QPalette::Highlight, Qt::green);
	rsError_display	-> setPalette (p);
	rsError_display	-> setValue (100 - 4 * e);
}

void	techData::showRsCorrections	(int c, int ec) {
	rsCorrections -> display (c);
	ecCorrections -> display (ec);
}

void	techData::showTimetableButton	(bool b) {
	if (b)
	   timeTable_button	-> setEnabled (true);
	else
	   timeTable_button	-> setEnabled (false);
}

void	techData::updateFM		(std::vector<int> &fmFrequencies) {
	if (fmFrequencies. size () == 0)
	   return;
	showFm		(fmFrequencies);
}

void	techData::showFrameDumpButton	(bool b) {
	if (b)
	   framedumpButton	-> show ();
	else
	   framedumpButton	-> hide ();
}

void	techData::showServiceName	(const QString &s1, const QString &s2) {
	if ((s2 != "") && (s1 != s2)) 
	   programName	-> setText (s1 + "(" + s2 + ")");
	else
	   programName -> setText (s1);
}

static
QString hextoString (int v) {
QString res;
        for (int i = 0; i < 4; i ++) {
           uint8_t t = (v & 0xF000) >> 12;
           QChar c = t <= 9 ? (char)('0' + t) : (char) ('A' + t - 10);
           res. append (c); 
           v <<= 4;
        }
        return res;
}

void	techData::showServiceId		(int SId) {
QString text	= hextoString (SId);
	serviceIdDisplay -> setText (text);
}

void	techData::showStartAddress	(int sa) {
	startAddressDisplay	-> setText (QString::number (sa));
}

void	techData::showLength		(int l) {
	lengthDisplay		-> setText (QString::number (l));
}

void	techData::showSubChId		(int subChId) {
	subChIdDisplay		-> setText (QString::number (subChId));
}

void	techData::showLanguage		(int l) {
	language	-> setAlignment (Qt::AlignRight);
	language	-> setText (getLanguage (l));
}

void	techData::showUep		(int shortForm, int protLevel) {
	QString protL = getProtectionLevel (shortForm, protLevel);
	uepField	-> setText (protL);
}

void	techData::showCodeRate		(int shortForm, int protLevel) {
	codeRate -> setText (getCodeRate (shortForm, protLevel));
}

void	techData::showFm		(std::vector<int> &v) {
	if (v. size () == 0) {
	   fmFrequency	-> hide ();
	   fmLabel	-> hide ();
	}
	else {
	   fmLabel            -> show ();
           fmFrequency        -> show ();	
	   QString f;
	   int teller	= 0;
//	for now there is room for up to 2 freqyencies
	   for (auto freq: v) {
	      if (++teller > 2)
	         break;			// for now
              f. append (QString::number (freq) + " ");
	   }
           f. append (" Khz");
           fmFrequency        -> setText (f);
	}
}

void	techData::audioDataAvailable	(int amount, int rate) {
std::complex<int16_t> buffer [amount];

	audioData -> getDataFromBuffer (buffer, amount);
	if (myFrame. isHidden ())
	   return;
	theAudioDisplay -> createSpectrum (buffer, amount, rate);
	
}

void    techData::colorFramedumpButton   ()      {
        setButtonColors (framedumpButton, FRAMEDUMP_BUTTON);
}

void    techData::colorAudiodumpButton   ()      {
        setButtonColors (audiodumpButton, AUDIODUMP_BUTTON);
}

void	techData::setButtonColors	(QPushButton *b,
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
	QString	baseColor_name	= baseColor. name ();
	QString textColor_name	= textColor. name ();
	store (dabSettings, COLOR_SETTINGS, buttonColor, baseColor_name);
	store (dabSettings, COLOR_SETTINGS, buttonFont,  textColor_name);
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

void	techData::showRate	(int rate, bool ps, bool sbr) {
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

void	techData::showStereo	(bool b) {
	 if (b) {
	   stereoLabel	-> setStyleSheet ("QLabel {color : white}");
           stereoLabel  -> setText ("<i>stereo</i>");
        }
        else
           stereoLabel  -> setText ("      ");
}

void	techData::showMissed	(int missed) {
	missedSamples	-> display (missed);
}

void	techData::hideMissed	() {
	missedLabel	-> hide ();
	missedSamples	-> hide ();
}

void	techData::isDABPlus	(bool b) {
	if (b) {	// yes it is DAB+

	   ASCTy	-> setText ("DAB+");
	   rsErrorLabel		-> show ();
	   aacErrorLabel	-> show ();
	   rsError_display	-> show ();
	   aacError_display	-> show ();
	}
	else {
	   ASCTy	-> setText ("DAB");
	   rsErrorLabel		-> hide ();
	   aacErrorLabel	-> hide ();
	   rsError_display	-> hide ();
	   aacError_display	-> hide ();
	}
}

void	techData::hideMissedLabel	() {
	missedLabel	-> hide ();
	missedSamples	-> hide ();
}

