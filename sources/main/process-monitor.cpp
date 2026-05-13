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
#include	"process-monitor.h"
#include        "radio.h"
#include	"dab-constants.h"

	processMonitor::processMonitor (RadioInterface *theRadio) {
	this	-> theRadio	= theRadio;
	myWidget	= new QScrollArea (nullptr);
        myWidget	-> setWidgetResizable (true);

        contentWidget   = new QTableWidget (0, 5);
        contentWidget   -> setColumnWidth (0, 150);
        myWidget        -> setWidget (contentWidget);
        contentWidget   -> setHorizontalHeaderLabels (
                                  QStringList () << tr ("service") <<
	                          tr ("SId") << tr ("SCIds") <<
	                          tr ("subChId") << tr ("Mode"));

	connect (contentWidget, &QTableWidget::cellClicked,
	         this, &processMonitor::process_clickRequest);
	connect (this, &processMonitor::handle_clickRequest,
	         theRadio, &RadioInterface::handle_clickRequest);
}

	processMonitor::~processMonitor () {
        clearTable ();
        delete  contentWidget;
        delete  myWidget;
}

void	processMonitor::show      () {
	myWidget	-> show ();
}

void	processMonitor::hide      () {
	myWidget	-> hide ();
}

bool	processMonitor::isVisible () {
        return !myWidget -> isHidden ();
}

void	processMonitor::addProcess	(const QString &s, uint32_t SId,
	                                 uint8_t SCIds, uint8_t subChId,
	                                 int mode) {
int rowCount	= contentWidget	-> rowCount ();

	for (int row = 0; row < rowCount; row ++) {
	   if (contentWidget -> item (row, 0) -> text () != s)
	      continue;
	   if (contentWidget -> item (row, 1) -> text () !=
	                                 QString::number (SId, 16). toUpper ())
	      continue;
	   if (contentWidget -> item (row, 2) -> text () !=  
	                                 QString::number (SCIds))
	      continue;
	   if (contentWidget -> item (row, 3) -> text () !=  
	                                 QString::number (subChId))
	      continue;
	   return;
	}

	contentWidget     -> insertRow (rowCount);

	QTableWidgetItem *item0 = new QTableWidgetItem;
	item0	-> setTextAlignment (Qt::AlignLeft | Qt::AlignVCenter);
	item0	-> setText (s);
	contentWidget     -> setItem (rowCount, 0, item0);

	QTableWidgetItem *item1 = new QTableWidgetItem;
	item1	-> setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
	item1	-> setText (QString::number (SId, 16). toUpper ());
	contentWidget     -> setItem (rowCount, 1, item1);

	QTableWidgetItem *item2 = new QTableWidgetItem;
	item2	-> setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
	item2	-> setText (QString::number (SCIds));
	contentWidget     -> setItem (rowCount, 2, item2);

	QTableWidgetItem *item3 = new QTableWidgetItem;
	item3	-> setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
	item3	-> setText (QString::number (subChId));
	contentWidget     -> setItem (rowCount, 3, item3);

	QTableWidgetItem *item4 = new QTableWidgetItem;
	item4	-> setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
	item4	-> setText (QString::number (mode));
	contentWidget     -> setItem (rowCount, 4, item4);
}

void	processMonitor::removeProcess	(const QString &s,
	                                 uint32_t SId, uint8_t SCIds) {
int rowCount = contentWidget	-> rowCount ();

int	found	= -1;
	for (int row = 0; row < rowCount; row ++) {
	   if (contentWidget -> item (row, 0) -> text () != s)
	      continue;
	   if (contentWidget -> item (row, 1) -> text () !=
	                                 QString::number (SId, 16). toUpper ())
	      continue;
	   if (contentWidget -> item (row, 2) -> text () !=  
	                                 QString::number (SCIds))
	      continue;
	   found = row;
	   break;
	}

	if (found == -1)
	   return;

	contentWidget	-> removeRow (found);
}

void    processMonitor::clearTable        () {
int     rows    = contentWidget -> rowCount ();
        for (int i = rows; i > 0; i --)  
           contentWidget -> removeRow (i - 1);
}

void	processMonitor::process_clickRequest	(int row, int column) {
QTableWidgetItem* theItem = contentWidget  -> item (row, 0);	
	if (row == 0)
	   return;
        (void)column;
        emit handle_clickRequest (theItem -> text ());
}
	

