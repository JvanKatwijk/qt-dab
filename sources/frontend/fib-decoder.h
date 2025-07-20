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
 *    along with Qt-TAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#pragma once
//
#include	<cstdint>
#include	<cstdio>
#include	<QObject>
#include	<QByteArray>
#include	"msc-handler.h"
#include	<QMutex>

class	RadioInterface;
class	ensemble;
class	fibConfig;

class	fibDecoder: public QObject {
Q_OBJECT
public:
			fibDecoder		(RadioInterface *);
			~fibDecoder		();

	void		clearEnsemble		();
	void		connectChannel		();
	void		disconnectChannel	();
	bool		syncReached		();

	uint16_t	getAnnouncing		(uint16_t);
	uint32_t	getSId			(int);
	uint8_t		serviceType		(int);
	int		getServiceComp		(const QString &);
	int		getServiceComp		(const uint32_t, const int);
	int		getServiceComp_SCIds	(const uint32_t, const int);
	bool		isPrimary		(const QString &);
	void		audioData		(const int, audiodata &);
	void		packetData		(const int, packetdata &);
	int		getNrComps		(const uint32_t);
	int		nrChannels		();
        uint8_t		get_ecc			();
	std::vector<int>	getFrequency	(const QString &);
	void		getChannelInfo		(channel_data *, const int);
	int32_t		getCIFcount		();	
	void		getCIFcount		(int16_t &, int16_t &);
	uint32_t	julianDate		();
	QStringList	basicPrint 		();
	int		scanWidth		();

	int		freeSpace		();
protected:
	void		processFIB		(uint8_t *, uint16_t);
private:
	std::vector<serviceId> insert (std::vector<serviceId> &l,
                                          serviceId n, int order);
	RadioInterface	*myRadioInterface;
	fibConfig	*currentConfig;
	fibConfig	*nextConfig;
	ensemble	*theEnsemble;
	void		adjustTime		(int32_t *dateTime);

	void		process_FIG0		(uint8_t *);
	void		process_FIG1		(uint8_t *);
	void		FIG0Extension0		(uint8_t *);
	void		FIG0Extension1		(uint8_t *);
	void		FIG0Extension2		(uint8_t *);
	void		FIG0Extension3		(uint8_t *);
//	void		FIG0Extension4		(uint8_t *);
	void		FIG0Extension5		(uint8_t *);
//	void		FIG0Extension6		(uint8_t *);
	void		FIG0Extension7		(uint8_t *);
	void		FIG0Extension8		(uint8_t *);
	void		FIG0Extension9		(uint8_t *);
	void		FIG0Extension10		(uint8_t *);
//	void		FIG0Extension11		(uint8_t *);
//	void		FIG0Extension12		(uint8_t *);
	void		FIG0Extension13		(uint8_t *);
	void		FIG0Extension14		(uint8_t *);
//	void		FIG0Extension15		(uint8_t *);
//	void		FIG0Extension16		(uint8_t *);
	void		FIG0Extension17		(uint8_t *);
	void		FIG0Extension18		(uint8_t *);
	void		FIG0Extension19		(uint8_t *);
//	void		FIG0Extension20		(uint8_t *);
	void		FIG0Extension21		(uint8_t *);
//	void		FIG0Extension22		(uint8_t *);
//	void		FIG0Extension23		(uint8_t *);
//	void		FIG0Extension24		(uint8_t *);
//	void		FIG0Extension25		(uint8_t *);
//	void		FIG0Extension26		(uint8_t *);

	int16_t		HandleFIG0Extension1	(uint8_t *,
	                                         int16_t,
	                                         const uint8_t,
	                                         const uint8_t,
	                                         const uint8_t);
	int16_t		HandleFIG0Extension2	(uint8_t *,
	                                         int16_t,
	                                         const uint8_t,
	                                         const uint8_t,
	                                         const uint8_t);
	int16_t		HandleFIG0Extension3	(uint8_t *,
	                                         int16_t,
	                                         const uint8_t,
	                                         const uint8_t,
	                                         const uint8_t);
	int16_t		HandleFIG0Extension5	(uint8_t *,
	                                         uint16_t,
	                                         const uint8_t,
	                                         const uint8_t,
	                                         const uint8_t);
	int16_t		HandleFIG0Extension8	(uint8_t *,
	                                         int16_t,
	                                         const uint8_t,
	                                         const uint8_t,
	                                         const uint8_t);
	int16_t		HandleFIG0Extension13	(uint8_t *,
	                                         int16_t,
	                                         const uint8_t,
	                                         const uint8_t,
	                                         const uint8_t);
	int16_t		HandleFIG0Extension21	(uint8_t*,
	                                         uint16_t,
	                                         const uint8_t,
	                                         const uint8_t,
	                                         const uint8_t);

	void		FIG1Extension0		(uint8_t *);
	void		FIG1Extension1		(uint8_t *);
//	void		FIG1Extension2		(uint8_t *);
//	void		FIG1Extension3		(uint8_t *);
	void		FIG1Extension4		(uint8_t *);
	void		FIG1Extension5		(uint8_t *);
	void		FIG1Extension6		(uint8_t *);

	QMutex		fibLocker;
	int		CIFcount;
	int16_t		CIFcount_hi;
	int16_t		CIFcount_lo;
	uint32_t	mjd;			// julianDate

	void		handleAnnouncement	(uint16_t SId,
	                                         uint16_t flags,
	                                         uint8_t SubChId);
signals:
	void		addToEnsemble		(const QString &, int, int);
	void		ensembleName		(int, const QString &);
	void		clockTime		(int, int, int, int, int,
	                                                 int, int, int, int);
	void		changeinConfiguration	();
	void		announcement		(int, int);
	void		nrServices		(int);
	void		lto_ecc			(int, int);
	void		setFreqList		();
};


