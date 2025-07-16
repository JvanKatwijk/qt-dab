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
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma once

#include	<QString>
#include	<QtXml>
#include	<QFile>
#include	<vector>
#include        <QStringList>
#include        <QObject>
#include        <QString>
#include	<QListView>
#include        <QStringListModel>
#include        <QModelIndex>

class   RadioInterface;
class	QComboBox;

typedef struct presetData_ {
	QString serviceName;
	QString channel;
} presetData;

class	presetHandler: public QListView {
Q_OBJECT
public:
		presetHandler	(RadioInterface *,
	                            const QString &);
		~presetHandler	();
	void	addElement	(const QString &, const QString  &);
	void	addElement	(const QString &);
	void	removeElement	(const QString &);
	void	clearPresetList	();
	int	nrElements	();
	QString	candidate	(int i);
public	slots:
	void	selectElement	(QModelIndex);
signals:
	void	handlePresetSelect	(const QString &, const QString &);
private:
	RadioInterface	*radio_p;
	std::vector<presetData> thePresets;
	QStringList	presetList;
	QStringListModel	displayList;
	QString	fileName;
};

