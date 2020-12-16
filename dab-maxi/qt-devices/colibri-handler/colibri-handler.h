#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the spectrumViewer
 *
 *    spectrumViewer is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 2 of the License.
 *
 *    spectrumViewer is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with spectrumViewer. if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef	__COLIBRI_HANDLER_H
#define	__COLIBRI_HANDLER_H
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

	class	colibriHandler: public deviceHandler, public Ui_colibriWidget {
Q_OBJECT
public:

			colibriHandler		(QSettings *);
			~colibriHandler		(void);
	bool		restartReader		(int32_t);
	void		stopReader		();
	void		setVFOFrequency		(int32_t);
	int32_t		getVFOFrequency		(void);
	int32_t		getSamples		(std::complex<float> *, int32_t);
	int32_t		Samples			();
	void		resetBuffer		(void);
	int16_t		bitDepth		(void);
	void		hide			();
	void		show			();
	bool		isHidden		();
	QString		deviceName		();

	RingBuffer<std::complex<float>>	_I_Buffer;
	int16_t		convBufferSize;
	int16_t		convIndex;
	std::vector <complex<float> >   convBuffer;
	int16_t		mapTable_int   [2048];
	float		mapTable_float [2048];
private:
	QFrame			myFrame;
	LibLoader		m_loader;
	QSettings		*colibriSettings;
	int			sampleRate	(int);
	Descriptor		m_deskriptor;
	std::atomic<bool>	running;
	bool			iqSwitcher;
private slots:
	void			set_gainControl	(int);
	void			handle_iqSwitcher	();
};
#endif
