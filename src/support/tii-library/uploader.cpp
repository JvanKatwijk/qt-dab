#
/*
 *    Copyright (C)  2015, 2023
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

#include	"uploader.h"
#include	<stdio.h>

static inline
bool    isValid (QChar c) {
        return c. isLetter () || c. isDigit () || (c == '-');
}

	uploader::uploader	() {
	throw 1;
}

	uploader::~uploader	() {
}

QString	uploader::fileName	(const QString  &ensembleName,
	                         uint32_t Eid,
	                         const QString &channel) {
QString theTime	= QDateTime::currentDateTime (). toString ();

	QString res =  "Qt-DAB-" + QString::number (Eid, 16) + "-" +
	                channel + "-" + theTime + ".csv";
	for (int i = 0; i < res. length (); i ++)
	   if (!isValid (res. at (i)))
	      res. replace (i, 1, '-');
	return res;
}

bool	uploader::loadUp (const QString  &ensembleName,
	                  uint32_t Eid, const QString &channel,
	                    const QString & content) {
	(void)ensembleName;
	(void)Eid;
	(void)channel;
	(void)content;
	return false;
}

