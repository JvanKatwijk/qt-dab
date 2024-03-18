#
/*
 *    Copyright (C)  2016 .. 2024
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB 
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

#include        "dab-constants.h"
#include	<QObject>
#include	<qwt.h>
#include	<qwt_plot.h>
#include	<QVector>
#include	<QByteArray>
#include	"fft-handler.h"
#include	"super-frame.h"
#include	"ui_scopewidget.h"

#define	SHOW_SPECTRUM		0
#define	SHOW_CORRELATION	1
#define	SHOW_NULL		2
#define	SHOW_TII		3
#define	SHOW_CHANNEL		4
#define	SHOW_STDDEV		5

class	RadioInterface;
class	QSettings;

class	spectrumScope;
class	waterfallScope;
class	nullScope;
class	correlationScope;
class	devScope;
class	channelScope;
class	IQDisplay;

class	displayWidget: public QObject, public Ui_scopeWidget {
Q_OBJECT
public:
		displayWidget	(RadioInterface *,
	                         QSettings *);
		~displayWidget	();
	int	get_tab		();
	void	set_bitDepth	(int);
	void	show_spectrum	(std::vector<Complex> &, int);
	void	show_null	(Complex *, int);
	void	show_correlation	(const std::vector<float> &, int,
	                         QVector<int> &, int);
	void	show_tii	(std::vector<Complex>, int);
	void	show_channel	(const std::vector<Complex>);
	void	show_stdDev	(const std::vector<float>);

	void	showIQ		(const std::vector<Complex>);

	void	show_quality	(float, float, float);
	void	show_corrector	(int, float);
	void	show_snr	(float);
	void	show_correction	(int);
	void	show_clock_err	(int);
	void	showFrequency	(int);

	void	show_cpuLoad	(float);
	void	show_transmitters	(QByteArray &);

	void	show_dcOffset	(float);
	void	set_dcRemoval	(bool);
	void	set_syncLabel	(bool);
	void	show		();
	void	hide		();
	bool	isHidden	();

private:
	superFrame		myFrame;
	fftHandler		theFFT;
	QSettings		*dabSettings_p;
	spectrumScope		*spectrumScope_p;
	waterfallScope		*waterfallScope_p;
	nullScope		*nullScope_p;
	correlationScope	*correlationScope_p;
	spectrumScope		*TII_Scope_p;
	devScope		*devScope_p;
	channelScope		*channelScope_p;
	IQDisplay		*IQDisplay_p;

	int			currentTab;
	int			ncpScope;
private slots:
	void		switch_tab	(int);
	void		rightMouseClick		();
	void		handle_ncpScope_checkBox (int);
signals:
	void		mouseClick	();
	void		frameClosed	();
};


