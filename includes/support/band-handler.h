#
/*
 *    Copyright (C) 2016 .. 2023
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

#pragma once

#include	<cstdint>
#include	<QComboBox>
#include	<QObject>
#include	<QString>
#include	<QSettings>
#include	<QTableWidget>
#include	<QFile>
#include	<QtXml>
#include	<stdio.h>
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
	void    saveSettings	();
	void	setupChannels	(QComboBox *s, uint8_t band);
	void	setup_skipList	(const QString &);
	int32_t Frequency	(QString Channel);
	int	firstChannel	();
	int	nextChannel	(int);
	int	prevChannel	(int);
	void	show		();
	void	hide		();
	bool	isHidden	();

public slots:
	void	cellSelected	(int, int);
private:
	QSettings		*dabSettings;
	QString			fileName;
	int			lastOf			(dabFrequencies *);
	dabFrequencies		*selectedBand;
	QTableWidget		theTable;
	void			default_skipList	();
	void			file_skipList		(const QString &);
	void			updateEntry		(const QString &);
};

