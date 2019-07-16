#
/*
 *    Copyright (C) 2014 .. 2017
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

#ifndef __SOAPY_HANDLER__
#define	__SOAPY_HANDLER__

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
#include	"virtual-input.h"
#include	"ui_soapy-widget.h"
#include	"soapy-worker.h"

class	soapyHandler: public virtualInput, public Ui_soapyWidget {
Q_OBJECT
public:
			soapyHandler		(QSettings *);
			~soapyHandler		(void);
	void		setVFOFrequency		(int32_t);
	int32_t		getVFOFrequency		(void);
	int32_t		defaultFrequency	(void);

	bool		restartReader		(void);
	void		stopReader		(void);
	int32_t		getSamples		(std::complex<float> *,
	                                                          int32_t);
	int32_t		Samples			(void);
	void		resetBuffer		(void);
	int16_t		bitDepth		(void);
private:
	SoapySDR::Device	*device;
	SoapySDR::Stream	*stream;
	QSettings		*soapySettings;
	QFrame			*myFrame;
	QLineEdit		*deviceLineEdit;
	std::vector<std::string> gains;
	soapyWorker		*worker;
private slots:
	void		createDevice		(void);
	void		handle_spinBox_1	(int);
	void		handle_spinBox_2	(int);
	void		set_agcControl		(int);
	void		handleAntenna		(const QString &);
};
#endif

