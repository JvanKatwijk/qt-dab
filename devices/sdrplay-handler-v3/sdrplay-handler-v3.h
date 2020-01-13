#
/*
 *    Copyright (C) 2014 .. 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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

#ifndef __SDRPLAY_HANDLER_V3__
#define	__SDRPLAY_HANDLER_V3__

#include	<QObject>
#include	<QFrame>
#include	<QSettings>
#include	<atomic>
#include	<stdio.h>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"virtual-input.h"
#include	"ui_sdrplay-widget-v3.h"

class	controlQueue;
class	sdrplayController;
class	xml_fileWriter;

#ifdef __MINGW32__
#define GETPROCADDRESS  GetProcAddress
#else
#define GETPROCADDRESS  dlsym
#endif

class	sdrplayHandler_v3: public virtualInput, public Ui_sdrplayWidget_v3 {
Q_OBJECT
public:
			sdrplayHandler_v3	(QSettings *, QString &);
			~sdrplayHandler_v3	();
	int32_t		getVFOFrequency		();
	int32_t		defaultFrequency	();

	bool		restartReader		(int32_t);
	void		stopReader		();
	int32_t		getSamples		(std::complex<float> *,
	                                                          int32_t);
	int32_t		Samples			();
	void		resetBuffer		();
	int16_t		bitDepth		();

private:
	QString			recorderVersion;
	RingBuffer<std::complex<int16_t>>	*_I_Buffer;
	sdrplayController	*theController;
	controlQueue		*theQueue;
	int32_t			vfoFrequency;
	std::atomic<bool>	running;
	int16_t			hwVersion;
	QSettings		*sdrplaySettings;
	bool			agcMode;
	int16_t			nrBits;
	int16_t			denominator;
        FILE			*xmlDumper;
        xml_fileWriter		*xmlWriter;
        bool			setup_xmlDump		();
        void			close_xmlDump		();
        std::atomic<bool>	dumping;

private slots:
	void			set_ifgainReduction	(int);
	void			set_lnagainReduction	(int);
	void			set_agcControl		(int);
	void			set_ppmControl		(int);
	void			set_antennaSelect	(const QString &);
public slots:
	void			show_TotalGain		(int);
	void			show_DeviceData		(const QString &,
	                                                     int, float);
	void			show_Error		(int);
	void			show_runFlag		(bool);
	void			show_lnaGain		(int);
	void			set_lnaRange		(int, int);
	void			show_deviceLabel	(const QString &, int);
	void			show_antennaSelector	(bool);
	void			show_tunerSelector	(bool);
	void			set_xmlDump		();

};
#endif

