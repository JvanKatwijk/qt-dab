#
/*
 *    Copyright (C) 2014 .. 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"dab-tables.h"
#include	"audio-descriptor.h"


	audioDescriptor::audioDescriptor (audiodata *ad):
	                                       myFrame (nullptr) {
        setupUi (&myFrame);
        myFrame. show();
	serviceLabel    -> setText
                      (QString().number (ad -> SId, 16). toUpper());
        QFont font      = serviceLabel -> font();
        font. setBold (true);
        serviceLabel    -> setFont (font);

	serviceName  -> setText (ad -> serviceName);
	subChannelId -> setText (QString::number (ad -> subchId));
	startAddress -> setText (QString::number (ad -> startAddr));
	Length       -> setText (QString::number (ad -> length));
	bitrate      -> setText (QString::number (ad -> bitRate));
	QString protL	= getProtectionLevel (ad -> shortForm,
	                                      ad -> protLevel);
	protectionLevel   -> setText (protL);
	dabType      -> setText (ad -> ASCTy == 077 ? "DAB+" : "DAB");
	Language     -> setText (getLanguage (ad -> language));
	programType  ->
	   setText (getProgramType (ad -> programType));
	if (ad -> fmFrequency == -1) {
	   fmLabel	-> hide();
	   fmFrequency	-> hide();
	}
	else {
	   fmLabel	-> show();
	   QString f	= QString::number (ad -> fmFrequency);
	   f. append (" Khz");
	   fmFrequency	-> setText (f);
	}
}

	audioDescriptor::~audioDescriptor() {
}

