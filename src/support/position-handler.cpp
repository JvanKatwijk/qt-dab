#
/*
 *    Copyright (C)  2015, 2023
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
#include	"position-handler.h"
#include	<QWidget>

void	set_position_and_size (QSettings * settings,
	                       QWidget *w, const QString &key) {
	settings	-> beginGroup (key);
	int x	= settings	-> value (key + "-x", 100). toInt ();
	int y	= settings	-> value (key + "-y", 100). toInt ();
	int wi	= settings	-> value (key + "-w", 300). toInt ();
	int he	= settings	-> value (key + "-h", 200). toInt ();
	settings	-> endGroup ();
	w 	-> resize (QSize (wi, he));
	w	-> move (QPoint (x, y));
}

void	store_widget_position (QSettings *settings,
	                       QWidget *w, const QString &key) {
	settings	-> beginGroup (key);
	settings	-> setValue (key + "-x", w -> pos (). x ());
	settings	-> setValue (key + "-y", w -> pos (). y ());
	settings	-> setValue (key + "-w", w -> size (). width ());
	settings	-> setValue (key + "-h", w -> size (). height ());
	settings	-> endGroup ();
}

