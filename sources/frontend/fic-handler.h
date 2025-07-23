#
/*
 *    Copyright (C) 2016 .. 2023
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
#
/*
 * 	FIC data
 */
#pragma once

#include	<QObject>
#include	<QMutex>
#include	<cstdio>
#include	<cstdint>
#include	<vector>
#include	<atomic>
#include	"viterbi.h"
#include	"dab-params.h"
#include	"fib-decoder.h"


class	RadioInterface;
class	dabParams;

class ficHandler: public fibDecoder {
Q_OBJECT
public:
		ficHandler		(RadioInterface *, uint8_t, uint8_t);
		~ficHandler		();
	void	processFICBlock		(std::vector<int16_t> &, int16_t);
	void	stop			();
	void	restart			();
	void	startFICDump		(FILE *);
	void	stopFICDump		();
	void	getFIBBits		(uint8_t *, bool *);
	int	getFICQuality		();
private:
	dabParams	params;
	viterbi		myViterbi;
//	viterbiSpiral	myViterbi;
	uint8_t		hardBits	[768];
        int16_t		ficInput	[2304];
	uint8_t		punctureTable	[3072 + 24];
	uint8_t		fibBits		[4 * 768];
	bool		ficValid	[4];
	uint8_t		PRBS		[768];
	bool		processFICInput	(int16_t);
	int16_t		index;
	int16_t		BitsperBlock;
	int16_t		ficno;
	uint16_t	convState;
	FILE		*ficDumpPointer;
	QMutex          ficLocker;
	int		ficBlocks;
	int		ficErrors;
	int		ficBits;
	int		ficPointer;
	std::atomic<bool> running;
	int		successRatio;
	int		fibCounter;
	uint16_t	starter;
signals:
	void		showFICQuality	(int, int);
	void		showFICBER	(float);
};

