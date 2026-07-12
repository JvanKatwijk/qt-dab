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
	{0.30, 0x0000FF}, 
	{0.40, 0x00FF00}, 
	{0.50, 0x0FF000},
	{1.00, 0xFF0000},
};

	audioLevel::audioLevel (QWidget *parent)
	                                 : QWidget(parent) {
	resize (8, 200);
}
void	audioLevel::setLevel (qreal level) {
	level = (level + 20) * 6; 
	if (m_level != level) {
	   m_level = level;
	   update();
	}
}

QColor	audioLevel::colorMix (double level) {
int32_t p = 3;

	for (int i = 0; i < 5 - 1; i ++) {
	   if (level <= colorStops [i + 1]. level) {
	      p = i;
	      break;
	   }
	}

	double x0	= colorStops [p]. level;
	QColor cx0 (colorStops [p]. color);
	double x1	= colorStops [p + 1]. level;
	QColor cx1 (colorStops [p + 1]. color);
//	set up the mix
	double  xx =  (level - x0) / (x1 - x0);
	xx = xx < 0 ? 0 : xx >= 1 ? 1 : xx;
	return {
	   static_cast<uint32_t>
	            (cx0. red () + xx * (cx1. red () - cx0. red ())),
	   static_cast<uint32_t>
	            (cx0. green () + xx * (cx1. green () - cx0. green ())),
	   static_cast<uint32_t>
	            (cx0. blue ()  + xx * (cx1. blue () - cx0. blue () ))
	};
}

void	audioLevel::paintEvent (QPaintEvent *event) {
Q_UNUSED(event);
QPainter painter (this);

	int base	= 0.7 * height ();
	if (m_level > height ())
	   m_level = height () - 1;
	int level	= (height () - m_level);

	for (int i = 0; i < level; i ++) {
	   float currPos = float (i) / height ();
	   painter. setPen (colorMix (currPos));
	   int32_t Y_base = height () - i;
	   painter. drawLine (0, Y_base,  width (), Y_base);
	}
}


