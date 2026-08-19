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
#include	<QFrame>
#include	"tech-window.h"
#include	"radio.h"
#include	"audio-display.h"
#include	"ITU-tables.h"
#include	<QColorDialog>

#include	"settingNames.h"
#include	"settings-handler.h"
#include	"position-handler.h"

#define	TECHDATA_SETTING	"techWindowSettings"

#define	DARK	"#00007f"
#define	WHITE	"#ffffff"

#define FRAMEDUMP_BUTTON        QString ("framedumpButton") 
#define AUDIODUMP_BUTTON        QString ("audiodumpButton")

		techWindow::techWindow	(QFrame	* theFrame,
	                                 RadioInterface *mr,
	                                 QSettings	*s,
	                                 RingBuffer<std::complex<int16_t>> *audioData):
	                                         myFrame (theFrame) {
	myRadioInterface	= mr;
	dabSettings		= s;
	this	-> audioData	= audioData;

        setupUi (myFrame);
	QString settingsHeader	= TECHDATA_SETTING;

//	we start being hidden, the radio decides whether we are visible
//	or not
	myFrame	-> hide ();
	theAudioDisplay	= new audioDisplay (mr, audioScope, dabSettings);

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

	uepField	-> setStyleSheet ("font-weight: bold; color:magenta");
	codeRate	-> setStyleSheet ("font-weight: bold; color:magenta");
	bitRateLabel	-> setStyleSheet ("font-weight: bold; color:magenta");
	audiorateLabel	-> setStyleSheet ("font-weight: bold; color:magenta");
	psLabel		-> setStyleSheet ("font-weight: bold; color:magenta");
	sbrLabel	-> setStyleSheet ("font-weight: bold; color:magenta");
	frameLabel	-> setStyleSheet ("QLabel {font-weight: bold; color: red}");
	frameLabel	-> setText ("frame");
	rsLabel		-> setStyleSheet ("QLabel {font-weight: bold; color: red}");
	rsLabel		-> setText ("RS");
	aacLabel	-> setStyleSheet ("QLabel {font-weight: bold; color: red}");
	aacLabel	-> setText ("AAC");
	programName	-> setStyleSheet ("font-weight: bold; color: white");
	QFont font      = programName -> font ();
        font. setPointSize (13); 
        programName	-> setFont (font);

	serviceIdDisplay	-> setStyleSheet ("font-weight: bold; color: white");
	fmLabel		-> setStyleSheet ("QLabel {color : magenta}");
	startAddressDisplay	-> setStyleSheet ("font-weight: bold; color:white");
	lengthDisplay	-> setStyleSheet ("font-weight: bold; color:white");
	subChIdDisplay	-> setStyleSheet ("font-weight: bold; color:white");
	ASCTy			-> setStyleSheet ("font-weight: bold; color:white");
	language	-> setStyleSheet ("font-weight: bold; color:white");
}

		techWindow::~techWindow	() {
	myFrame	-> hide ();
	delete theAudioDisplay;
}

void	techWindow::storePosition	() {
}

void	techWindow::cleanUp	() {
	const QString ee ("-");
	programName		-> setText (ee);
	shortnameLabel		-> setText (ee);
	frameLabel		-> setText ("frame");
	frameLabel		-> setColor (S_RED);
	rsLabel			-> setText ("RS");
	rsLabel			-> setColor (S_RED);
	crcErrorLabel		-> setText ("error");
	crcErrorLabel		-> setColor (S_RED);
	aacLabel		-> setText ("AAC");
	aacLabel		-> setColor (S_RED);
	startAddressDisplay	-> setText (QString::number (0));
	lengthDisplay		-> setText (QString::number (0));
	subChIdDisplay		-> setText (QString::number (0));
	uepField		-> setText (ee);
	codeRate		-> setText (ee);
	ASCTy			-> setText (ee);
	language		-> setText (ee);
	audiorateLabel		-> setText (QString::number (0));
}

void	techWindow::showServiceData	(int tableNo, audiodata *ad) {
	programName		-> setText (ad -> serviceName);
	shortnameLabel		-> setAlignment (Qt::AlignRight);
	shortnameLabel		-> setText (ad -> shortName);
	showDetails (tableNo, ad);
}

void	techWindow::showServiceData	(int tableNo, audiodata *ad,
	                                        const QPixmap &p) {
	programName		-> setText (ad -> serviceName);
	int height		= 40; 
	int width =
	         static_cast<float>(p. width ()) / p. height () * height;
	shortnameLabel		-> setAlignment (Qt::AlignCenter);
	shortnameLabel		->
	                 setPixmap (p. scaled (width, height)); 
	showDetails (tableNo, ad);
}

void	techWindow::showDetails		(int tableNo, audiodata *ad) {
	showServiceId		(ad -> SId);
	showStartAddress	(ad -> startAddr);
	showLength		(ad -> length);
	showSubChId		(ad -> subchId);
	showUep			(ad -> shortForm, ad -> protLevel);
	showCodeRate		(ad -> shortForm, ad -> protLevel);
	showLanguage		(tableNo, ad -> language);
	showFm			(ad -> fmFrequencies);
	bitRateLabel		-> setText (QString::number (ad -> bitRate) + " k");
}

void	techWindow::showFrameErrors	(int e) {
//	frameLabel	-> setLabel ("frame");
	if (e > 8)
	   frameLabel	-> setColor (S_RED);
	else
	if (e > 3)
	   frameLabel	-> setColor (S_YELLOW);
	else
	   frameLabel	-> setColor (S_GREEN);
}

