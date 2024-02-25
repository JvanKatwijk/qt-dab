#
/*
 *    Copyright (C) 2014 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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

#include	<cstdint>
#include	"dab-constants.h"
#include	"settingNames.h"
#include	"super-frame.h"

#include	<QThread>
//
//	We provide a simple interface to the devices. Note that
//	it is not just an abstract interface,
//	it provides a number of shared functions (like hide and show).
//	and - since it provides default interfaces for all functions -
//	there is no need for e.g. a file handling interface
//	to implement functions for gain or frequency
class	deviceHandler: public QThread {
Q_OBJECT
public:
			deviceHandler	();
virtual			~deviceHandler	();
virtual		bool	restartReader	(int32_t freq);
virtual		void	stopReader	();
virtual		int32_t	getSamples	(std::complex<float> *, int32_t);
virtual		int32_t	Samples		();
virtual		void	resetBuffer	();
virtual		int16_t	bitDepth	() { return 10;}
virtual		QString deviceName	();
virtual		bool	isFileInput	();
//
//	all derived classes are subject to visibility settings
//	performed by these functions
		bool	getVisibility	();
		void	setVisibility	(bool);
//
protected:
		superFrame	myFrame;
		int32_t	lastFrequency;
	        int	theGain;
signals:
		void	frameClosed 	();
};

