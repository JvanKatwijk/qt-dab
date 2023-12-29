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
 *    along with dab-scanner; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#pragma once
#include        <cstdint>
#include        <QObject>
#include        <QString>
#include        <QSettings>
#include        <QTableWidget>
#include        <QFile>
#include        <QtXml>
#include        <stdio.h>

typedef struct {
        QString key;
        int     fKHz;
        bool    skip;
} dabFrequencies;

class	QSettings;

class	skiptableHandler: public QObject {
Q_OBJECT
public:
		skiptableHandler	(QSettings *);
		~skiptableHandler	();
	void	setup_skipTable		(dabFrequencies *);
	void	load_skipTable		(const QString &);
	void	save_skipTable		(const QString &);

	void	show_skipTable		();
	void	hide_skipTable		();
	bool	isHidden		();
private:
	QTableWidget	theTable;
	QSettings	*dabSettings;
	dabFrequencies	*selectedBand;
	QString		skipFile;
	
	void		updateEntry	(const QString &channel);

private slots:
	void	cellSelected	(int, int);
};


