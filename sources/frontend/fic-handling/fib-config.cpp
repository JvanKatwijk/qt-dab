#
/*
 *    Copyright (C)  2018 .. 2025
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

//
//	fibConfig contains the FIG stacks and values that are
//	dependent on the CN_bit
#include	"fib-config.h"
#include	<stdio.h>
#include	<string.h>
#include	"ITU_tables.h"
#include	"radio.h"

//
	fibConfig::fibConfig	(RadioInterface *mr) {
	(void)mr;
	reset ();
}

	fibConfig::~fibConfig	() {}

void	fibConfig::reset	() {
	FIG01_stack.	resize	(0);
	FIG02_stack.	resize	(0);
	FIG03_stack.	resize	(0);
	FIG07_value	= 0;
	FIG08_stack.	resize	(0);
	FIG013_stack.	resize	(0);
	FIG014_stack.	resize	(0);
}

int	fibConfig::freeSpace	() {
int amount = 0;
	for (auto &ss: FIG01_stack) 
	   amount += ss. Length;
	return 864 - amount;
}

bool	fibConfig::in_FIG01_stack (const FIG01 &el) {
	for (auto &f: FIG01_stack) 
	   if (f. subChId == el. subChId)
	      return true;
	return false;
}

bool	fibConfig::in_FIG03_stack (const FIG03 &el) {
	for (auto &f: FIG03_stack)
	   if (f. SCId == el. SCId)
	      return true;
	return false;
}

bool	fibConfig::in_FIG08_stack (const FIG08 &el) {
	for (auto &f: FIG08_stack)
	   if (f. SId == el. SId)
	      return true;
	return false;
}

bool	fibConfig::in_FIG013_stack (const FIG013 &el) {
	for (auto &f: FIG013_stack)
	   if (f.SId == el. SId)
	      return true;
	return false;
}

bool	fibConfig::in_FIG014_stack (const FIG014 &el) {
	for (auto &f: FIG014_stack)
	   if (f. subChId == el. subChId)
	      return true;
	return false;
}

