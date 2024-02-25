#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
 *
 *    Many of the ideas as implemented in Qt-DAB are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
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
 *
 */

#pragma once

#include	<QObject>
#include	<QSettings>
#include	<QString>
#include	<cstdio>
#include	<atomic>
#include	"rtl-sdr.h"
#include	"dab-constants.h"
#include	"fir-filters.h"
#include	"device-handler.h"
#include	"ringbuffer.h"
#include	"ui_rtlsdr-widget.h"
class	dll_driver_v3;
class	xml_fileWriter;
//
//	This class is a simple wrapper around the
//	rtlsdr library that is read in  as dll (or .so file in linux)
//	It does not do any processing
class	rtlsdrHandler_v3: public deviceHandler, public  Ui_dabstickWidget {
Q_OBJECT
public:
			rtlsdrHandler_v3	(QSettings *, const QString &);
			~rtlsdrHandler_v3	();
	bool		restartReader		(int32_t);
	void		stopReader		();
	int32_t		getSamples		(std::complex<float> *,
	                                                        int32_t);
	int32_t		Samples			();
	void		resetBuffer		();
	int16_t		maxGain			();
	int16_t		bitDepth		();
	QString		deviceName		();

//	These need to be visible for the separate usb handling thread
	RingBuffer<std::complex<uint8_t>> _I_Buffer;
	struct rtlsdr_dev	*theDevice;
	std::atomic<bool>	isActive;
private:
	QSettings	*rtlsdrSettings;
	int32_t		inputRate;
	int32_t		deviceCount;
	dll_driver_v3	*workerHandle;
	int32_t		lastFrequency;
	int16_t		gainsCount;
	QString		deviceModel;
	QString		recorderVersion;
	FILE            *xmlDumper;
        xml_fileWriter  *xmlWriter;
        bool            setup_xmlDump		();
        void            close_xmlDump		();
        std::atomic<bool> xml_dumping;
	FILE		*iqDumper;
        bool            setup_iqDump		();
        void            close_iqDump		();
        std::atomic<bool> iq_dumping;
	void		record_gainSettings	(int);
	void		update_gainSettings	(int);
	bool		save_gainSettings;

	bool		filtering;
	LowPassFIR	theFilter;
	int		currentDepth;

signals:
	void		new_gainIndex		(int);
	void		new_agcSetting		(bool);
private slots:
	void		set_ExternalGain	(const QString &);
	void		set_autogain		(int);
	void		set_ppmCorrection	(int);
	void		set_xmlDump		();
	void		set_iqDump		();
	void		set_filter		(int);
	void		set_biasControl		(int);
};

