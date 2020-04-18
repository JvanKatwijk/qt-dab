#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  Qt-DAB program
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

#ifndef	__PRESET_HANDLER__
#define	__PRESET_HANDLER__
#include	<QString>
#include	<QtXml>
#include	<QFile>
#include	<vector>
#include        <QStringList>
#include        <QObject>
#include        <QString>

class   RadioInterface;
class	QComboBox;

typedef struct presetData_ {
	QString serviceName;
	QString channel;
} presetData;

class	presetHandler: public QObject {
Q_OBJECT
public:
		presetHandler	(RadioInterface *);
		~presetHandler	();
	void	loadPresets	(QString, QComboBox *);
	void	savePresets	(QComboBox *);
private:
	RadioInterface	*radio;
	QString	fileName;
};

#endif
