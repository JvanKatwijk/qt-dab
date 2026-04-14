#
/*
 *    Copyright (C) 2016 .. 2023
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
 */
//
#include	"channel-descriptor.h"


		channelDescriptor::channelDescriptor	() {}
	
		channelDescriptor::~channelDescriptor	() {}

void	channelDescriptor::setName	(const QString &name) {
	this	-> channelName	= name;
}

void	channelDescriptor::add	(serviceDescriptor sd) {
	for (auto &ssd : channelData) 
	   if ((ssd. serviceName == sd. serviceName) &&
	       (ssd. SID == sd. SID))
	      return;
	sd. channelName	= this -> channelName;
	sd. isFavorite	= false;
	channelData. push_back (sd);
}

void	channelDescriptor::update	(const serviceDescriptor &sd, bool f) {
	for (auto &ssd : channelData) {
	   if (ssd. serviceName == sd. serviceName) {
	      ssd. isFavorite = f;
	      return;
	   }
	}
}

std::vector<serviceDescriptor>
	channelDescriptor::getData	(int Mode) {
	std::vector<serviceDescriptor> res;
	for (auto &sd: channelData) {
	   if (((Mode == FAVORITEVIEW) && sd. isFavorite) || (Mode == ENSEMBLEVIEW))
	      res. push_back (sd);
	}
	return res;
}
