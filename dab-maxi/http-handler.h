#
/*
 *	Copyright (C) 2022
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

#ifndef	__HTTP_HANDLER_H
#define	__HTTP_HANDLER_H

#include	<QObject>
#include	<thread>
#include	<atomic>
#include	<string>
#include	<vector>
#include	<complex>
#include	<mutex>
#include	<QString>
class	RadioInterface;

typedef struct {
	std::complex<float> coords;
	QString transmitterName;
	QString channelName;
	int	distance;
	int	azimuth;
} httpData;

class	httpHandler: public QObject {
Q_OBJECT
public:
		httpHandler	(RadioInterface *,
	                         int port,
	                         std::complex<float> address,
	                         bool	autoBrowse,
	                         const QString &browserAddress);
		~httpHandler	();
	void	start		();
	void	stop		();
	void	run		();
	void	putData		(std::complex<float>target,
	                         QString transmittername,
	                         QString channelName,
	                         int distance, int azimuth);
private:
	RadioInterface		*parent;
	int		port;
	std::complex<float> homeAddress;
#ifdef	__MINGW32__
	std::wstring	browserAddress;
#else
	std::string	browserAddress;
#endif
	std::atomic<bool>	running;
	std::thread	threadHandle;
	std::string     theMap		(std::complex<float> address);
	std::string	coordinatesToJson (std::vector<httpData> &t);
	std::vector<httpData>	transmitterList;
	std::mutex	locker;
	bool		autoBrowse;
signals:
	void		terminating	();
};

#endif
