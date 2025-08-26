#
/*
 *    Copyright (C) 2014 .. 2023
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


#pragma once

#include	<QLabel>
#include	<QSpinBox>
#include	<QLCDNumber>
#include	<QComboBox>
#include	<QPushButton>
#include	<QHBoxLayout>
#include	<QVBoxLayout>
#include	<QCheckBox>

class	limeWidget {
public:
	QLabel		*titleLabel;
	QSpinBox	*gainSelector;
	QLCDNumber	*actualGain;
	QLabel		*gainLabel;
	QComboBox	*antennaList;
	QLabel		*antennaLabel;
	QLCDNumber	*underrunDisplay;
	QLabel		*underrunLabel;
	QLCDNumber	*overrunDisplay;
	QLabel		*overrunLabel;
	QLCDNumber	*fifoDisplay;
	QLabel		*fifoLabel;
	QPushButton	*dumpButton;
	QCheckBox	*filterSelector;
	QSpinBox	*filterDepth;
	QLabel		*nameOfDevice;
	QLabel		*frequencyLabel;

	limeWidget	() {}
	~limeWidget	() {}

void	setupUi		(QFrame *qw) {
	titleLabel	= new QLabel	("lime handler");
	nameOfDevice	= new QLabel	("deviceName");
	gainSelector	= new QSpinBox	();
	gainLabel	= new QLabel	("gain");
	actualGain	= new QLCDNumber ();
	antennaList	= new QComboBox	();
	antennaLabel	= new QLabel	("antennas");
	underrunDisplay	= new QLCDNumber ();
	underrunLabel	= new QLabel	("underruns");
	overrunDisplay	= new QLCDNumber ();
	overrunLabel	= new QLabel	("overruns");
	fifoDisplay	= new QLCDNumber ();
	fifoLabel	= new QLabel ("fifo");
	dumpButton	= new QPushButton	();
	filterSelector	= new QCheckBox	("filter");
	filterDepth	= new QSpinBox	();
	frequencyLabel	= new QLabel ();
	filterDepth	-> setRange (5, 25);
	dumpButton	-> setText ("Dump");
	actualGain	->  setFrameShape (QFrame::NoFrame);
        actualGain	->  setSegmentStyle (QLCDNumber::Flat);
	underrunDisplay	->  setFrameShape (QFrame::NoFrame);
        underrunDisplay	->  setSegmentStyle (QLCDNumber::Flat);
	overrunDisplay	->  setFrameShape (QFrame::NoFrame);
        overrunDisplay	->  setSegmentStyle (QLCDNumber::Flat);
	fifoDisplay	->  setFrameShape (QFrame::NoFrame);
        fifoDisplay	->  setSegmentStyle (QLCDNumber::Flat);

	QHBoxLayout *line2	= new QHBoxLayout ();
	line2		-> addWidget (gainSelector);
	line2		-> addWidget (gainLabel);
	line2		-> addWidget (actualGain);

	QHBoxLayout *line3	= new QHBoxLayout ();
	line3		-> addWidget (antennaList);
	line3		-> addSpacing (3);
	line3		-> addWidget (antennaLabel);
	line3		-> addWidget (frequencyLabel);

	QHBoxLayout *line4	= new QHBoxLayout ();
	line4		-> addWidget (underrunLabel);
	line4		-> addWidget (underrunDisplay);
	line4		-> addWidget (overrunLabel);
	line4		-> addWidget (overrunDisplay);
	
	QHBoxLayout *line5	= new QHBoxLayout ();
	line5		-> addWidget (fifoLabel);
	line5		-> addWidget (fifoDisplay);
	line5		-> addWidget (filterSelector);
	line5		-> addWidget (filterDepth);

	QHBoxLayout *line1	= new QHBoxLayout ();
	line1		-> addWidget (titleLabel);
	line1		-> addWidget (dumpButton);
	
	QVBoxLayout *base	= new QVBoxLayout ();
	base		-> addItem	(line1);
	base		-> addItem	(line2);
	base		-> addItem	(line3);
	base		-> addItem	(line4);
	base		-> addItem	(line5);
	base		-> addWidget	(nameOfDevice);

	qw	-> setLayout (base);
}
};


