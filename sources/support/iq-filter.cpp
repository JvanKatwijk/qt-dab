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

#include	"iq-filter.h"
#include	"dab-constants.h"
#include	<vector>

	iqFilter:: iqFilter (float alpha) : m_alpha (alpha),
                                            m_mu_I2 (1.0f),
                                            m_mu_Q2 (1.0f),
                                            m_mu_IQ(0.0f) {
}

        iqFilter:: ~iqFilter ()  {}


//	Processes an array of complex samples in-place
void	iqFilter:: process (std::vector<Complex>& samples) {
        size_t n = samples.size();
        
        for (size_t i = 0; i < n; ++i) {
           float I = real (samples [i]);
           float Q = imag (samples [i]);

//	1. Update power and correlation statistics (Exponential Moving Average)
           m_mu_I2 = (1.0f - m_alpha) * m_mu_I2 + m_alpha * (I * I);
           m_mu_Q2 = (1.0f - m_alpha) * m_mu_Q2 + m_alpha * (Q * Q);
           m_mu_IQ = (1.0f - m_alpha) * m_mu_IQ + m_alpha * (I * Q);

//	2. Derive correction factors (Avoid division by zero)
           float i_var = std::max (m_mu_I2, 1e-6f);
           float q_var = std::max (m_mu_Q2, 1e-6f);

//	Phase error coefficient
	   float c_phase = m_mu_IQ / i_var; 
            
//	Amplitude error coefficient (scaled to account for phase matrix)
	   float g_sq	= q_var / i_var - (c_phase * c_phase);
	   float c_gain	= (g_sq > 0.0f) ? (1.0f / std::sqrt(g_sq)) : 1.0f;

//	3. Apply correction (Gram-Schmidt variant)
	   float I_corr = I;
	   float Q_corr = c_gain * (Q - c_phase * I);
	   samples [i] = std::complex<float>(I_corr, Q_corr);
	}
}

Complex	iqFilter:: process (Complex sample) {
float I = real (sample);
float Q = imag (sample);

//	1. Update power and correlation statistics (Exponential Moving Average)
	m_mu_I2 = (1.0f - m_alpha) * m_mu_I2 + m_alpha * (I * I);
	m_mu_Q2 = (1.0f - m_alpha) * m_mu_Q2 + m_alpha * (Q * Q);
	m_mu_IQ = (1.0f - m_alpha) * m_mu_IQ + m_alpha * (I * Q);

//	2. Derive correction factors (Avoid division by zero)
	float i_var = std::max (m_mu_I2, 1e-6f);
	float q_var = std::max (m_mu_Q2, 1e-6f);

//	Phase error coefficient
	float c_phase = m_mu_IQ / i_var; 
            
//	Amplitude error coefficient (scaled to account for phase matrix)
	float g_sq	= q_var / i_var - (c_phase * c_phase);
	float c_gain	= (g_sq > 0.0f) ? (1.0f / std::sqrt(g_sq)) : 1.0f;

//	3. Apply correction (Gram-Schmidt variant)
	float I_corr = I;
	float Q_corr = c_gain * (Q - c_phase * I);
	return Complex (I_corr, Q_corr);
}

//	Reset tracking state if tuning to a new frequency
void	iqFilter::reset () {
	m_mu_I2 = 1.0f;
        m_mu_Q2 = 1.0f;
        m_mu_IQ = 0.0f;
}




