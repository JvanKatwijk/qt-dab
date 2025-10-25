/*
 *    Copyright (C) 2025
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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

#include	"dl2-handler.h"

//	Experimental code for handling DL2 data
	dl2_handler::dl2_handler	(const QString &channel,
	                                 const QString &ensembleName,
	                                 const QString &serviceName,
	                                 QSettings *ss):
	                                   theFilenameFinder (ss) {
	this	-> channel	= channel;
	this	-> ensembleName	= ensembleName;
	this	-> serviceName	= serviceName;
}

	dl2_handler::~dl2_handler	() {}
static QString previousComposer;
static uint8_t old_IT	= 0;
void	dl2_handler::show_dl2	(uint8_t ct, uint8_t IT, const QString &s) {
static	QString	title		= "";
static	QString	composer	= "";
static	QString	stationName	= "";
static	QString	programNow	= "";

	if (IT != old_IT) {
	   title	= "";
	   composer	= "";
	   stationName	= "";
	   programNow	= "";
	}
	old_IT = IT;

	QString	fileName = theFilenameFinder. basicPath ();
	if (!fileName. endsWith ("/"))
	   fileName += "/";
	fileName	+= "DL2_titles.csv";
	QDateTime theTime	= QDateTime::currentDateTime ();
	QString front		= theTime. toString () + ";" +
	                          channel + ";" +
	                          ensembleName + ";" + serviceName + ";";;
	QString res;
	switch (ct) {
	   case 1:	// the title
	      title = s;
	      if (composer != "") {
	         res = front + title + ";" + composer + ";";
	         title		= "";
	         composer	= "";
	      }
	      break;
	   case 4:	// the artist
	   case 8:	// the composer
	   case 9:	// the band
	      if (previousComposer == "")
	         previousComposer = s;
	      else
	      if (previousComposer. startsWith (s) ||
	          s. startsWith (previousComposer) ||
	          s == previousComposer)
	      break;
	      previousComposer = s;
	      composer = s;
	      if (title != "") {
	         res = front + title + ";" + composer + ";";
	         title		= "";
	         composer	= "";
	      }
	      break;
	   case 31:	// stationname short
	   case 32:	// stationname long
	      stationName	= s;
	      if  (programNow != "") {	
	         res =  front + stationName + ";" + programNow + ";";
	         stationName	= "";
	         programNow	= "";
	      }
	      break;
	   case 33:	// program now
	      programNow	= s;
	      if  (stationName != "") {	
	         res =  front + stationName + ";" + programNow + ";";
	         stationName	= "";
	         programNow	= "";
	      }
	      break;
	   default:	
	      break;
	}
	if (res != "") {
	   FILE *dlTextFile	= fopen (fileName. toUtf8 (). data (), "a+");
	   if (dlTextFile != nullptr) {
	      fprintf (dlTextFile, "%s\n", res. toLatin1 (). data ());
	      fclose (dlTextFile);
	   }
	}
}
