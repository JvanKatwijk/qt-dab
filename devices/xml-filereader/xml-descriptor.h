#
/*
 *    Copyright (C) 2014 .. 2019
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
 *
 */

#ifndef	__XML_DESCRIPTOR__
#define	__XML_DESCRIPTOR__

#include <QtXml>

#include	<QString>
#include	<stdint.h>
#include	<stdio.h>

class Blocks	{
public:
			Blocks		() {}
			~Blocks		() {}
	int		blockNumber;
	int		nrElements;
	QString		typeofUnit;
	int		frequency;
	QString		modType;
};

class xmlDescriptor {
public:
	QString		deviceName;
	QString		deviceModel;
	QString		recorderName;
	QString		recorderVersion;
	QString		recordingTime;
	int		sampleRate;
	int		nrChannels;
	int		bitsperChannel;
	QString		container;
	QString		byteOrder;
	QString		iqOrder;
	int		nrBlocks;
	std::vector<Blocks> blockList;
			xmlDescriptor	(FILE *, bool *);
			~xmlDescriptor	();
	void		printDescriptor	();
	void		setSamplerate	(int sr);
	void		setChannels 	(int	nrChannels,
	             	                 int	bitsperChannel,
	                                 QString	ct,
	             	                 QString	byteOrder);
	void		addChannelOrder (int channelOrder,
	                                 QString Value);
	void		add_dataBlock (int currBlock,  int Count,
                                       int  blockNumber, QString Unit);
	void		add_freqtoBlock	(int blockno, int freq);
	void		add_modtoBlock (int blockno, QString modType);
};

#endif
