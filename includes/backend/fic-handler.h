#
/*
 *    Copyright (C) 2013 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
 *
 */
#
/*
 * 	FIC data
 */
#ifndef	__FIC_HANDLER__
#define	__FIC_HANDLER__

#include	<stdio.h>
#include	<stdint.h>
#include	"viterbi-768.h"
#include	<QObject>
#include	"fib-processor.h"
#include	<QMutex>

class	RadioInterface;
class	mscHandler;
class	dabParams;

class ficHandler: public QObject, public viterbi_768 {
Q_OBJECT
public:
		ficHandler		(RadioInterface *);
		~ficHandler		(void);
	void	process_ficBlock	(int16_t *, int16_t);
	void	setBitsforMode		(uint8_t);
	void	clearEnsemble		(void);
	bool	syncReached		(void);
	int16_t	get_ficRatio		(void);
	uint8_t	kindofService		(QString &);
	void	dataforDataService	(QString &, packetdata *);
	void	dataforAudioService	(QString &, audiodata *);

	DSPCOMPLEX get_coordinates	(int16_t, int16_t, bool *);
	int16_t	mainId			(void);
private:
	void		process_ficInput	(int16_t *, int16_t);
	int8_t		*PI_15;
	int8_t		*PI_16;
	uint8_t		*bitBuffer_in;
	uint8_t		*bitBuffer_out;
	int16_t		*ofdm_input;
	dabParams	*params;
	int16_t		index;
	int16_t		BitsperBlock;
	int16_t		ficno;
	int16_t		ficBlocks;
	int16_t		ficMissed;
	int16_t		ficRatio;
	uint16_t	convState;
	QMutex		fibProtector;
	fib_processor	fibProcessor;
	uint8_t		PRBS [768];
	uint8_t		shiftRegister [9];
signals:
	void		show_ficSuccess	(bool);
};

#endif


