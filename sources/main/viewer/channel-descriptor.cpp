#
/*
 *    Copyright (C) 2016 .. 2026
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


		theChannel::theChannel	() {}
	
		theChannel::~theChannel	() {}

void	theChannel::setName	(const QString &name) {
	this	-> channelName	= name;
}

void	theChannel::add	(serviceDescriptor sd) {
	for (auto &ssd : channelData) 
	   if ((ssd. serviceName == sd. serviceName) &&
	       (ssd. SID == sd. SID))
	      return;
	sd. channelName	= this -> channelName;
//	sd. isFavorite	= false;
	channelData. push_back (sd);
}

void	theChannel::update	(const serviceDescriptor &sd, bool f) {
	for (auto &ssd : channelData) {
	   if (ssd. serviceName == sd. serviceName) {
	      ssd. isFavorite = f;
	      return;
	   }
	}
}

std::vector<serviceDescriptor>
	theChannel::getData	(int Mode, int order) {
	std::vector<serviceDescriptor> res;
	for (auto &sd: channelData) {
	   if (((Mode == FAVORITEVIEW) && sd. isFavorite) ||
	                                      (Mode == ENSEMBLEVIEW))
	      res. push_back (sd);
	}
	sort (res, order);
	return res;
}
static
int	fcmp_id	(const void *a, const void *b) {
	serviceDescriptor *el1	= (serviceDescriptor *)a;
	serviceDescriptor *el2	= (serviceDescriptor *)b;
	if ((uint32_t)el1 -> SID > (uint32_t)el2 -> SID)
	   return -1;
	if ((uint32_t)el1 -> SID < (uint32_t)el2 -> SID)
	   return 1;
	return 0;
}

static
int	fcmp_subCh	(const void *a, const void *b) {
	serviceDescriptor *el1	= (serviceDescriptor *)a;
	serviceDescriptor *el2	= (serviceDescriptor *)b;
	if ((uint16_t)el1 -> subChId > (uint16_t)el2 -> subChId)
	   return -1;
	if ((uint16_t)el1 -> SID < (uint32_t)el2 -> subChId)
	   return 1;
	return 0;
}

static
int	fcmp_alpha	(const void *a, const void *b) {
	serviceDescriptor *el1	= (serviceDescriptor *)a;
	serviceDescriptor *el2	= (serviceDescriptor *)b;
	if (el1 -> serviceName > el2 -> serviceName)
	   return -1;
	if (el1 -> serviceName < el2 -> serviceName)
	   return 1;
	return 0;
}

void	theChannel::sort	(std::vector<serviceDescriptor> &res, 
	                                             int order) {
	switch (order) {
	   case ID_BASED:
	      qsort (res. data (), res. size (),
	               sizeof (serviceDescriptor), &fcmp_id);
	      break;
	   case SUBCH_BASED:
	      qsort (res. data (), res. size (),
	               sizeof (serviceDescriptor), &fcmp_subCh);
	      break;
	   default:		// should not happen
	   case ALPHA_BASED:
	      qsort (res. data (), res. size (),
	               sizeof (serviceDescriptor), &fcmp_alpha);
	      break;
	}
}

