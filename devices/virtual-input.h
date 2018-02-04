#
/*
 *    Copyright (C) 2014 .. 2017
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
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	We have to create a simple virtual class here, since we
 *	want the interface with different devices (including  filehandling)
 *	to be transparent
 */
#ifndef	__VIRTUAL_INPUT__
#define	__VIRTUAL_INPUT__

#include	<stdint.h>
#include	"dab-constants.h"
#include	<QObject>

class	virtualInput: public QObject {
public:
			virtualInput 	(void);
virtual			~virtualInput 	(void);
virtual		void	setVFOFrequency	(int32_t);
virtual		int32_t	getVFOFrequency	(void);
virtual		int32_t	defaultFrequency(void);
virtual		bool	restartReader	(void);
virtual		void	stopReader	(void);
virtual		int32_t	getSamples	(std::complex<float> *, int32_t);
virtual		int32_t	Samples		(void);
virtual		void	resetBuffer	(void);
virtual		int16_t	bitDepth	(void) { return 10;}
virtual		int32_t	getOffset	(void);
virtual		void	setOffset	(int32_t);
//
protected:
		int32_t	lastFrequency;
	        int32_t	vfoOffset;
	        int	theGain;
		int32_t	coarseOffset;
};
#endif

