#
/*
 *    Copyright (C) 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDR-J (JSDR).
 *    SDR-J is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDR-J is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef	JOURNALINE_DATAHANDLER
#define	JOURNALINE_DATAHANDLER
#include	"dab-constants.h"
#include	"virtual-datahandler.h"
#include	"dabdatagroupdecoder.h"
#include	<QByteArray>


class	journaline_dataHandler:public virtual_dataHandler {
public:
	journaline_dataHandler();
	~journaline_dataHandler();
void	add_mscDatagroup	(QByteArray &);
private:
	DAB_DATAGROUP_DECODER_t theDecoder;
	DAB_DATAGROUP_DECODER_data	myCallBack;
};

#endif



