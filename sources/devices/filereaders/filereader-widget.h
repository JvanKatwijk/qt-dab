#
/*
 *    Copyright (C) 2016 .. 2024
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

#pragma once

#include	<QLabel>
#include	<QProgressBar>
#include	<QLCDNumber>
#include	<QHBoxLayout>
#include	<QVBoxLayout>

class	filereaderWidget {
public:
	QLabel		*titleLabel;
	QLabel		*nameofFile;
	QProgressBar	*fileProgress;
	QLCDNumber	*currentTime;
	QLabel		*seconds;
	QLCDNumber	*totalTime;
	QLabel		*deviceLabel;
	QLabel		*theDevice;
	QLabel		*typeLabel;
	QLabel		*typeOfFile;
	QLabel		*amountLabel;
	QLabel		*sampleCount;
	QLabel		*frequencyLabel;
	filereaderWidget () {}
	~filereaderWidget () {}

void	setupUi		(QWidget *qw) {
	titleLabel	= new QLabel ("Playing pre-recorded file");
	nameofFile	= new QLabel ();
	deviceLabel	= new QLabel ();
	theDevice	= new QLabel ();
	typeLabel	= new QLabel ("filetype ");
	typeOfFile	= new QLabel ();
	amountLabel	= new QLabel (" nr samples ");
	sampleCount	= new QLabel ();
	frequencyLabel	= new QLabel	();
	fileProgress	= new QProgressBar ();
	currentTime	= new QLCDNumber	();
	currentTime	->  setFrameShape (QFrame::NoFrame);
	currentTime	->  setSegmentStyle (QLCDNumber::Flat);

	seconds		= new QLabel ("seconds of");
	totalTime	= new QLCDNumber	();

	totalTime	->  setFrameShape (QFrame::NoFrame);
	totalTime	->  setSegmentStyle (QLCDNumber::Flat);
	QHBoxLayout *line_2	= new QHBoxLayout ();
	line_2		-> addWidget	(deviceLabel);
	line_2		-> addWidget (theDevice);
	QHBoxLayout *line_3	= new QHBoxLayout ();
	line_3		-> addWidget (typeLabel);
	line_3		-> addWidget (typeOfFile);
	line_3		-> addWidget (amountLabel);
	line_3		-> addWidget (sampleCount);
	line_3		-> addWidget (frequencyLabel);
	QHBoxLayout *bottom	= new QHBoxLayout ();
	bottom		-> addWidget (currentTime);
	bottom		-> addWidget (seconds);
	bottom		-> addWidget (totalTime);

	QVBoxLayout *base	= new QVBoxLayout ();
	base		-> addWidget (titleLabel);
	base		-> addWidget (nameofFile);
	base		-> addItem	(line_2);
	base		-> addItem	(line_3);
	base		-> addWidget (fileProgress);
	base		-> addItem   (bottom);
	
	qw		-> setLayout (base);
}
};

