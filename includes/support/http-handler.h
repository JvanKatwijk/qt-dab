#
/*
 *	Copyright (C) 2016 .. 2023
 *	Jan van Katwijk (J.vanKatwijk@gmail.com)
 *	Lazy Chair Computing
 *
 *	This file is part of Qt-DAB
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
#include	<thread>
#include	<atomic>
#include	<string>
#include	<vector>
#include	<complex>
#include	<mutex>
#include	<QString>
#include	<QSettings>
#include	"tii-mapper.h"
class	RadioInterface;

typedef struct {
	uint8_t	type;
	position coords;
	QString transmitterName;
	QString channelName;
	QString	dateTime;
	int	tiiValue;
	int	distance;
	int	azimuth;
	float	power;
	float	height;
} httpData;

class	httpHandler: public QObject {
Q_OBJECT
public:
		httpHandler	(RadioInterface *,
	                         const QString &mapPort,
	                         const QString &browserAddress,
	                         position	address,
	                         const QString &saveName,
	                         bool	autoBrowse,
	                         QSettings	*setings = nullptr);
		~httpHandler	();
	void	start		();
	void	stop		();
	void	run		();
	void	putData		(uint8_t, position);
	void	putData		(uint8_t	type,
	                         position	target,
	                         QString transmittername,
	                         QString channelName,
	                         QString dateTime,
	                         int ttiId,
	                         int distance,
	                         int azimuth,
	                         float power,
	                         float height);
private:
	QSettings		*dabSettings;
	FILE			*saveFile;
	QString			*saveName;
	RadioInterface		*parent;
	QString			mapPort;
	position		homeAddress;
	std::vector<httpData> transmitterVector;

#ifdef	__MINGW32__
	std::wstring	browserAddress;
#else
	std::string	browserAddress;
#endif
	std::atomic<bool>	running;
	std::thread	threadHandle;
	std::string     theMap		(position address);
	std::string	coordinatesToJson (std::vector<httpData> &t);
	std::vector<httpData>	transmitterList;
	std::mutex	locker;
	bool		autoBrowser_off;
signals:
	void		terminating	();
	void		setChannel	(const QString &);
};

