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

#include	<QWidget>
#include	<atomic>
#include	<QObject>
#include	<QScrollArea>
#include	<QTableWidget>
#include	<QStringList>
#include	<QTableWidgetItem>
#include	<QObject>
#include	<QString>
#include	<QByteArray>
#include	<QLabel>
#include	<QPushButton>
#include	<QCheckBox>
#include	<QComboBox>
#include	<QSpinBox>
#include	"findfilenames.h"
#include	"scantable-handler.h"

#include	"super-frame.h"

class	RadioInterface;
class	QSettings;

class	scanHandler: public superFrame {
Q_OBJECT
public:
		scanHandler	(RadioInterface *, QSettings *, const QString &);
		~scanHandler	();
	bool	isVisible	();
	void	clearTable	();
	void	addEnsemble	(const QString &channel, const QString &name);
	void	addTransmitters (const QStringList &, const QString &);
	void	addService	(const QString &);
	void	addText		(const QString &koptext);

	bool	scan_to_data	();
	bool	scan_single	();	
	bool	scan_continuous	();

	QString	getFirstChannel	();
	QString	getNextChannel	();	
	QString	getNextChannel	(const QString &);	
	int16_t	channelIndex	(const QString &);
	bool	active		();
	void	setStop		();
	FILE	*askFileName	();
	bool	dumpInFile	();

	QStringList	getChannelNames	();
	int32_t		Frequency	(const QString &);
	QString		getChannel	(int frequency);

	int		switchStayValue	();
private:
	scantableHandler	scanTable;
	QString		channel;
	findfileNames	filenameFinder;
	RadioInterface	*theRadio;
	QSettings	*dabSettings;
	QScrollArea	*myWidget;
	QTableWidget	*contentWidget;
	QPushButton	*startKnop;
	QPushButton	*stopKnop;
	QPushButton	*showKnop;
	QPushButton	*dumpDirKnop;
	QPushButton	*clearKnop;
	QPushButton	*defaultLoad;
	QPushButton	*defaultStore;
	QPushButton	*loadKnop;
	QPushButton	*storeKnop;
	QComboBox	*scanModeSelector;
	QCheckBox	*dumpChecker;
	QSpinBox	*switchStaySetting;

#ifndef	__MINGW32__
	dabFrequencies alternatives [128];
	dabFrequencies *load_extFile       (const QString &extFile);
#endif
	bool		no_scanTables;

	dabFrequencies	*selectedBand;
	std::atomic<bool>	scanning;
	int		currentRow;
	int		nrServices;
	int		totalServices;
	int16_t		addRow ();
	QLabel		*kopLine;

	int		currentChannel;
	int		scanMode;
	QString		scanFile;
private slots:

	void		handle_startKnop	();
	void		handle_stopKnop		();
	void		handle_scanMode		(int);
	void		handle_showKnop		();
	void		handle_clearKnop	();
	void		handle_defaultLoad	();
	void		handle_defaultStore	();
	void		handle_loadKnop		();
	void		handle_storeKnop	();
	void		handle_switchStaySetting (int newV);

	void		setDumpDir		();
signals:
	void		startScanning	();
	void		stopScanning	();
};

