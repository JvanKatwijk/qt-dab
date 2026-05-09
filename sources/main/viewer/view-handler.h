#
/*
 *    Copyright (C)  2026
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
#include	<QObject>
#include	<QScrollArea>
#include	<QTableWidget>
#include	<QTableWidgetItem>
#include	<QFile>
#include	<QDataStream>
#include	<QSettings>
#include	<QTimer>
#include	<stdio.h>
#include	<stdint.h>
#include	"dab-constants.h"
#include	"ui_viewarea.h"
#include	"channel-descriptor.h"
#include	"database.h"
#include	"service-descriptor.h"

#define	PREV_SERVICE		QString ("prevService")
#define NEXT_SERVICE		QString ("nextService")

class	RadioInterface;

class serviceViewer : public QObject, public Ui_viewArea {
Q_OBJECT
public:
	serviceViewer	(const QString &fileName,
	                 RadioInterface	*theRadio,
	                 const QStringList &channels,
	                 QSettings	*serviceSettings,
	                 QFrame *theFrame);
	~serviceViewer		();
void	startMode		(int, const QString &, int);
void	startMode		(int, int);
QString	currentChannel		();
int	getMode			();
void	set_channelIndex	(const QString &);
void	addService		(const descriptorType &);
void	remove			(const QString &, const QString &);
void	reportService		(const QString &);
void	reportService		(const QString &, const QString &);
QString	extractName		(uint32_t);
void	clearAll		();
void	saveName		(const QString &);
QStringList	getSelectables	();
void	set_ensembleId		(const QString &, int);
void	set_countryName		(const QString &);
void	clear_ensembleId	();
QTableWidget	*theTable;
private:	
	RadioInterface	*theRadio;
	void		startSession		(int, int);
	void		insert			(const serviceDescriptor &sd);
	void		set_channelIndex	(int);
	void		set_Colors		();
//	void		addService		(const serviceDescriptor &);
public slots:
	void		clickOnService		(int, int);
	void		handle_channelSelector	(const QString &);
	void		handle_nextService	();
	void		handle_prevService	();
	void		handle_nextChannel	();	
	void		handle_prevChannel	();
	void		handle_modeSwitcher	();

	void		handleFontSelect	();
	void		handleFontColorSelect	();
	void		handleFontSizeSelect	(int);
	void		handleRightMouseClick	(const QString &);
	void		setServiceOrder		(int);	// dummy
signals:
	void		setChannel		(const QString &channel);
	void		selectService		(const QString &,
	                                         const QString &);
	void		start_background_task	(const QString &);	// dummy
	void		reduceButtons		(bool);
private:
	serviceBase	theDataBase;
	std::vector<serviceDescriptor> displayList;
	void		clearTable		();
	QSettings	*viewSettings;
	QString		fileName;
	QString		defaultName;
	QSettings	*serviceSettings;
	QFrame		*theFrame;
	int		serviceOrder;
	int		theMode;
	int		currentService;
	int		scrollPos;
	void		show_displayList	();
	void		updateFonts	();
	QFont		normalFont;
        QFont		markedFont;
        QFont		channelFont;
	void		mark		(int);
	void		unmark		(int);

	QTimer		updateTimer;

	int		locate		(serviceDescriptor &sd);

	void	color_prevService	();
	void	color_nextService	();
	void	setButtonColors		(QPushButton *b,
                                         const QString &buttonName);

};

