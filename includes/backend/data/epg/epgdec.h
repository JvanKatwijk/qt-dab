/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2006
 *
 * Author(s):
 *	Julian Cable
 *
 * Description:
 *	ETSI DAB/DRM Electronic Programme Guide XML Decompressor
 *
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
\******************************************************************************/

#ifndef _EPGDEC_H
#define _EPGDEC_H
#include	<cstdint>
#include	<vector>
#include	<qdom.h>
using namespace std;

#define	_BYTE uint8_t
class tag_length_value {
public:
	uint8_t tag;
		tag_length_value	(const _BYTE* p);
	bool	is_cdata() const { return tag == 1; }
	bool	is_epg() const { return tag == 2; }
	bool	is_service_information() const { return tag == 3; }
	bool	is_string_token_table() const { return tag == 4; }
	bool	is_default_id() const { return tag == 5; }
	bool	is_child_element() const { return (5<tag) && (tag<0x80); }
	bool	is_attribute() const { return tag>=0x80; }

	size_t length;
	_BYTE* value;
};

class CEPGDecoder {
public:
		CEPGDecoder() {
	        }
	void	decode			(const vector<_BYTE>&,
	                                 const QString &);
	QDomDocument	doc;
};


/* Modified Julian Date ----------------------------------------------------- */
class CModJulDate
{
public:
	CModJulDate() : iYear(0), iDay(0), iMonth(0) {}
	CModJulDate(const uint32_t iModJulDate) {Set(iModJulDate);}

	void Set(const uint32_t iModJulDate);

	int GetYear() {return iYear;}
	int GetDay() {return iDay;}
	int GetMonth() {return iMonth;}

protected:
	int iYear, iDay, iMonth;
};

#endif

