#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
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

#ifndef __ELAD_HANDLER__
#define	__ELAD_HANDLER__

#include	<QObject>
#include	<QFrame>
#include	<QFileDialog>
#include	"dab-constants.h"
#include	"virtual-input.h"
#include	"ringbuffer.h"
#include	"ui_widget.h"
#include	<libusb-1.0/libusb.h>

class	QSettings;
class	eladWorker;
class	eladLoader;
typedef	std::complex<float>(*makeSampleP)(uint8_t *);

class	eladHandler: public virtualInput, public Ui_Form {
Q_OBJECT
public:
		eladHandler		(QSettings *);
		~eladHandler		(void);
	void	setVFOFrequency		(int32_t);
	int32_t	getVFOFrequency		(void);
	bool	legalFrequency		(int32_t);
	int32_t	defaultFrequency	(void);

	bool	restartReader		(void);
	void	stopReader		(void);
	int32_t	getSamples		(std::complex<float> *, int32_t);
	int32_t	Samples			(void);
	void	resetBuffer		(void);
	int32_t	getRate			(void);
	int16_t	bitDepth		(void);
private	slots:
	void	setGainReduction	(void);
	void	setOffset		(int);
	void	setFilter		(void);
public slots:
	void	show_eladFrequency	(int);
	void	show_iqSwitch		(bool);
private:
	QSettings	*eladSettings;
	bool		deviceOK;
	eladLoader	*theLoader;
	eladWorker	*theWorker;
	RingBuffer<std::complex<float>>	*_I_Buffer;
	QFrame		*myFrame;
	int32_t		vfoFrequency;
	int32_t		vfoOffset;
	int		gainReduced;
	int		localFilter;
	uint8_t		conversionNumber;
	int16_t		iqSize;
};
#endif

