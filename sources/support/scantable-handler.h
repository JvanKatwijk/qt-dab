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
        bool    scan;
} dabFrequencies;

class	QSettings;

class	scantableHandler: public QObject {
Q_OBJECT
public:
		scantableHandler	(QSettings *);
		~scantableHandler	();
	void	setup_scanTable		(dabFrequencies *);
	void	load_scanTable		(const QString &);
	void	save_scanTable		(const QString &);

	void	clear_scanTable		();
	void	show_scanTable		();
	void	hide_scanTable		();
	bool	isHidden		();
private:
	QTableWidget	theTable;
	QSettings	*dabSettings;
	dabFrequencies	*selectedBand;
	void		updateEntry	(const QString &channel, bool val);

private slots:
	void	cellSelected	(int, int);
};


