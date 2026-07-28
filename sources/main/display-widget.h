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
#include	<QVector>
#include	<QByteArray>
#include	"fft-handler.h"
#include	"super-frame.h"
#include	"db-element.h"
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
class	nullScope;
class	tiiScope;
class	correlationScope;
class	devScope;
class	carrierScope;
class	channelScope;
class	iqDisplay;

class	displayWidget: public QObject, public Ui_scopeWidget {
Q_OBJECT
public:
		displayWidget	(RadioInterface *,
	                         QSettings *);
		~displayWidget	();
	void	storePosition	();
	int	getTab		();
	void	setBitDepth	(int);

	void	setSilent	();
	void	showSpectrum	(std::vector<Complex> &, int);
	void	showNULL	(std::vector<Complex> &, int, int);
	void	showCorrelation	(std::vector<float> &,
	                         QVector<int> &, int,
	                         std::vector<transmitter> &);
//	                                 std::vector<transmitterDesc> &);
	void	showTII		(std::vector<Complex>, int, int);
	void	showChannel	(const std::vector<Complex>);
	void	showStdDev	(const std::vector<float>);

	bool	does_showCarriers	();
	uint8_t	carrierMode	();
	void	showCarriers	(int, const std::vector<DABFLOAT> &);
	void	showIQ		(const std::vector<Complex>);

	void	showFICBER	(float);
	void	showQuality	(float, float, float);
	void	showCorrector	(int, float);
	void	showSNR		(float);
	void	showCorrection	(int);
	void	showClock_err	(int);
	void	showFrequency	(const QString &, int);

	void	cleanTII	();
	void	showCPULoad	(float);
	void	showTransmitters	(std::vector<transmitter> &);

	void	showDCOffset	(float);
	void	set_dcRemoval	(bool);
	void	setSyncLabel	(bool);
	void	setFIC_label	(bool);
	void	show		();
	void	hide		();
	bool	isHidden	();

private:
	superFrame		myFrame;
	fftHandler		theFFT;
	QSettings		*dabSettings_p;
	spectrumScope		*spectrumScope_p;
	nullScope		*nullScope_p;
	tiiScope		*tiiScope_p;
	correlationScope	*correlationScope_p;
	channelScope		*channelScope_p;
	devScope		*devScope_p;
	carrierScope		*carrierScope_p;
	iqDisplay		*IQDisplay_p;

	DABFLOAT		workingBuffer [2048];
	int			currentTab;
	int			ncpScope;
	bool			setMarkers;

	uint8_t			show_carrierType;
	int			carrier_width;
	int			carrier_height;
private slots:
	void		switch_tab		(int);
	void		rightMouseClick		();
	void		handleNcpScope_checkBox (int);
	void		handleMarksButton	();
	void		handle_mouseClicked	();

	void		handle_showCarriers	();
	void		handle_carrierSelector	(const QString &);
signals:
	void		mouseClick	();
	void		frameClosed	();
	void		viewCarriers	(uint8_t);
};


