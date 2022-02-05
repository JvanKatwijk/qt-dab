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
#ifndef	__TII_HANDLER_H
#define	__TII_HANDLER_H
#include	<stdint.h>
#include	<QString>
#include	<QSettings>
#include	"dlfcn.h"
typedef	void	*HINSTANCE;

//	DLL and ".so" function prototypes
typedef void	*(*init_tii_P)	();
typedef void	(*close_tii_P)	(void *);
typedef bool	(*tiiFileP)	(void *, const std::string &);
typedef void	(*loadTableP)	(void *, const std::string &);
typedef	std::string	(*get_transmitterNameP) (void *, const std::string &,
	                                         uint16_t, uint8_t, uint8_t);
typedef	void	(*get_coordinatesP) (void *, float *, float *,
	                                  const std::string &, const std::string &);
typedef	int	(*distanceP)	(void *, float, float, float, float);
typedef int	(*cornerP)	(void *, float, float, float, float);
typedef bool	(*is_blackP)	(void *, uint16_t, uint8_t, uint8_t);
typedef void	(*set_blackP)	(void *, uint16_t, uint8_t, uint8_t);

class	tiiHandler {
public:
		tiiHandler	();
		~tiiHandler	();
	bool	tiiFile		(const QString &);
	QString	get_transmitterName	(const QString &,
	                                 uint16_t, uint8_t, uint8_t);
	void	get_coordinates	(float *, float *, const QString &, const QString &);
        int     distance        (float, float, float, float);
        int     corner          (float, float, float, float);
        bool    is_black        (uint16_t, uint8_t, uint8_t);
        void    set_black       (uint16_t, uint8_t, uint8_t);
        void    loadTable       (const QString &tf);
        bool    valid           ();
private:
	void	*handler;

	HINSTANCE	Handle;
	bool		loadFunctions	();
	init_tii_P	init_tii_L;
	close_tii_P	close_tii_L;
	tiiFileP	tiiFileL;
	loadTableP	loadTableL;
	get_transmitterNameP get_transmitterNameL;
	get_coordinatesP	get_coordinatesL;
	distanceP	distanceL;
	cornerP		cornerL;
	is_blackP	is_blackL;
	set_blackP	set_blackL;
};
#endif


