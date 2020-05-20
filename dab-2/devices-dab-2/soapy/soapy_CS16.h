#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation recorder 2 of the License.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2 if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef	SOAPY_CS16
#define	SOAPY_CS16

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>
#include        <SoapySDR/Errors.hpp>
#include <stdio.h> //printf
#include <stdlib.h> //free
#include <complex.h>

#include	"soapy-worker.h"

class	dabProcessor;

class	soapy_CS16: public soapyWorker {
public:
		soapy_CS16	(SoapySDR::Device *, dabProcessor *);
		~soapy_CS16	(void);
	void	run		(void);
private:
	SoapySDR::Device	*theDevice;
	SoapySDR::Stream	*stream;
	dabProcessor		*base;
	bool	running;
};

#endif

