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

class	padHandler: public QObject {
Q_OBJECT
public:
			padHandler		(RadioInterface *,
	                                         bool);
			~padHandler		();
	void		processPAD		(uint8_t *,
	                                         int16_t, uint8_t, uint8_t);
private:
	RadioInterface	*myRadioInterface;
	bool		backgroundFlag;
	void		handle_variablePAD	(uint8_t *, int16_t, uint8_t);
	void		handle_shortPAD		(uint8_t *, int16_t, uint8_t);
	void		dynamicLabel		(uint8_t *, int16_t, uint8_t);
	void		new_MSC_element		(std::vector<uint8_t>);
	void		add_MSC_element		(std::vector<uint8_t>);
	void		build_MSC_segment	(std::vector<uint8_t>);
	bool		pad_crc			(uint8_t *, int16_t);
	QByteArray	dynamicLabelText;
	int16_t		charSet;
//	motObject	*currentSlide;
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

signals:
	void		show_label			(const QString &, int);
	void		show_mothandling		(bool);
};

