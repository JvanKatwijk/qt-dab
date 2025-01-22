#
/*
 *    Copyright (C) 2014 .. 2024
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
 *
 *      Main program
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <memory>
#include <vector>

class DAB_Viterbi_Decoder_Internal;

class viterbi {
public:
	static constexpr size_t m_constraint_length = 7;
	static constexpr size_t m_code_rate = 4;
	viterbi		(int, bool f = false);
    	~viterbi	();
void	deconvolve	(const int16_t *, uint8_t *);
void	convolve	(uint8_t *input, uint8_t *out, int blockLength);

private:
	std::unique_ptr<DAB_Viterbi_Decoder_Internal> m_decoder;
	uint64_t m_accumulated_error;
	int	frameBits;
	int	nrInputValues;
};
