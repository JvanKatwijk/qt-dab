#
/*
 *    Copyright (C) 2016 .. 2020
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
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
//
//	The eti controller reads in the eti file, preporcesses
//	the FIB/FIG data and passed it on to the fib decoder
//
//	It also implements the interface to the backends (i.e.
//	the mscBackend class in the regular processing line
//	It is essentially a copy of the eti processor of the
//	etiBackend in the eti-stuff  repository
//
#pragma once
/*
 *
 */
#include	"frontend-handler.h"
#include	"fib-decoder.h"
#include	<atomic>
#include	"frame-processor.h"

class		RadioInterface;
class		etiFiles;
class		processParams;
class		logger;
class		dataprocessor;;

class etiProcessor: public frontendHandler {
Q_OBJECT
public:
		etiProcessor  	(RadioInterface *,
	                         etiFiles	*,
	                         processParams	*,
	                         QSettings	*,
	                         logger		*,
	                         uint8_t);
		~etiProcessor		();
	void	start			();
	void	stop			();
	bool	serviceRuns		(uint32_t, uint8_t);
	bool	isPrimaryService	(uint32_t, uint8_t);
	bool	isAudioService		(uint32_t x, uint8_t y);
	bool	isPacketService		(uint32_t x, uint8_t y);
	bool	is_SPI			(uint32_t x);
	std::vector<uint8_t>
                 get_secondaryServices	(uint32_t SId);
	void	getServiceName		(QString & s1, QString & s2,
                                         uint32_t v1, uint8_t v2);
	void	mapNameToId		(const QString &,
                                                        uint32_t &, uint8_t &);
	void	audioData		(uint32_t SId, uint8_t SCIds,
                                                         audiodata &ad);
        void	packetData		(uint32_t SId, uint8_t SCIds,
                                                         packetdata &pd);
	void	getFreqs		(uint32_t SId, 
	                                       std::vector<uint32_t> &fl);
        uint32_t julianDate		();
	QList<contentType> 
	         contentPrint		();
	void	stopService		(uint32_t SId, uint8_t SCIds);
	bool	setAudioChannel		(audiodata & ad,
                                         RingBuffer<std::complex<int16_t>> *b,
                                         FILE *f, int flag);
        bool	setDataChannel		(packetdata &pd,
                                         RingBuffer<uint8_t> *b, int flag);
	std::vector<basicService>
	    	getServices		();

private:
	fibDecoder	my_fibDecoder;
	RadioInterface	*myRadioInterface;
	etiFiles	*theDevice;
	processParams	*p;
	QSettings	*dabSettings;
	logger		*theLogger;
	uint8_t		cpuSupport;
	RingBuffer<int16_t>    *audioBuffer;
	std::atomic<bool>	running;

	int		fibCounter;
	int		successRatio;
	void		process_FIC (uint8_t *p, uint16_t fib);

	QMutex          locker;
        bool            audioService;
//
//	We maintain a stack of active services (similar to mscBackend
//	The first two parmaeters are to identify the service from
//	the caller side, the latter are required for identification
//	in the eti datastream
	typedef struct {
	   uint32_t	SId;	// Sid and SCIds for identification
	   uint8_t	SCIds;
	   uint8_t	subChId;
	   int		bitRate;
	   frameProcessor	*p_a;
	} runElement;
	std::vector<runElement>  runStack;

protected:	
virtual	void	run			();

signals:
	void		showFICQuality  (int, int);
	void		activeServices	(int);
};

