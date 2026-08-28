#
/*
 *	Copyright (C) 2025
 *	Jan van Katwijk (J.vanKatwijk@gmail.com)
 *	Lazy Chair Computing
 *
 *	This file is part of Qt-DAB
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
//	IQ imbalance correction.
//	The most common real-time approach is a blind, adaptive	algorithm
//	based on Gram-Schmidt orthogonalization or statistical estimatio
//
//	Code provides by AI
#pragma once

#include	"dab-constants.h"
#include	<vector>

class	iqFilter {
public:
        iqFilter	(float alpha = 0.0001f);
        ~iqFilter	();
void    process		(std::vector<Complex>& samples);
Complex	process		(Complex);
void	reset		();

private:
// Filter coefficient for tracking (e.g., 0.0001 for slow tracking)
        float m_alpha;

        float m_mu_I2; // Moving average of I^2
        float m_mu_Q2; // Moving average of Q^2
        float m_mu_IQ; // Moving average of I*Q
};




