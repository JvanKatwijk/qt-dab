#
/*
 *    Copyright (C) 2015 .. 2024
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
#include	"journaline-datahandler.h"
#include	"dabdatagroupdecoder.h"
#include	"bit-extractors.h"
#include	"journaline-screen.h"
#include	<sys/time.h>
#include	"newsobject.h"
#include	"NML.h"

static bool running = false;
static
void	my_callBack (const DAB_DATAGROUP_DECODER_msc_datagroup_header_t *header,
	             const unsigned long len, const unsigned char *buf,
	             void *arg) {
	struct timeval theTime;
	gettimeofday (&theTime, NULL);
	(void) header;
	unsigned char buffer [4096];
	(void) arg;
	long unsigned int nmlSize	= 0;
	NML::RawNewsObject_t theBuffer;
	NewsObject hetNieuws (len, buf, &theTime);
	hetNieuws. copyNml (&nmlSize, buffer);
	theBuffer. nml_len	= nmlSize;
	theBuffer. extended_header_len = 0;
	for (uint16_t  i = 0; i < len; i ++)
	   theBuffer. nml [i] = buffer [i];
	RemoveNMLEscapeSequences theRemover;
	NMLFactory xxx;
	NML *ttt = xxx.CreateNML (theBuffer, &theRemover);
	static_cast <journaline_dataHandler *>(arg) -> add_to_dataBase (ttt);
	delete ttt;
}

	journaline_dataHandler::journaline_dataHandler ():
	                                  theScreen (table, &locker){
	theDecoder	= DAB_DATAGROUP_DECODER_createDec (my_callBack, this);
	init_dataBase ();
	connect (this, &journaline_dataHandler::start,
	         &theScreen, &journalineScreen::start);
	running = true;
}

	journaline_dataHandler::~journaline_dataHandler() {
	theScreen. hide ();
	if (!running)
	   return;
	DAB_DATAGROUP_DECODER_deleteDec (theDecoder);
	destroy_dataBase ();
	running = false;
}

//void	journaline_dataHandler::add_mscDatagroup (QByteArray &msc) {
void	journaline_dataHandler::
	             add_mscDatagroup (const std::vector<uint8_t> &msc) {
int16_t	len	= msc. size ();
uint8_t	*data	= (uint8_t *)(msc. data());
uint8_t	*buffer	= (uint8_t *) alloca (len / 8 * sizeof (uint8_t));
int32_t	res;
	if (!theScreen. isVisible ())
	   return;
	for (uint16_t i = 0; i < len / 8; i ++)
	   buffer [i] = getBits (data, 8 * i, 8);
	res = DAB_DATAGROUP_DECODER_putData (theDecoder, len / 8, buffer);
	if (res < 0)
	   return;
}

void	journaline_dataHandler::stop		() {
	theScreen. hide ();
	if (!running)
	   return;
	DAB_DATAGROUP_DECODER_deleteDec (theDecoder);
	destroy_dataBase ();
	running = false;
}

void	journaline_dataHandler::init_dataBase 	() {
	destroy_dataBase ();
	table. resize (0);
}

void	journaline_dataHandler::destroy_dataBase	() {
	for (uint16_t i = 0; i < table. size (); i ++)
	   delete table [i]. element;
}

void	journaline_dataHandler::add_to_dataBase (NML * NMLelement) {
	switch (NMLelement -> GetObjectType ()) {
	   case NML::INVALID:
	      return;
	   case NML::MENU: {
	   case NML::PLAIN:
	   case NML::LIST:
	      NML::News_t  *x	= new NML::News_t;
	      x -> object_id 	= NMLelement	-> GetObjectId ();
	      x -> object_type	= NMLelement	-> GetObjectType ();
	      x -> static_flag	= NMLelement	-> isStatic ();
	      x -> revision_index = NMLelement	-> GetRevisionIndex ();
	      x -> extended_header = NMLelement	-> GetExtendedHeader ();
	      x -> title	= NMLelement	-> GetTitle ();
	      x -> item		= NMLelement	-> GetItems ();
	      int index_oldElement = findIndex (x -> object_id);
	      if (index_oldElement >= 0) {
	         locker. lock ();
	         NML::News_t *p = table [index_oldElement]. element;
	         delete p;
	         table [index_oldElement]. element = x;
	         table [index_oldElement]. updated = true;
	         locker. unlock ();
	         break;
	      }
	      tableElement temp;
	      temp. key	= x -> object_id;
	      temp. element	= x;
	      temp. updated	= false;
	      locker. lock ();
	      table. push_back (temp);
	      locker. unlock ();
	      if (x -> object_id == 0) {
	         start (table. size () - 1);
	      }
	   }
	   break;

	   default:
	      fprintf (stderr, "SOMETHING ELSE\n");
	      break;
	}
}

int	journaline_dataHandler::findIndex (int key) {
int res	= -1;
	locker. lock ();
	for (uint16_t i = 0; i < table. size (); i ++)
	   if (table [i]. key == key) {
	      res = i;
	      break;
	   }
	locker. unlock ();
	return res;
}

