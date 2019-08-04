#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB (formerly SDR-J, JSDR).
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
#ifndef	__ENSEMBLE_PRINTER__
#define	__ENSEMBLE_PRINTER__
//
//	a simple convenience class to print - whenever the button is
//	pressed - the content of the ensemble with relevant data.
//	The output can be viewed (a.o) in LibreOfficeCalc
#include	<QString>
#include	"dab-constants.h"
#include	<cstdint>
#include	<cstdio>

class	dabProcessor;

class	ensemblePrinter {
public:
		ensemblePrinter();
		~ensemblePrinter();
	void	showEnsembleData	(QString,
	                                 int32_t,
	                                 QString,	// time
	                                 QStringList,
	                                 dabProcessor *,
	                                 FILE *);
private:
	QString	code_to_string		(uint8_t, uint8_t);
};

#endif
