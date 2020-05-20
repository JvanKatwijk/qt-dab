#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2; if not, write to the Free Software
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
#include	"device-handler.h"
#include	"ui_soapy-widget.h"
#include	"soapy-worker.h"

class	RadioInterface;
class	dabProcessor;

class	soapyHandler: public deviceHandler, public Ui_soapyWidget {
Q_OBJECT
public:
			soapyHandler		(RadioInterface *,
	                                         QSettings *,	
                                                 dabProcessor *);
			~soapyHandler		(void);
	int32_t		getVFOFrequency		(void);
	int32_t		defaultFrequency	(void);
	bool		restartReader		(int32_t);
	void		stopReader		(void);
	void		resetBuffer		(void);
	void		show			();
	void		hide			();
	bool		isHidden		();
	int16_t		bitDepth		(void);
private:
	void			handle_Value	(int, float, float);
	QFrame			myFrame;
	dabProcessor		*base;
	int			totalOffset;
	SoapySDR::Device	*device;
	SoapySDR::Stream	*stream;
	QSettings		*soapySettings;
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

