#
/*
 *    Copyright (C)  2016 .. 2026
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
#include	"tech-window.h"
#include	"radio.h"
#include	"audio-display.h"
#include	"dab-tables.h"
#include	<QColorDialog>

#include	"settingNames.h"
#include	"settings-handler.h"
#include	"position-handler.h"

#define	TECHDATA_SETTING	"techWindowSettings"

#define	DARK	"#00007f"
#define	WHITE	"#ffffff"

#define FRAMEDUMP_BUTTON        QString ("framedumpButton") 
#define AUDIODUMP_BUTTON        QString ("audiodumpButton")

		techWindow::techWindow	(RadioInterface *mr,
	                                 QSettings	*s,
	                                 RingBuffer<std::complex<int16_t>> *audioData):
	                                         superFrame (nullptr) {
	myRadioInterface	= mr;
	dabSettings		= s;
	this	-> audioData	= audioData;

        setupUi (this);
	QString settingsHeader	= TECHDATA_SETTING;

	setPositionAndSize (dabSettings, this, TECHDATA_SETTING);

//	we start being hidden, the radio decides whether we are visible
//	or not
	this	-> hide ();
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

	connect (framedumpButton, &smallPushButton::rightClicked,
                 this, &techWindow::colorFramedumpButton);
        connect (audiodumpButton, &smallPushButton::rightClicked,
                 this, &techWindow::colorAudiodumpButton); 

	connect (audiodumpButton, &QPushButton::clicked,
	         mr, &RadioInterface::handleAudiodumpButton);
	connect	(framedumpButton, &QPushButton::clicked,
	         mr, &RadioInterface::handleFramedumpButton);

	bitRateLabel	-> setStyleSheet ("color:red");
	uepField	-> setStyleSheet ("color:magenta");
	codeRate	-> setStyleSheet ("color:magenta");
	psLabel		-> setStyleSheet ("color:cyan");
	sbrLabel	-> setStyleSheet ("color:cyan");
	audiorateLabel	-> setStyleSheet ("color:red");
}

		techWindow::~techWindow	() {
	hide ();
	delete theAudioDisplay;
}

void	techWindow::storePosition	() {
	if (!isHidden ())
	   storeWidgetPosition (dabSettings, this, TECHDATA_SETTING);
}

void	techWindow::cleanUp	() {
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
	audiorateLabel		-> setText (QString::number (0));
}

void	techWindow::showServiceData	(audiodata *ad) {
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

void	techWindow::showFrameErrors	(int e) {
	QPalette p	= frameError_display -> palette();
	if (100 - 4 * e < 80)
	   p. setColor (QPalette::Highlight, Qt::red);
	else
	   p. setColor (QPalette::Highlight, Qt::green);

	frameError_display	-> setPalette (p);
	frameError_display	-> setValue (100 - 4 * e);
}

void	techWindow::showAacErrors	(int e) {
	QPalette p      = aacError_display -> palette();
        if (100 - 4 * e < 80)
           p. setColor (QPalette::Highlight, Qt::red);
        else
           p. setColor (QPalette::Highlight, Qt::green);
        aacError_display      -> setPalette (p);
        aacError_display      -> setValue (100 - 4 * e);
}

void	techWindow::showRsErrors		(int e) {
	QPalette p	= rsError_display -> palette();
	if (100 - 4 * e < 80)
	   p. setColor (QPalette::Highlight, Qt::red);
	else
	   p. setColor (QPalette::Highlight, Qt::green);
	rsError_display	-> setPalette (p);
	rsError_display	-> setValue (100 - 4 * e);
}

void	techWindow::showRsCorrections	(int c, int ec) {
	rsCorrections -> display (c);
	ecCorrections -> display (ec);
}

void	techWindow::updateFM		(std::vector<int> &fmFrequencies) {
	if (fmFrequencies. size () == 0)
	   return;
	showFm		(fmFrequencies);
}

void	techWindow::showFrameDumpButton	(bool b) {
	if (b)
	   framedumpButton	-> show ();
	else
	   framedumpButton	-> hide ();
}

void	techWindow::showServiceName	(const QString &s1, const QString &s2) {
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

void	techWindow::showServiceId		(int SId) {
QString text	= hextoString (SId);
	serviceIdDisplay -> setText (text);
}

void	techWindow::showStartAddress	(int sa) {
	startAddressDisplay	-> setText (QString::number (sa));
}

void	techWindow::showLength		(int l) {
	lengthDisplay		-> setText (QString::number (l));
}

void	techWindow::showSubChId		(int subChId) {
	subChIdDisplay		-> setText (QString::number (subChId));
}

void	techWindow::showLanguage		(int l) {
	language	-> setAlignment (Qt::AlignRight);
	language	-> setText (getLanguage (l));
}

void	techWindow::showUep		(int shortForm, int protLevel) {
	QString protL = getProtectionLevel (shortForm, protLevel);
	uepField	-> setText (protL);
}

void	techWindow::showCodeRate		(int shortForm, int protLevel) {
	codeRate -> setText (getCodeRate (shortForm, protLevel));
}

void	techWindow::showFm		(std::vector<int> &v) {
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

void	techWindow::audioDataAvailable	(int amount, int rate) {
std::complex<int16_t> buffer [1024];

	if (isHidden ()) {
	   audioData -> FlushRingBuffer ();
	   return;
	}

	while (audioData -> GetRingBufferReadAvailable () >= 1024) {
	   audioData -> getDataFromBuffer (buffer, 1024);
	   theAudioDisplay -> createSpectrum (buffer, 1024, rate);
	}
}

void    techWindow::colorFramedumpButton   ()      {
        setButtonColors (framedumpButton, FRAMEDUMP_BUTTON);
}

void    techWindow::colorAudiodumpButton   ()      {
        setButtonColors (audiodumpButton, AUDIODUMP_BUTTON);
}

void	techWindow::setButtonColors	(QPushButton *b,
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

void	techWindow::framedumpButton_text	(const QString &text, int size) {
	QFont font	= framedumpButton -> font ();
	font. setPointSize (size);
	framedumpButton	-> setFont (font);
	framedumpButton	-> setText (text);
	framedumpButton	-> update ();
}

void	techWindow::audiodumpButton_text	(const QString &text, int size) {
	QFont font	= audiodumpButton -> font ();
	font. setPointSize (size);
	audiodumpButton	-> setFont (font);
	audiodumpButton	-> setText (text);
	audiodumpButton	-> update ();
}

void	techWindow::showRate	(int rate, bool ps, bool sbr) {
	if (!ps)
	   psLabel -> setText (" ");
	else {
	   psLabel -> setText ("ps");
	}
	if (!sbr)
	   sbrLabel -> setText ("  ");
	else {
	   sbrLabel -> setText ("sbr");
	}
	audiorateLabel	-> setText (QString::number (rate));
}

void	techWindow::showStereo	(bool b) {
	 if (b) {
	   stereoLabel	-> setStyleSheet ("QLabel {color : green}");
           stereoLabel  -> setText ("<i>stereo</i>");
        }
        else {
	   stereoLabel	-> setStyleSheet ("QLabel {color : white}");
           stereoLabel  -> setText ("      ");
	}
}

void	techWindow::showMissed	(int missed) {
	missedSamples	-> display (missed);
}

void	techWindow::hideMissed	() {
	missedLabel	-> hide ();
	missedSamples	-> hide ();
}

void	techWindow::isDABPlus	(bool b) {
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

void	techWindow::hideMissedLabel	() {
	missedLabel	-> hide ();
	missedSamples	-> hide ();
}
