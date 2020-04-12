#
/*
 *    Copyright (C) 2015 .. 2017
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
 *
 *	MOT handling is a crime, here we have a single class responsible
 *	for handling a MOT directory
 */
#ifndef	__MOT_DIRECTORY__
#define	__MOT_DIRECTORY__

#include	"mot-object.h"
#include	<QString>
class	RadioInterface;

class	motDirectory {
public:
			motDirectory	(RadioInterface *,
	                                 uint16_t,
	                                 int16_t,
	                                 int32_t,
	                                 int16_t,
	                                 uint8_t *);
			~motDirectory();
	motObject	*getHandle	(uint16_t);
	void		setHandle	(motObject *, uint16_t);
	void		directorySegment (uint16_t transportId,
                                        uint8_t *segment,
                                        int16_t segmentNumber,
                                        int32_t segmentSize,
                                        bool    lastSegment);
	uint16_t	get_transportId();
private:
	void		analyse_theDirectory();
	uint16_t	transportId;

	RadioInterface	*myRadioInterface;
	uint8_t		*dir_segments;
	bool		marked [512];
	int16_t		dir_segmentSize;
	int16_t		num_dirSegments;
	int16_t		dirSize;
	int16_t		numObjects;
	typedef struct {
	   bool		inUse;
	   uint16_t	transportId;
	   motObject	*motSlide;
	} motComponentType;
	motComponentType	*motComponents;
};

#endif

