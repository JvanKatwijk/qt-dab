#
/*
 *    Copyright (C) 2018 .. 2025
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

#include	"ensemble.h"

void	ensemble::reset	() {
	isSynced	= false;
	primaries. resize (0);
	secondaries. resize (0);
	namePresent = false;
}

uint32_t  ensemble::serviceToSId	(const QString &s) {
	for (auto &serv: primaries)
	   if (serv. name == s)	
	      return serv. SId;
	for (auto &serv: secondaries)
	   if (serv. name == s)
	      return serv. SId;
	return 0;
}

QString	ensemble::SIdToserv	(uint32_t SId) {
	for (auto &serv: primaries)
	   if (serv. SId == SId)
	      return serv. name;
	return " ";
}

int	ensemble::programType (uint32_t SId) {
	for (auto &serv: primaries)
	   if (serv. SId == SId)
	      return serv. programType;
	return -1;
}

std::vector<int>	ensemble::fmFrequencies	(uint32_t SId) {
std::vector<int> res;
	for (auto &serv: primaries)
	   if (serv. SId == SId) {
	      for (auto freq: serv. fmFrequencies)
	         res. push_back (freq);
	      return res;
	   }
	return res;
}

