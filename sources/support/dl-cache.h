#
/*
 *    Copyright (C) 2016 .. 2025
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
#pragma once

#include	<QString>
#include	<vector>
#define	CACHE_SIZE	16
#define CACHE_MASK	(CACHE_SIZE - 1)

	class	dlCache {
private:
	std::vector<QString> cache;
	int	p;
	int	size;
public:
	dlCache	(int size) {
	   (void)size;
	   cache. resize (CACHE_SIZE);
	   this	-> size	= CACHE_SIZE;
	   p = 0;
	}

	dlCache	() {
	}

void	add		(const QString &s) {
	cache [p] = s;
	p = (p + 1) & CACHE_MASK;
}

bool	isMember	(const QString &s) {
	for (int i = 0; i < CACHE_SIZE; i ++)
	   if (cache [i] == s)
	      return true;
	return false;
}

bool	addifNew	(const QString &s) {
	for (uint16_t i = p; i < p + CACHE_SIZE; i ++) {
	   if (cache [i & CACHE_MASK] == s) {
	      for (uint16_t j = i; j < (p - 1) + CACHE_SIZE; j ++)
	         cache [j & CACHE_MASK] = cache [(j + 1) & CACHE_MASK];
	      cache [(p - 1 + CACHE_SIZE) & CACHE_MASK] = s;
	      return true;
	   }
	}
	cache [p] = s;
	p = (p + 1) & CACHE_MASK;
	return false;
}
};

