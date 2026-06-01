#
/*
 *    Copyright (C)  2015 .. 2025
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
//
//	Implementation of the FIG database

#include	<QObject>
#include	<stdint.h>
#include	<vector>
#include	<QString>
#include	"dab-constants.h"

#include	"fib-dbtypes.h"
class	RadioInterface;

class	fibConfig: public QObject {
Q_OBJECT
public:
			fibConfig	(RadioInterface *);
			~fibConfig	();
//
//	The db types are included
//	Most components are a direct translation of the
//	FIG 

        std::vector<FIG01>      FIG01_stack;
	bool	in_FIG01_stack	(const FIG01 &);
        std::vector<FIG02>      FIG02_stack;
        std::vector<FIG03>      FIG03_stack;
	bool	in_FIG03_stack	(const FIG03 &);
	int32_t	FIG07_value;
	bool	in_FIG08_stack	(const FIG08 &);
	std::vector<FIG08>      FIG08_stack;
        std::vector<FIG013>     FIG013_stack;
	bool	in_FIG013_stack	(const FIG013 &);
        std::vector<FIG014>     FIG014_stack;
	bool	in_FIG014_stack	(const FIG014 &);
	void		reset		();
	int		freeSpace	();
};

