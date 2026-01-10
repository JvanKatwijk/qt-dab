#
/*
 *    Copyright (C)  2016, 2026
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
#include	<stdio.h>
#include	<stdint.h>
#include	<QObject>
#include	<QScrollArea>
#include	<QTableWidget>
#include	<QTableWidgetItem>
#include	<QFile>
#include	<QDataStream>
#include	"dab-constants.h"

class	QSettings;
class	RadioInterface;

#define	SHOW_ENSEMBLE	0
#define	SHOW_PRESETS	1

class	ensembleHandler: public QTableWidget {
Q_OBJECT
public:
		ensembleHandler		(RadioInterface *parent,
	                                 QSettings *ensembleSettings,
	                                 const QString & favoritesFile);
		~ensembleHandler	();
	void	reset			();
	bool	addToEnsemble		(serviceId &);
	bool	alreadyIn		(serviceId &);
	void	remove			(const QString &);
	void	addFavoriteFromScanList	(const QString &);
	void	reportStart		(const QString &);

	void	handleScheduledSelect	(const QString &, const QString &);
	QStringList	getSelectables	();
	int	getServiceCount		();
	void	selectPrevService	();
	void	selectNextService	();
	void	selectCurrentItem	();
	void	setMode			(bool);
	int	getShowMode		();
	void	setShowMode		(int);

	int	nrFavorites		();
	bool	hasFavorite		(const QString &);
	uint16_t extract_SId		(const QString &);
	QString	extractName		(uint32_t);
	int	serviceOrder;
private	slots:
	void	clickOnService	(int, int);
public slots:
	void	handleFontSelect	();
	void	handleFontColorSelect	();
	void	handleFontSizeSelect	(int);
	void	handleRightMouseClick	(const QString &);
	void	setServiceOrder		(int);
private:
	QFont		normalFont;
	QFont		markedFont;
	QFont		channelFont;
	QSettings	*ensembleSettings;
	QString		favFile;
	struct	service {
	   QString	name;
	   QString	channel;
	   bool		selected;
	};

	std::vector<serviceId>	ensembleList;
	std::vector<struct service>	favorites;
	uint8_t	ensembleMode;
	bool	handlePresets;
	QString	fontColor;
	void	clearTable		();
	void	unSelect		();
	void	setFont			(int, int);
	void	loadFavorites		(const QString &);
	void	storeFavorites		(const QString &);
	void	updateList		();
	void	add_to_favorites	(const QString &, const QString &);
	void	remove_from_favorites	(const QString &);

	int	inEnsembleList		(const QString &);
	int	inFavorites		(const QString &);
	void	addRow			(const QString &, const QString &,
	                                 bool, int);
std::vector<serviceId>
		insert			(std::vector<serviceId> &,
	                                 const serviceId &, int);
signals:
	void	selectService		(const QString &, const QString &);
	void	start_background_task	(const QString &);
};

