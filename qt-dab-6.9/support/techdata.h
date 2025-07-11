#
/*
 *    Copyright (C)  2016 .. 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB 
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
#include	"ui_technical_data.h"
#include	"dab-constants.h"
#include	"audio-display.h"
#include	"ringbuffer.h"
#include	"super-frame.h"

class	RadioInterface;
class	QSettings;


class	techData: public QObject, public Ui_technical_data {
Q_OBJECT
public:
		techData	(RadioInterface *,
	                         QSettings *,
	                         RingBuffer<std::complex<int16_t>> *audioData);
		~techData	();
	void	showServiceData	(audiodata *);
	void	cleanUp			();
	void	show			();
	void	hide			();
	bool	isHidden		();
	void	isDABPlus		(bool);
	void	hideMissedLabel	();
	void	showTimetableButton	(bool);
	void	updateFM		(std::vector<int> &);
private:
	RadioInterface		*myRadioInterface;
	QSettings		*dabSettings;
	RingBuffer<std::complex<int16_t>>	*audioData;
	superFrame		myFrame;
	audioDisplay		*theAudioDisplay;

	void			setButtonColors	(QPushButton *,
	                                            const QString &buttonName);

public slots:
	void		showFrameErrors		(int);
	void		showAacErrors		(int);
	void		showRsErrors		(int);
	void		showRsCorrections	(int, int);
	void		showFrameDumpButton	(bool);
	void		showServiceName		(const QString &,
	                                         const QString &);
	void		showServiceId		(int);
	void		showSubChId		(int);
	void		showStartAddress	(int);
	void		showLength		(int);
	void		showLanguage		(int);
	void		showASCTy		(int);
	void		showUep			(int, int);
	void		showCodeRate		(int, int);
	void		showFm			(std::vector<int> &);
	void		showRate		(int, bool, bool);
	void		showStereo		(bool);

	void		hideMissed		();
	void		showMissed		(int);
	void		audioDataAvailable	(int, int);

	void		framedumpButton_text	(const QString &s, int);
	void		audiodumpButton_text	(const QString &s, int);

private slots:
	void		colorFramedumpButton	();
	void		colorAudiodumpButton	();

signals:
	void		handleTimeTable		();
	void		handleAudioDumping	();
	void		handleFrameDumping	();
	void		frameClosed		();
};


