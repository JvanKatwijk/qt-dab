#
/*
 *    Copyright (C) 2017 .. 2024
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
 *
 */
#pragma once

#include	"dab-constants.h"
#include	"virtual-datahandler.h"
#include	"dabdatagroupdecoder.h"
#include	"journaline-screen.h"
#include	<vector>
#include	"NML.h"
#include	<QObject>

class	journaline_dataHandler: public virtual_dataHandler {
Q_OBJECT
public:
		journaline_dataHandler	();
		~journaline_dataHandler	();
	void	add_mscDatagroup	(const std::vector<uint8_t> &);
	void	add_to_dataBase		(NML *);
private:
	std::vector<tableElement> table;
	journalineScreen	theScreen;
	DAB_DATAGROUP_DECODER_t theDecoder;
	DAB_DATAGROUP_DECODER_data	myCallBack;
	void		init_dataBase	();
	void		destroy_dataBase ();
	int		findIndex	(int);
signals:
	void		start		(int);
};

