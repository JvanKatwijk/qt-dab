#
/*
 *    Copyright (C) 2024
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
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#pragma once

#include	<QWidget>
#include	<QScrollArea>
#include	<QTableWidget>
#include	<QTableWidgetItem>
#include	<QObject>
#include	<QSettings>
#include	<qwt_compass.h>
#include	"cacheElement.h"
class	RadioInterface;

class	dxDisplay:public QFrame {
Q_OBJECT
public:
		dxDisplay	(RadioInterface *, QSettings *);
		~dxDisplay	();
	void	setChannel	(const QString &);
	void	addRow		(cacheElement &theTransmitter, bool);
	void	addRow		(uint8_t, uint8_t, const QString &);
	void	cleanUp		();
	void	show		();
	void	hide		();
	int	nrRows		();
private:
	QScrollArea	*myWidget;
	QTableWidget	*tableWidget;
	QSettings	*dxSettings;
	QwtCompass	*theDial;
	QString		theChannel;
	void		dump	(FILE *);
};

	

