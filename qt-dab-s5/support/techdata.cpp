#
/*
 *    Copyright (C)  2015, 2016, 2017, 2018, 2019, 2020, 2021
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
#include	"color-selector.h"


#define FRAMEDUMP_BUTTON        QString ("framedumpButton") 
#define AUDIODUMP_BUTTON        QString ("audiodumpButton")

		techData::techData	(RadioInterface *mr,
	                                 QSettings	*s,
	                                 RingBuffer<int16_t> *audioData):
	                                         myFrame (nullptr) {
	myRadioInterface	= mr;
	dabSettings		= s;
	this	-> audioData	= audioData;

        setupUi (&myFrame);
	dabSettings -> beginGroup ("techDataSettings");
        int x   = dabSettings -> value ("position-x", 100). toInt ();
        int y   = dabSettings -> value ("position-y", 100). toInt ();
	int wi	= dabSettings -> value ("width", 100). toInt ();
	int he	= dabSettings -> value ("height", 100). toInt ();
        dabSettings -> endGroup ();
        myFrame. move (QPoint (x, y));
	myFrame. resize (QSize (wi, he));

	formLayout -> setLabelAlignment (Qt::AlignLeft);
	myFrame. hide ();
	timeTable_button	-> hide ();
	the_audioDisplay	= new audioDisplay (mr, audio, dabSettings);

	dabSettings	-> beginGroup ("colorSettings");
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

	connect (framedumpButton, SIGNAL (rightClicked ()),
                 this, SLOT (color_framedumpButton ()));
        connect (audiodumpButton, SIGNAL (rightClicked ()),
                 this, SLOT (color_audiodumpButton ())); 

	connect	(framedumpButton, SIGNAL (clicked ()),
	         this, SIGNAL (handle_frameDumping ()));
	connect (audiodumpButton, SIGNAL (clicked ()),
	         this, SIGNAL (handle_audioDumping ()));

	connect (timeTable_button, SIGNAL (clicked ()),
	         this, SIGNAL (handle_timeTable ()));
}

		techData::~techData	() {
	dabSettings	-> beginGroup ("techDataSettings");
        dabSettings	-> setValue ("position-x", myFrame. pos (). x ());
        dabSettings	-> setValue ("position-y", myFrame. pos (). y ());
	dabSettings	-> setValue ("width", myFrame. width ());
	dabSettings	-> setValue ("height", myFrame. height ());
	dabSettings	-> endGroup ();
	myFrame. hide ();
	delete the_audioDisplay;
}

void	techData::cleanUp	() {
	programName		-> setText (QString (""));
	rsCorrections		-> display (0);
	frameError_display	-> setValue (0);
	rsError_display		-> setValue (0);
	aacError_display	-> setValue (0);
	bitrateDisplay		-> display (0);
	startAddressDisplay	-> display (0);
	lengthDisplay		-> display (0);
	subChIdDisplay		-> display (0);
	uepField		-> setText (QString (""));
	codeRate		-> setText (QString (""));
	ASCTy			-> setText (QString (""));
	language		-> setText (QString (""));
	motAvailable		-> 
	               setStyleSheet ("QLabel {background-color : red}");
	timeTable_button	-> hide ();
	audioRate		-> display (0);
}

void	techData::show_serviceData	(audiodata *ad) {
	show_serviceName	(ad -> serviceName);
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
	   timeTable_button	-> show ();
	else
	   timeTable_button	-> hide ();
}

void	techData::show_frameDumpButton	(bool b) {
	if (b)
	   framedumpButton	-> show ();
	else
	   framedumpButton	-> hide ();
}

void	techData::show_serviceName	(const QString &s) {
	programName -> setText (s);
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
int16_t buffer [amount];

	audioData -> getDataFromBuffer (buffer, amount);
	if (!myFrame. isHidden ())
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
colorSelector *selector;
int	index;

	selector		= new colorSelector ("button color");
	index			= selector -> QDialog::exec ();
	QString baseColor	= selector -> getColor (index);
	delete selector;
	if (index == 0)
	   return;
	selector		= new colorSelector ("text color");
	index			= selector	-> QDialog::exec ();
	QString textColor	= selector	-> getColor (index);
	delete selector;
	if (index == 0)
	   return;
	QString temp = "QPushButton {background-color: %1; color: %2}";
	b	-> setStyleSheet (temp. arg (baseColor, textColor));

	QString buttonColor	= buttonName + "_color";
	QString buttonFont	= buttonName + "_font";

	dabSettings	-> beginGroup ("colorSettings");
	dabSettings	-> setValue (buttonColor, baseColor);
	dabSettings	-> setValue (buttonFont, textColor);
	dabSettings	-> endGroup ();

//	fprintf (stderr, "%s -> %s, %s -> %s\n",
//	                buttonColor. toLatin1 (). data (),
//	                baseColor. toLatin1 (). data (),
//	                buttonFont. toLatin1 (). data (),
//	                textColor. toLatin1 (). data ());
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

void	techData::showRate	(int rate) {
	audioRate	-> display (rate);
}

void	techData::showMissed	(int missed) {
	missedSamples	-> display (missed);
}

void	techData::hideMissed	() {
	missedLabel	-> hide ();
	missedSamples	-> hide ();
}


