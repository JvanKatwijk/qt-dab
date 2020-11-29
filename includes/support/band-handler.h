#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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

#ifndef	__BANDHANDLER__
#define	__BANDHANDLER__
#include	<cstdint>
#include	<QComboBox>
#include	<QObject>
#include	<QString>
#include	<QSettings>
#include	<QTableWidget>
//
//	a simple convenience class
//


typedef struct {
	QString key;
	int	fKHz;
	bool	skip;
} dabFrequencies;
class bandHandler: public QObject {
Q_OBJECT
public:
		bandHandler	(const QString &, QSettings *);
		~bandHandler	();
	void	setupChannels	(QComboBox *s, uint8_t band);
	int32_t Frequency	(QString Channel);
	int	nextChannel	(int);
	int	prevChannel	(int);
	void	show		();
	void	hide		();
	bool	isHidden	();
	void    saveSettings	();

public slots:
	void	cellSelected	(int, int);
private:
	QSettings		*dabSettings;
	int	lastOf		(dabFrequencies *);
	dabFrequencies		*selectedBand;
	QTableWidget		theTable;
};
#endif

