#
/*
 *    Copyright (C) 2014 .. 2017
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
#include	<QFrame>
#include	<QSettings>
#include	<QLineEdit>
#include	<atomic>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	<SoapySDR/Device.hpp>
#include	<SoapySDR/Formats.hpp>
#include        <SoapySDR/Errors.hpp>
#include	"device-handler.h"
#include	"ui_soapy-widget.h"
#include	"soapy-worker.h"

class	soapyHandler: public QObject,
	              public deviceHandler, public Ui_soapyWidget {
Q_OBJECT
public:
			soapyHandler		(QSettings *);
			~soapyHandler		();
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
	SoapySDR::Device	*device;
	SoapySDR::Stream	*stream;
	QSettings		*soapySettings;
	QLineEdit		*deviceLineEdit;
	std::vector<std::string> gains;
	soapyWorker		*worker;
private slots:
	void		createDevice		();
	void		handle_spinBox_1	(int);
	void		handle_spinBox_2	(int);
	void		set_agcControl		(int);
	void		handleAntenna		(const QString &);
};

