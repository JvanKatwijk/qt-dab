#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the sdrplayDab program
 *
 *    sdrplayDab is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    sdrplayDab is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with sdrplayDab; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef	__DEVICE_HANDLER__
#define	__DEVICE_HANDLER__

#include	<stdint.h>
#include	<QThread>
#include	"dab-constants.h"

class	dabProcessor;

class	deviceHandler: public QThread {
Q_OBJECT
public:
			deviceHandler 	(void);
virtual			~deviceHandler 	(void);
virtual		bool	restartReader	(int32_t);
virtual		void	stopReader	(void);
virtual		int32_t	getVFOFrequency	(void);
virtual		void	resetBuffer	(void);
virtual		int16_t	bitDepth	(void) { return 10;}
virtual		void	show		(void) {}
virtual		void	hide		(void) {}
virtual		bool	isHidden	(void) {return false;}
virtual		QString	deviceName	(void);
public	slots:
void		set_Values		(int, float, float);
protected:
virtual		void	handle_Value	(int, float, float);
		dabProcessor	*base;
};
#endif

