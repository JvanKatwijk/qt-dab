#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  qt-dab program
 *
 *    qt-dab is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    qt-dab is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef	__PRESET_HANDLER__
#define	__PRESET_HANDLER__
#include	<QString>
#include	<QtXml>
#include	<QFile>
#include	<vector>
#include        <QWidget>
#include        <QScrollArea>
#include        <QTableWidget>
#include        <QStringList>
#include        <QTableWidgetItem>
#include        <QObject>
#include        <QString>

class   RadioInterface;

typedef struct {
	QString serviceName;
	QString channel;
} presetData;

class	presetHandler: public QObject {
Q_OBJECT
public:
		presetHandler	(RadioInterface *, QString);
		~presetHandler	();
	int	nrItems		();
	void	item		(int, presetData *);
	bool	item		(QString, presetData *);
	void	update		(presetData *);
	void	show		();	
	void	hide		();
private slots:
	void	tableSelect	(int, int);
	void	removeRow	(int, int);
signals:
	void	select_presetService	(QString, QString);
private:
	void		loadTable	();
	void		addRow		(const QString &name,
	                                 const QString &channel);
	bool		inPresets	(presetData *);
	void		writeFile	();
	RadioInterface	*radio;
	QScrollArea	*myWidget;
        QTableWidget    *tableWidget;

	QString	fileName;
	std::vector<presetData> presets;
};

#endif
