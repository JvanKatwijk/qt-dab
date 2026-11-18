#
/*
 *    Copyright (C) 2024
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
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
/*
 *	Based on user requests a secondary fromtend path for handling 
 *	eti files for inputr is added.
 *	the original ofdm handler is - together with an (almost) copy
 *	of the front end handling of the eti backend made into a
 *	derived version of a "frontend-handler"
 *	Note that the eti frontend hamdling here is simpler and
 *	not all GUI buttons are supported
 */

#include	"frontend-handler.h"

		frontendHandler::frontendHandler	() {}
		frontendHandler::~frontendHandler	() {}
	void frontendHandler::start			() {}
	void frontendHandler::stop			() {}
	void frontendHandler::startDumping		(const QString & a,
	                                                 int b, int c, 
	                                                 const QString d) {
	   (void)a; (void)b; (void)c; (void)d;
	}
	void frontendHandler::stopDumping		() {}
	bool frontendHandler::isDumping			() { return false; }
	bool frontendHandler::startEtiGenerator		(const QString &a) {
	   (void)a;
	   return false;
	}
	void frontendHandler::stopEtiGenerator		() {}
	void frontendHandler::resetEtiGenerator		() {}
	void frontendHandler::setScanMode		(bool b) { (void)b;}
//	void frontendHandler::getFrameQuality		(int *, int*, int *);
	void frontendHandler::set_dataTracer		(bool b) {(void)b;}
	bool frontendHandler::serviceRuns		(uint32_t SId,
	                                                 uint8_t SCIds) {
	   (void)SId, (void)SCIds;
	   return false;
	}
	void frontendHandler::viewCarriers		(uint8_t x) {(void)x;}
	void frontendHandler::setTIIThreshold		(int16_t x) {(void)x;}
	void frontendHandler::setTIICollisions		(int x) {(void)x;}
	bool frontendHandler::isPrimaryService		(uint32_t SId,
	                                                 uint8_t x) {
	   (void)SId; (void)x;
	   return false;
	}
	bool frontendHandler::isAudioService		(uint32_t x,
	                                                 uint8_t y) {
	   (void)x; (void)y;
	   return false;
	}
	bool frontendHandler::isPacketService		(uint32_t x,
	                                                 uint8_t y)  {
	   (void)x; (void)y;
	   return false;
	}
	bool frontendHandler::is_SPI			(uint32_t x) {
	   (void)x;
	   return false;
	}
	std::vector<uint8_t>
                 frontendHandler::get_secondaryServices	(uint32_t SId) {
	   (void)SId;
	   std::vector<uint8_t> res;
	   return res;
	}

	int frontendHandler::FIG07_value		() { return -1;}
	void frontendHandler::getFreqs			(uint32_t x,
	                                                 std::vector<uint32_t> & y) {
	   (void)x; (void)y;
	}
	void frontendHandler::getServiceName		(QString & s1,
	                                                 QString & s2,
	                                                 uint32_t v1,
	                                                 uint8_t v2) {
	   (void)s1; (void)s2; (void)v1; (void)v2;
	}
	void frontendHandler::mapNameToId		(const QString &s1,
	                                                 uint32_t &SId,
	                                                 uint8_t &SCIds) {
	   (void)s1;
	   SId		= 0;
	   SCIds	= 0;
	}
        void frontendHandler::audioData			(uint32_t SId,
	                                                 uint8_t SCIds,
	                                                 audiodata &ad) {
	   (void)SId;
	   (void)SCIds;
	}
        void frontendHandler::packetData		(uint32_t SId,
	                                                 uint8_t SCIds,
	                                                 packetdata &pd) {
	   (void)SId;
	   (void)SCIds;
	}
	uint32_t frontendHandler::julianDate		() {return 0;}
	QList<contentType> frontendHandler::contentPrint	() {
	   QList<contentType> res;
	   return res;
	}
	int frontendHandler::freeSpace			() {return -1;}
	void frontendHandler::startFicDump		(const QString &s) {}
	bool frontendHandler::ficDumping_on		() {return false;}
	void frontendHandler::stopFicDump		() {}
//	void frontendHandler::setSpeedUp		(bool);
	void frontendHandler::setFreqCorrelator		(uint8_t m) {
	   (void)m;
	}
	void frontendHandler::stopService		(uint32_t SId,
	                                                 uint8_t SCIds) {
	   (void)SId; (void)SCIds;
	}
	bool frontendHandler::setAudioChannel		(audiodata & ad,
	                                         RingBuffer<std::complex<int16_t>> *b,
	                                         FILE *f, int flag) {
	   (void)b; (void)f; (void)flag;
	}
	bool frontendHandler::setDataChannel		(packetdata &pd,
	                                                 RingBuffer<uint8_t> *b,
	                                                int flag) {
	   (void)b; (void)flag;
	}

	void frontendHandler::handleIQSelector		()	{}
	void frontendHandler::handleDecoderSelector	(int d)	{ (void)d;}
	void frontendHandler::setCorrelationOrder	(const QString &s) {}
	void frontendHandler::setDXMode			(bool b) { (void)b;}
	void frontendHandler::set_dcRemoval		(bool b) { (void)b;}
	void frontendHandler::set_iqBalance		(bool b) { (void)b;}
	std::vector<basicService>
	               frontendHandler::getServices	() {
	   std::vector<basicService> res;
	   return res;
	}

