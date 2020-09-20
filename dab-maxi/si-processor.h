#
/*
 *    Copyright (C) 2013 .. 2017
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
#ifndef	__SI_PROCESSOR__
#define	__SI_PROCESSOR__

#include	<QString>
#include	<stdio.h>
#include	<stdint.h>
#include	<stdlib.h>
#include	<vector>

class	ensembleInfo;
class	serviceInfo;

class	siProcessor {
public:
		siProcessor		();
		~siProcessor		();
ensembleInfo	*process_SI		(uint8_t *, int);
private:
int		header			(int *pos);
ensembleInfo	*process_ensemble	(int *pos);
serviceInfo	process_service		(int *pos, int length);

int		process_serviceId	(int pos);
QString		process_serviceName	(int pos);
QString		process_mediaDescription (int pos);
QString		process_keywords	(int pos);
QString		longDescription		(int pos);

QString stringTable [30];
uint8_t	*data;
int	length;
};

#endif

