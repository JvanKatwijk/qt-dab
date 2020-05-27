#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include	"timesyncer.h"
#include	"sample-reader.h"

#define C_LEVEL_SIZE    50

	timeSyncer::timeSyncer (sampleReader *mr) {
	myReader	= mr;
}

	timeSyncer::~timeSyncer() {}

int	timeSyncer::sync (int T_null, int T_F) {
float	cLevel		= 0;
int	counter		= 0;
float	envBuffer       [syncBufferSize];
const
int	syncBufferMask	= syncBufferSize - 1;
int	i;

	syncBufferIndex = 0;
	for (i = 0; i < C_LEVEL_SIZE; i ++) {
	   std::complex<float> sample        = myReader -> getSample (0);
	   envBuffer [syncBufferIndex]       = jan_abs (sample);
	   cLevel                            += envBuffer [syncBufferIndex];
	   syncBufferIndex ++;
	}
//SyncOnNull:
	counter      = 0;
	while (cLevel / C_LEVEL_SIZE  > 0.55 * myReader -> get_sLevel()) {
	   std::complex<float> sample        =
	         myReader -> getSample (0);
//	         myReader. getSample (coarseOffset + fineCorrector);
	   envBuffer [syncBufferIndex] = jan_abs (sample);
//      update the levels
	   cLevel += envBuffer [syncBufferIndex] -
	        envBuffer [(syncBufferIndex - C_LEVEL_SIZE) & syncBufferMask];
	   syncBufferIndex = (syncBufferIndex + 1) & syncBufferMask;
           counter ++;
           if (counter > T_F) { // hopeless
	      return NO_DIP_FOUND;
           }
        }
/**
  *     It seemed we found a dip that started app 65/100 * 50 samples earlier.
  *     We now start looking for the end of the null period.
  */
	counter      = 0;
//SyncOnEndNull:
	 while (cLevel / C_LEVEL_SIZE < 0.75 * myReader -> get_sLevel()) {
	   std::complex<float> sample =
	           myReader -> getSample (0);
	   envBuffer [syncBufferIndex] = jan_abs (sample);
//      update the levels
	   cLevel += envBuffer [syncBufferIndex] -
	         envBuffer [(syncBufferIndex - C_LEVEL_SIZE) & syncBufferMask];
	   syncBufferIndex = (syncBufferIndex + 1) & syncBufferMask;
	   counter   ++;
	   if (counter > T_null + 50) { // hopeless
	      return NO_END_OF_DIP_FOUND;
	   }
	}

	return TIMESYNC_ESTABLISHED;
}
