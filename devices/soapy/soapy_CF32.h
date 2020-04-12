#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation recorder 2 of the License.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef	SOAPY_CF32
#define	SOAPY_CF32

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>
#include        <SoapySDR/Errors.hpp>
#include <stdio.h> //printf
#include <stdlib.h> //free
#include <complex.h>

#include	"soapy-worker.h";


class	soapy_CF32: public soapyWorker {
public:
	soapy_CF32	(SoapySDR::Device *);
	~soapy_CF32	(void);
int	Samples		(void);
int	getSamples	(std::complex<float> *, int);
void	run		(void);
private:
SoapySDR::Device	*theDevice;
SoapySDR::Stream	*stream;
RingBuffer<std::complex<float>> *theBuffer;
bool	running;
};

#endif

