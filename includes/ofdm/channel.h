#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDR-J 
 *    SDR-J is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDR-J is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__CHANNEL_H
#define	__CHANNEL_H

#ifdef	__WITH_JAN__
#include	<Eigen/Dense>
#include	<QString>
#include	"dab-constants.h"
#include	<vector>
using namespace	Eigen;

//	The processor for estimating the channel(s) of a single
//	symbol
class	channel {
public:
		channel		(std::vector<std::complex<float>> &, int, int);
		~channel	();
	void	estimate	(std::complex<float> *, std::complex<float> *);
private:
	int16_t		numberofCarriers;
	int16_t		numberofPilots;
	int16_t		numberofTaps;
	int16_t		fftSize;
	typedef Matrix<std::complex<float>, Dynamic, Dynamic> MatrixXd;
	typedef Matrix<std::complex<float>, Dynamic, 1> Vector;
	MatrixXd	F_p;
	MatrixXd	S_p;
	MatrixXd	S_pxF_p;
	std::vector<int16_t>	pilotTable;
};

#endif
#endif

