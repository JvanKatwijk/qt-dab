#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 2 of the License.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB. if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma once
#include	<QThread>
#include	<QSettings>
#include	<QFileDialog>
#include	<QTime>
#include	<QDate>
#include	<QLabel>
#include	<QDebug>
#include	<QFileDialog>
#include	"ui_colibri-widget.h"
#include	"common.h"
#include	"LibLoader.h"
#include	"ringbuffer.h"
#include	"device-handler.h"

	class	colibriHandler: public QObject,
	                        public deviceHandler, public Ui_colibriWidget {
Q_OBJECT
public:

			colibriHandler		(QSettings *);
			~colibriHandler		();

	bool		restartReader		(int32_t);
	void		stopReader		();
	int32_t		getSamples		(std::complex<float> *, int32_t);
	int32_t		Samples			();
	void		resetBuffer		();
	int16_t		bitDepth		();
	QString		deviceName		();

	RingBuffer<std::complex<float>>	_I_Buffer;
	int16_t		convBufferSize;
	int16_t		convIndex;
	std::vector <complex<float> >   convBuffer;
	int16_t		mapTable_int   [2048];
	float		mapTable_float [2048];
private:
	LibLoader		m_loader;
	QSettings		*colibriSettings;
	int			sampleRate	(int);
	int			selectedRate;
	Descriptor		m_deskriptor;
	std::atomic<bool>	running;
	bool			iqSwitcher;
private slots:
	void			set_gainControl	(int);
	void			handle_iqSwitcher	();
};
