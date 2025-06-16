#
/*
 *    Copyright (C) 2013 .. 2017
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
 *    along with Qt-SDR; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include	"dab-constants.h"
#include	"dll-driver.h"
#include	"rtlsdr-handler.h"

#define READLEN_DEFAULT (4 * 8192)

static
void    RTLSDRCallBack (uint8_t *buf, uint32_t len, void *ctx) {
rtlsdrHandler   *theStick       = (rtlsdrHandler *)ctx;
 
        if ((theStick == nullptr) || (len != READLEN_DEFAULT)) {
           fprintf (stderr, "%d \n", len);
           return;
        }
 
        static_cast<rtlsdrHandler *>(ctx) -> processBuffer (buf, len);
}

	dll_driver::dll_driver (rtlsdrHandler *d) {
	theStick	= d;
	start();
}

	dll_driver::~dll_driver() {
}

void	dll_driver::run () {
	(theStick -> rtlsdr_read_async) (theStick -> theDevice,
	                          (rtlsdr_read_async_cb_t)&RTLSDRCallBack,
	                          (void *)theStick,
	                          0,
	                          READLEN_DEFAULT);
}
