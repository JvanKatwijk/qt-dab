#
/*
 *    Copyright (C) 2015 .. 2025
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
#include	<cstring>
#include	<cstdint>
#include	<vector>
#include	"mot-object.h"
#include	<QScopedPointer>

class	RadioInterface;

struct DL2_object {
	uint8_t ct;
	uint8_t base;
	uint8_t len;
	QString str;
};

struct DL2_base {
	bool	valid;
	uint8_t	IT;
	uint8_t	IR;
	QString	dlsText;
	struct DL2_object entity [4];
};

	
class	padHandler: public QObject {
Q_OBJECT
public:
			padHandler		(RadioInterface *,
	                                         uint32_t,	// SId
	                                         bool);
			~padHandler		();
	void		processPAD		(const uint8_t *,
	                                         int16_t, uint8_t, uint8_t);
private:
	RadioInterface	*myRadioInterface;
	uint32_t	SId;
	bool		backgroundFlag;
	void		handle_variablePAD	(const uint8_t *,
	                                             int16_t, uint8_t);
	void		handle_shortPAD		(const uint8_t *,
	                                             int16_t, uint8_t);
	void		dynamicLabel		(const uint8_t *,
	                                             int16_t, uint8_t);
	void		new_MSC_element		(const std::vector<uint8_t> &);
	void		add_MSC_element		(const std::vector<uint8_t> &);
	void		build_MSC_segment	(const std::vector<uint8_t> &);
	bool		pad_crc			(uint8_t *, int16_t);
	QByteArray	dynamicLabelText;
	int16_t		charSet;
	QScopedPointer<motObject>	currentSlide;
	uint8_t		last_appType;
	bool		mscGroupElement;
	int		xpadLength;
	int16_t		still_to_go;
	std::vector<uint8_t> shortpadData;
        bool		lastSegment;
        bool		firstSegment;
	int16_t		segmentNumber;
//      dataGroupLength is set when having processed an appType 1
        int 		dataGroupLength;

	int16_t		segmentno;
	int16_t		remainDataLength;
	bool		isLastSegment;
	bool		moreXPad;

//
//      The msc_dataGroupBuffer is - as the name suggests - used for
//      assembling the msc_data group.
        std::vector<uint8_t> msc_dataGroupBuffer;

//	Experimental
	struct {
	   QString theText;
	   bool		valid;
	   QString	title;
	   QString	composer;
	   QString	stationname;
	   QString	currentProgram;
	} DL2_record;
	DL2_base	the_DL2;
	QString		extractText		(uint16_t, uint16_t);
	void		add_toDL2		(const QString &);
	void		add_toDL2		(const uint8_t *, int,
	                                                uint8_t, uint8_t);
signals:
	void		showLabel		(const QString &, int);
	void		show_mothandling	(bool);
	void		show_dl2		(uint8_t, uint8_t,
	                                         const QString &);
};

