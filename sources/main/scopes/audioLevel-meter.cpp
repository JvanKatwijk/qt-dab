#
/*
 *    Copyright (C)  2022 .. 2026
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
#include "audioLevel-meter.h"
#include <QPainter>

static
struct {float	level;
	uint32_t color;
} colorStops [] = {
	{0.00, 0x000000},
	{0.20, 0x0000FF}, 
	{0.45, 0x00FF00},
	{0.66, 0xFFFF00},
	{1.00, 0xFF0000},
};

	audioLevel::audioLevel (QWidget *parent)
	                                 : QWidget(parent) {
//	resize (8, 200);
}
int	min	= 10000;
int	max	= -10000;
int	teller	= 0;

void	audioLevel::setLevel (qreal level) {
	if (level < min)
	   min = level;
	if (level > max)
	   max = level;
	if (++teller > 50) {
	   fprintf (stderr, "%d %d\n", min, max);
	   min	= 10000;
	   max	= -10000;
	   teller = 0;
	}
	level = (level + 20) * 6; 
	if (m_level != level) {
	   m_level = level;
	   update();
	}
}

QColor	audioLevel::colorMix (double level) {
int32_t seg = 2;

	for (int i = 0; i < 5 - 1; i ++) {
	   if (level <= colorStops [i + 1]. level) {
	      seg = i;
	      break;
	   }
	}

	double t0	= colorStops [seg]. level;
	double t1	= colorStops [seg + 1]. level;
	QColor c0 (colorStops [seg]. color);
	QColor c1 (colorStops [seg + 1]. color);
	double frac = (t1 > t0) ?
	       std::clamp ((level - t0) / (t1 - t0),
	                    (double)0.0, (double)1.0) : 0.0;
	return {
	   static_cast<int32_t>
	            (c0. red () + frac * (c1. red () - c0. red ())),
	   static_cast<int32_t>
	            (c0. green () + frac * (c1. green() - c0. green ())),
	   static_cast<int32_t>
	            (c0. blue ()  + frac * (c1.blue () - c0. blue ()))
	};
}

void	audioLevel::paintEvent (QPaintEvent *event) {
Q_UNUSED(event);
QPainter painter (this);

	int base	= 0.7 * height ();
	int level	= (height () - m_level);

	for (int i = 0; i < level; i ++) {
	   float currPos = float (i) / height ();
	   painter. setPen (colorMix (currPos));
	   int32_t Y_base = height () - i;
	   painter. drawLine (0, Y_base,  width (), Y_base);
	}
}


