#
/*
 *    Copyright (C) 2016 .. 2023
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
#include	"channel-viewer.h"
#include	"database.h"

	channelViewer::channelViewer (serviceBase *theDataBase) {
	myWidget	= new QScrollArea (nullptr);
        myWidget	-> setWidgetResizable (true);

	this	-> theDataBase	= theDataBase;
        contentWidget   = new QTableWidget (0, 6);
        contentWidget   -> setColumnWidth (0, 50);
        contentWidget   -> setColumnWidth (1, 150);
        contentWidget   -> setColumnWidth (2, 100);
        contentWidget   -> setColumnWidth (3, 100);
        contentWidget   -> setColumnWidth (4, 100);
        myWidget        -> setWidget (contentWidget);
        contentWidget   -> setHorizontalHeaderLabels (
                                  QStringList () << tr ("channel") <<
	                          tr ("ensemble") << tr ("country") <<
	                          tr ("audioserv") << tr ("dataserv") << 
	                          tr ("secondary"));
}

	channelViewer::~channelViewer () {
        clear ();
        delete  contentWidget;
        delete  myWidget;
}

void	channelViewer::show	() {
	myWidget	-> show ();
}

void	channelViewer::hide      () {
	myWidget	-> hide ();
}

bool	channelViewer::isVisible () {
        return !myWidget -> isHidden ();
}

void    channelViewer::clear        () {
int     rows    = contentWidget -> rowCount ();
        for (int i = rows; i > 0; i --)  
           contentWidget -> removeRow (i - 1);
}

void	channelViewer::reload		() {
	for (auto &ch : theDataBase -> theData) {
	   QString channelName	= ch. channelName;
	   QString ensembleName = ch. ensembleName;
	   QString countryName	= ch. countryName;
	   int nrAudio		= 0;
	   int nrPacket		= 0;
	   int nrSecond		= 0;
	   for (auto serv : ch. channelData) {
	      if (((serv. SId & 0xFF0000) == 0) && (serv. SCIds == 0))
	         nrAudio ++;
	      else
	      if (serv. SCIds > 0)
	         nrSecond ++;
	      else
	         nrPacket ++;
	   }
	   int rowCount = contentWidget -> rowCount ();
	   contentWidget     -> insertRow (rowCount);

	   QTableWidgetItem *item0 = new QTableWidgetItem;
	   item0	-> setTextAlignment (Qt::AlignLeft | Qt::AlignVCenter);
	   item0	-> setText (channelName);
	   contentWidget     -> setItem (rowCount, 0, item0);

	   QTableWidgetItem *item1 = new QTableWidgetItem;
	   item1	-> setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
	   item1	-> setText (ensembleName);
	   contentWidget     -> setItem (rowCount, 1, item1);

	   QTableWidgetItem *item2 = new QTableWidgetItem;
	   item2	-> setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
	   item2	-> setText (countryName);
	   contentWidget     -> setItem (rowCount, 2, item2);

	   QTableWidgetItem *item3 = new QTableWidgetItem;
	   item3	-> setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
	   item3	-> setText (QString::number (nrAudio));
	   contentWidget     -> setItem (rowCount, 3, item3);

	   QTableWidgetItem *item4 = new QTableWidgetItem;
	   item4	-> setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
	   item4	-> setText (QString::number (nrPacket));
	   contentWidget     -> setItem (rowCount, 4, item4);

	   QTableWidgetItem *item5 = new QTableWidgetItem;
	   item5	-> setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
	   item5	-> setText (QString::number (nrSecond));
	   contentWidget     -> setItem (rowCount, 5, item5);
	}
}
