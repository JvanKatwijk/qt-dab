#
/*
 *    Copyright (C) 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
#ifndef		__TDC_DATAHANDLER__
#define		__TDC_DATAHANDLER__
#include	"dab-constants.h"
#include	<vector>
#include	"virtual-datahandler.h"
#include	"ringbuffer.h"

class	RadioInterface;
class	tdc_dataHandler : public virtual_dataHandler {
Q_OBJECT
public:
		tdc_dataHandler		(RadioInterface *,
	                                 RingBuffer<uint8_t> *, int16_t);
		~tdc_dataHandler();
	void	add_mscDatagroup	(std::vector<uint8_t>);
private:
	int32_t	handleFrame_type_0	(uint8_t *data,
                                         int32_t offset, int32_t length);
	int32_t	handleFrame_type_1	(uint8_t *data,
                                         int32_t offset, int32_t length);
	bool	serviceComponentFrameheaderCRC (uint8_t *, int16_t, int16_t);
	RadioInterface	 *myRadioInterface;
	RingBuffer<uint8_t> *dataBuffer;
signals:
	void	bytesOut		(int, int);
};
#endif



