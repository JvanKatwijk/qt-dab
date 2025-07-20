#
/*
 *    Copyright (C) 2014 .. 2023
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

#pragma once
#include	<stdio.h>
#include	<cstring>
#include	<stdexcept>

class	device_exception: public std::exception {
private:
	std::string message;
public:
	device_exception (const std::string &s) {
	   message = s;
	}
	const char * what () const noexcept override {
	   return message. c_str ();
	}
};

//class	rtl_tcp_exception : public std::exception {
//private:
//	std::string message;
//public:
//	rtl_tcp_exception (const std::string &s) {
//           message      = s;
//        }
//
//        const char *what  () const noexcept override {
//           return message. c_str ();
//        }
//};

