#
/*
 *    Copyright (C) 2026
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
#include	<atomic>
#include	"ui_soapy-widget.h"
#include	"device-handler.h"
#include	"soapy-converter.h"
#include 	"ringbuffer.h"
#include	<SoapySDR/Device.hpp>

class	QSettings;
class	xml_fileWriter;

class soapyHandler: public deviceHandler, public Ui_soapyWidget {
Q_OBJECT
public:
		soapyHandler	(QSettings *);
		~soapyHandler	();
	bool	restartReader	(int, int skipped = 0);
	void	stopReader	();
	void	reset		();
	int	getSamples	(std::complex<float> *, int);
	int	Samples		();
	int	getGain		();
	int	getGainCount	();
	int16_t	bitDepth	();
	bool	isFileInput	();

private:
	RingBuffer<std::complex<float>> m_sampleBuffer;
	soapyConverter  theConverter;
	QSettings       *soapySettings;
	SoapySDR::Device *m_device;
	SoapySDR::Stream *m_stream;
	std::vector<std::string> gainsList;
	std::string	streamFormat;
	std::atomic<bool>	m_running;
	std::atomic<bool>	m_dumping;
	std::atomic<int>	toSkip;
	int32_t			selectedRate;
	int32_t			m_freq;
	std::thread		m_thread;

	QString			selectedString;
	QString			selectedSerial;
	void			workerthread	();
	void			createDevice	(const QString d,
	                                                 const QString s);
	int			findDesiredSamplerate (const
	                                       SoapySDR::RangeList &theRanges);
	int			findDesiredBandwidth  (const
	                                       SoapySDR::RangeList &theRanges);

	xml_fileWriter		*xmlWriter;
	bool			setup_xmlDump	(bool);
	void			close_xmlDump	();

private slots:
	void			setGain_0	(int);
	void			setGain_1	(int);
	void			setGain_2 	(int);
	void			set_agcControl	(int);
	void			handleAntenna 	(const QString &);
	void			handle_xmlDump	();

public slots:
	void			reportStatus	(const QString &);
};