void	techWindow::showRsErrors		(int e) {
//	rsLabel	-> setLabel ("RS");
	if (e > 8)
	   rsLabel	-> setColor (S_RED);
	else
	if (e > 3)
	   rsLabel	-> setColor (S_YELLOW);
	else
	   rsLabel	-> setColor (S_GREEN);
}

void	techWindow::showcrcErrors		(int e) {
	crcErrorLabel	-> setLabel ("crc");
	if (e > 8)
	   crcErrorLabel	-> setColor (S_RED);
	else
	if (e > 3)
	   crcErrorLabel	-> setColor (S_YELLOW);
	else
	   crcErrorLabel	-> setColor (S_GREEN);
}
	
void	techWindow::showAacErrors	(int e) {
//	aacLabel	-> setLabel ("AAC");
	if (e > 8)
	   aacLabel	-> setColor (S_RED);
	else
	if (e > 3)
	   aacLabel	-> setColor (S_YELLOW);
	else
	   aacLabel	-> setColor (S_GREEN);
}

void	techWindow::showMissed	(int missed) {
	audioLabel_text	-> setLabel	("Audio");
	if (missed > 90)
	   audioLabel_text	-> setColor (S_GREEN);
	else
	if (missed > 70)
	   audioLabel_text	-> setColor (S_YELLOW);
	else
	   audioLabel_text	-> setColor (S_RED);
}

void	techWindow::showRsCorrections	(int c, int ec) {
//	rsLabel_text	-> setStyleSheet ("QLabel {color: lightgreen}");
//	ecLabel_text	-> setStyleSheet ("QLabel {color: lightgreen}");
//	rsLabel_data	-> setStyleSheet ("QLabel {color: lightgreen}");
//	ecLabel_data	-> setStyleSheet ("QLabel {color: lightgreen}");
//	rsLabel_data	-> setText (QString::number (c));
//	ecLabel_data	-> setText (QString::number (ec));
}

void	techWindow::hideMissed	() {
	audioLabel_text	-> hide ();
}

void	techWindow::updateFM		(const std::vector<uint32_t> &fmFrequencies) {
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

static
QString hextoString (int v) {
QString res;
        for (int i = 0; i < 4; i ++) {
           uint8_t t = (v & 0xF000) >> 12;
           QChar c = t <= 9 ? (char)('0' + t) : (char) ('A' + t - 10);
           res. append (c); 
           v <<= 4;
        }
        return "0x" + res;
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

void	techWindow::showLanguage		(int table, int l) {
	language	-> setAlignment (Qt::AlignRight);
	language	-> setText (getLanguage (table, l));
}

void	techWindow::showUep		(int shortForm, int protLevel) {
	QString protL = getProtectionLevel (shortForm, protLevel);
	uepField	-> setText (protL);
}

void	techWindow::showCodeRate		(int shortForm, int protLevel) {
	codeRate -> setText (getCodeRate (shortForm, protLevel));
}

void	techWindow::showFm		(const std::vector<uint32_t> &v) {
	if (v. size () == 0) {
	   fmFrequency	-> hide ();
	   fmLabel	-> hide ();
	}
	else {
	   fmLabel	-> setStyleSheet ("QLabel {color : magenta}");
	   fmLabel	-> show ();
           fmFrequency	-> show ();	
	   QString f;
	   int teller	= 0;
//	for now there is room for up to 3 freqyencies
	   for (auto freq: v) {
	      if (++teller > 3)
	         break;			// for now
              f. append (QString::number (((float)freq) / 1000.0, 'f', 1) + " ");
	   }
           f. append (" MHz");
           fmFrequency        -> setText (f);
	}
}

void	techWindow::audioDataAvailable	(int amount, int rate) {
std::complex<int16_t> buffer [1024];

	(void)amount;
	if (myFrame -> isHidden ()) {
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

void	techWindow::framedumpButton_text	(const QString &text,
	                                                        int size) {
	QFont font	= framedumpButton -> font ();
	font. setPointSize (size);
	framedumpButton	-> setFont (font);
	framedumpButton	-> setText (text);
	framedumpButton	-> update ();
}

void	techWindow::audiodumpButton_text	(const QString &text,
	                                                       int size) {
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
//	   psLabel	-> setStyleSheet ("font-weight: bold; color:magenta");
	   psLabel -> setText ("ps");
	}
	if (!sbr) {
	   sbrLabel -> setText ("  ");
	}
	else {
//	   sbrLabel	-> setStyleSheet ("font-weight: bold; color:magenta");
	   sbrLabel	-> setText ("sbr");
	}
	audiorateLabel	-> setText (QString::number (rate / 1000) + "k");
}

void	techWindow::showStereo	(bool b) {
	(void)b;
}


void	techWindow::isDABPlus	(bool b) {
	if (b) {	// yes it is DAB+

	   ASCTy	-> setText ("DAB+");
	   frameLabel		-> show ();
	   rsLabel		-> show ();
	   aacLabel		-> show ();
	   audioLabel_text	-> show ();
	}
	else {
	   ASCTy	-> setText ("DAB");
	   frameLabel		-> hide ();
	   rsLabel		-> hide ();
	   aacLabel		-> hide ();
	   audioLabel_text	-> hide ();
	}
}
