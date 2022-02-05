#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation recorder 2 of the License.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"tii-codes.h"
#include	<QMessageBox>

#ifdef __MINGW32__
#define GETPROCADDRESS  GetProcAddress
#else
#define GETPROCADDRESS  dlsym
#endif

	tiiHandler::tiiHandler	() {
	Handle		= dlopen ("libtii-lib.so", RTLD_NOW | RTLD_GLOBAL);
	fprintf (stderr, "%s\n", dlerror ());
	if (Handle == nullptr)
	   Handle	= dlopen ("/usr/local/lib/tii-lib.so",
	                                           RTLD_NOW | RTLD_GLOBAL);
	fprintf (stderr, "%s\n", dlerror ());
	if (Handle == nullptr)
	   fprintf (stderr, "Library not loaded\n");
//	set the defaults
	init_tii_L	= nullptr;
	close_tii_L	= nullptr;
	tiiFileL	= nullptr;
	loadTableL	= nullptr;
	get_transmitterNameL	= nullptr;
	get_coordinatesL	= nullptr;
	distanceL	= nullptr;
	cornerL		= nullptr;
	is_blackL	= nullptr;
	set_blackL	= nullptr;
	loadFunctions	();
	if (init_tii_L != nullptr)
	   handler	= init_tii_L ();
	else
	   handler	= nullptr;
}

	tiiHandler::~tiiHandler	() { 
	if (close_tii_L != nullptr)
	   close_tii_L (handler);
}

bool	tiiHandler::tiiFile         (const QString &s) {
	if (tiiFileL != nullptr)
	   return tiiFileL (handler, s. toStdString ());
	return false;
}

QString	tiiHandler::get_transmitterName     (const QString & channel,
	                                     uint16_t Eid,
	                                     uint8_t mainId, uint8_t subId) {
std::string res;

	if (get_transmitterNameL != nullptr)
	   res = get_transmitterNameL (handler,
	                               channel. toStdString (),
	                               Eid, mainId, subId);
	else
	  res = "";
	return QString::fromStdString (res);
}

void	tiiHandler::get_coordinates	(float *lat, float *lon,
	                                 const QString &channel,
	                                 const QString &name) {
	if (get_coordinatesL != nullptr)
	   get_coordinatesL (handler, lat, lon,
	                     channel. toStdString (), name. toStdString ());
	else {
	   *lat = 0;
	   *lon = 0;
	}
}

int	tiiHandler::distance		(float latitude1, float longitude1,
	                                 float latitude2, float longitude2) {
	if (distanceL != nullptr)
	   return distanceL (handler, latitude1, longitude1,
	                                 latitude2, longitude2);
	else
	   return 0;
}

int	tiiHandler::corner		(float latitude1, float longitude1,
	                                 float latitude2, float longitude2) {
	if (cornerL != nullptr)
	   return cornerL (handler, latitude1, longitude1,
	                                 latitude2, longitude2);
	else
	   return 0;
}

bool	tiiHandler::is_black		(uint16_t Eid,
	                                 uint8_t mainId, uint8_t subId) {
	if (is_blackL != nullptr)
	   return is_blackL (handler, Eid, mainId, subId);
	else
	   return false;
}

void	tiiHandler::set_black		(uint16_t Eid,
	                                 uint8_t mainId, uint8_t subId) {
	if (set_blackL != nullptr) 
	   set_blackL (handler, Eid, mainId, subId);
}

void	tiiHandler::loadTable		(const QString &tf) {
	if (loadTableL != nullptr)
	   loadTableL (handler, tf. toStdString ());
}

bool	tiiHandler::valid		() {
	return handler != nullptr;
}

bool	tiiHandler::loadFunctions	() {
	init_tii_L	= (init_tii_P)
                            GETPROCADDRESS (this -> Handle,
                                            "init_tii_L");
	if (init_tii_L == nullptr)
	   fprintf (stderr, "init_tii_L not loaded\n");

	close_tii_L	= (close_tii_P)
                            GETPROCADDRESS (this -> Handle,
                                            "close_tii_L");
	tiiFileL	= (tiiFileP)
                            GETPROCADDRESS (this -> Handle,
                                            "tiiFileL");
	if (tiiFileL == nullptr)
	   fprintf (stderr, "tiiFileL not loaded\n");
	loadTableL	= (loadTableP)
                            GETPROCADDRESS (this -> Handle,
                                            "loadTableL");
	get_transmitterNameL	= (get_transmitterNameP)
                            GETPROCADDRESS (this -> Handle,
                                            "get_transmitterNameL");
	if (get_transmitterNameL == nullptr)
	   fprintf (stderr, "could not load transmitterName\n");
	get_coordinatesL	= (get_coordinatesP)
                            GETPROCADDRESS (this -> Handle,
                                            "get_coordinatesL");
	distanceL	= (distanceP)
                            GETPROCADDRESS (this -> Handle,
                                            "distanceL");
	cornerL		= (cornerP)
                            GETPROCADDRESS (this -> Handle,
                                            "cornerL");
	is_blackL	= (is_blackP)
                            GETPROCADDRESS (this -> Handle,
                                            "is_blackL");
	set_blackL	= (set_blackP)
                            GETPROCADDRESS (this -> Handle,
                                            "set_blackL");
	return true;
}

