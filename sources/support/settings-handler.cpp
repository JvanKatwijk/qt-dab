#
/*
 *    Copyright (C)  2015 .. 2023
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
#include	<QSettings>
#include	"settings-handler.h"


void	store (QSettings *s, QString paragraph, QString key, QString v) {
	s	-> beginGroup (paragraph);
	s	-> setValue (key, v);
	s	-> endGroup ();
}

void	store (QSettings *s, QString paragraph, QString key, int value) {
	s	-> beginGroup (paragraph);
	s	-> setValue (key, value);
	s	-> endGroup ();
}

int	value_i (QSettings *s, QString paragraph, QString key, int def) {
int	res;
	s	-> beginGroup (paragraph);
	res	= s  ->  value (key, def). toInt ();
	s	-> endGroup ();
	return res;
}

float	value_f (QSettings *s, QString paragraph, QString key, float def) {
float     res;
        s       -> beginGroup (paragraph);
        res     = s -> value (key, def). toFloat ();
        s       -> endGroup ();
        return res;
}

QString	value_s (QSettings *s, QString paragraph, QString key, QString def) {
QString	res;
	s	-> beginGroup (paragraph);
	res	= s -> value (key, def). toString ();
	s	-> endGroup ();
	return res;
}

void	remove (QSettings *s, QString paragraph, QString key) {
	s	-> beginGroup (paragraph);
	s	-> remove (key);
	s	-> endGroup ();
}

