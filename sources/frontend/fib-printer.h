#
/*
 *    Copyright (C)  2015 .. 2025
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


#include	<QString>
#include	<QStringList>

#include	"dab-constants.h"
#include	"ensemble.h"
#include	"fib-config.h"
class	fibPrinter {
fibConfig	*currentConfig;
ensemble	*theEnsemble;
public:
	fibPrinter (fibConfig *currentConfig, ensemble *theEnsemble);
	~fibPrinter ();
	
QStringList	basicPrint		();
int		scanWidth		();
private:
QString		serviceName		(int index);
QString		serviceIdOf		(int index);
QString		subChannelOf 		(int index);
QString		startAddressOf 		(int index);
QString		lengthOf 		(int index);
QString		protLevelOf		(int index);
QString		codeRateOf 		(int index);
QString		bitRateOf		(int index);
QString		dabType 		(int index);
QString		languageOf 		(int index);
QString		programTypeOf		(int index);
QString		fmFreqOf		(int index);
QString		appTypeOf		(int index);
QString		FEC_scheme		(int index);
QString		packetAddress		(int index);
QString		DSCTy			(int index);
QString		audioHeader		();
QString		audioData		(int index);
QString		packetHeader		();
QString		packetData		(int index);
};

