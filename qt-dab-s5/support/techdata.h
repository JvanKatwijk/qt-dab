#
/*
 *    Copyright (C)  2015, 2016, 2017, 2018, 2019, 2020, 2021
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


#ifndef	__TECH_DATA_H
#define	__TECH_DATA_H

#include	<QObject>
#include	<QFrame>
#include	"ui_technical_data.h"
#include	"dab-constants.h"
#include	"audio-display.h"
#include	"ringbuffer.h"

class	RadioInterface;
class	QSettings;


class	techData: public QObject, public Ui_technical_data {
Q_OBJECT
public:
		techData	(RadioInterface *,
	                         QSettings *,
	                         RingBuffer<int16_t> *audioData);
		~techData	();

	void	show_serviceData	(audiodata *);
	void	cleanUp			();
	void	show			();
	void	hide			();
	bool	isHidden		();

private:
	RadioInterface		*myRadioInterface;
	QSettings		*dabSettings;
	RingBuffer<int16_t>	*audioData;
	QFrame			myFrame;
	audioDisplay		*the_audioDisplay;

	void			set_buttonColors	(QPushButton *,
	                                            const QString &buttonName);

public slots:
	void		show_frameErrors	(int);
	void		show_aacErrors		(int);
	void		show_rsErrors		(int);
	void		show_motHandling	(bool);
	void		show_rsCorrections	(int, int);
	void		show_timetableButton	(bool);
	void		show_frameDumpButton	(bool);
	void		show_serviceName	(const QString &);
	void		show_serviceId		(int);
	void		show_bitRate		(int);
	void		show_subChId		(int);
	void		show_startAddress	(int);
	void		show_length		(int);
	void		show_language		(int);
	void		show_ASCTy		(int);
	void		show_uep		(int, int);
	void		show_codeRate		(int, int);
	void		show_fm			(int);
	void		showRate		(int);

	void		hideMissed		();
	void		showMissed		(int);
	void		audioDataAvailable	(int, int);

	void		framedumpButton_text	(const QString &s, int);
	void		audiodumpButton_text	(const QString &s, int);

private slots:
	void		color_framedumpButton	();
	void		color_audiodumpButton	();

signals:
	void		handle_timeTable	();
	void		handle_audioDumping	();
	void		handle_frameDumping	();
};


#endif
