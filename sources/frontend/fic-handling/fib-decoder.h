#
/*
 *    Copyright (C) 2016 .. 2026
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

#include	"fib-dbtypes.h"
#include	"fib-config.h"

class	fibDecoder: public QObject {
Q_OBJECT
public:
			fibDecoder		(RadioInterface *);
			~fibDecoder		();

	void		clearEnsemble		();
	void		reset			();
	void		connectChannel		();
	void		disconnectChannel	();
	bool		syncReached		();
//
//	for eti
	int		nrChannels		();
	void		getChannelInfo		(channel_data *, const int);
//
//	For the timetable handling
	std::vector<basicService> getServices   ();
//
//	for content printing and scan results
	QList<contentType> contentPrint		();

	bool		isPrimaryService	(uint32_t, uint8_t);
	bool		isAudioService		(uint32_t, uint8_t);
	bool		is_SPI			(uint32_t);
	std::vector<uint8_t>
                get_secondaryServices		(uint32_t SId);

	int		FIG07_value		();
	void		getFreqs		(uint32_t,
	                                            std::vector<uint32_t> &);
	void		audioData		(uint32_t,
	                                               uint8_t,  audiodata &);
	void		packetData		(uint32_t,
	                                               uint8_t, packetdata &);

	void		getServiceName		(QString &, QString &,
	                                         uint32_t, uint8_t);
	void		mapNameToId		(const QString &,
	                                            uint32_t &, uint8_t &);
//	uint16_t	getAnnouncing		(uint16_t);
//	std::vector<int>	getFrequency	(const QString &);
//	bool		nonTIIFrame		();	
	void		getCIFcount		(int16_t &, int16_t &);
	uint32_t	julianDate		();
	int		freeSpace		();
//	QList<contentType> contentPrint		();
//	std::vector<basicService> getServices	();
protected:
	void		processFIB		(uint8_t *, uint16_t);
private:
	RadioInterface	*myRadioInterface;

	void		process_FIG0		(uint8_t *);
	void		process_FIG1		(uint8_t *);
	void		FIG0Extension0		(uint8_t *);
	void		FIG0Extension1		(uint8_t *);
	void		FIG0Extension2		(uint8_t *);
	void		FIG0Extension3		(uint8_t *);
//	void		FIG0Extension4		(uint8_t *);
	void		FIG0Extension5		(uint8_t *);
	void		FIG0Extension6		(uint8_t *);
	void		FIG0Extension7		(uint8_t *);
	void		FIG0Extension8		(uint8_t *);
	void		FIG0Extension9		(uint8_t *);
	void		FIG0Extension10		(uint8_t *);
//	void		FIG0Extension11		(uint8_t *);
//	void		FIG0Extension12		(uint8_t *);
	void		FIG0Extension13		(uint8_t *);
	void		FIG0Extension14		(uint8_t *);
	void		FIG0Extension15		(uint8_t *);
//	void		FIG0Extension16		(uint8_t *);
	void		FIG0Extension17		(uint8_t *);
	void		FIG0Extension18		(uint8_t *);
	void		FIG0Extension19		(uint8_t *);
	void		FIG0Extension20		(uint8_t *);
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
	int16_t		HandleFIG0Extension6	(uint8_t *,
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
	int16_t		HandleFIG0Extension20	(uint8_t *,
	                                         uint16_t,
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
	std::atomic<int>	CIFcount;
	std::atomic<int16_t>	CIFcount_hi;
	std::atomic<int16_t>	CIFcount_lo;
	uint32_t	mjd;			// julianDate

	void		handleAnnouncement	(uint16_t SId,
	                                         uint16_t flags,
	                                         uint8_t SubChId);

	
signals:
	void	signal_FIG00	();	// change in confug
	void	signal_FIG010	(int, int, int, int, int, int, int, int, int);
	void	signal_FIG019	(uint16_t, int);
	void	signal_FIG021	(uint16_t, uint32_t);
	void	signal_FIG09	(int, uint8_t, uint8_t);
	void	signal_FIG10	(const QString &, uint16_t);
	void	signal_FIG11	(const QString &, uint32_t);
	void	signal_FIG14	(const QString &, uint32_t, uint8_t);
	void	signal_FIG15	(const QString &, uint32_t);
//	signal	FIG019	// 	service specific announcement change
//	void	signal_FIG019	(de boodschap)

private:
	
	fibConfig	*currentConfig;
	fibConfig	*nextConfig;
	FIG00		FIG00_value;
	bool    in_FIG05_stack  (const FIG05 &);
	std::vector<FIG05>      FIG05_stack;

	FIG09		FIG09_value;
	FIG010		FIG010_value;
	std::vector<FIG017>     FIG017_stack;
	bool    in_FIG017_stack (const FIG017 &);
	std::vector<FIG018>     FIG018_stack;
	bool    in_FIG018_stack (const FIG018 &);
	std::vector<FIG021>     FIG021_stack;
	bool    in_FIG021_stack (const FIG021 &);

	FIG10		FIG10_value;
        std::vector<FIG11>      FIG11_stack;
        std::vector<FIG14>      FIG14_stack;
        std::vector<FIG15>      FIG15_stack;
        std::vector<FIG16>      FIG16_stack;
	int	dateTime [8];

	void		adjustTime		(int32_t *dateTime);
	uint16_t	get_subChId		(uint32_t, uint8_t);
	void		check_announcements (uint8_t clusterId,
                                        uint8_t AswFlags,
                                        uint8_t newFlag, uint16_t subChId);

};


