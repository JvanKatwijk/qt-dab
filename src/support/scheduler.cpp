#
/*
 *    Copyright (C) 2021
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
#include	"scheduler.h"
#include	"radio.h"
#include	"dab-constants.h"
#include	<stdio.h>
#include	<iostream>
#include	<fstream>
#include	<string>
#define	SWITCHTIME	15
#define	MINUTES_PER_DAY	(24 * 60)
//
///
//	Note
//	In this version, we maintain time as being relative to 1 jan
//	wakeupTime is given in minutes,
//	currentTime is eventually computed in seconds
//	the time difference then is in seconds and the delay in msec
	Scheduler::Scheduler (RadioInterface *mr):
	                              myWidget (nullptr) {
	myWidget. resize (240, 200);
	myWidget. setWidgetResizable(true);

	tableWidget 	= new QTableWidget (0, 3);
	tableWidget	-> setColumnWidth (0, 120);
	myWidget. setWidget(tableWidget);
	tableWidget 	-> setHorizontalHeaderLabels (
	            QStringList () << tr ("service name     ") << tr ("daynr") << tr ("time"));
	connect (tableWidget, SIGNAL (cellDoubleClicked (int, int)),
	         this, SLOT (removeRow (int, int)));
	wakeupTimer. setSingleShot (true);
	wakeupTimer. setInterval   (1000000);
	connect (&wakeupTimer, SIGNAL (timeout ()),
	         this, SLOT (handle_timeOut ()));
	connect (this, SIGNAL (timeOut (const QString &)),
	         mr, SLOT (scheduler_timeOut (const QString &)));
	this	-> wakeupTime = 365 * 24 * 60;
}

	Scheduler::~Scheduler () {
int16_t	i;
int16_t	rows	= tableWidget -> rowCount ();
	myWidget. hide ();
	wakeupTimer. stop ();
	for (i = rows; i > 0; i --)
	   tableWidget -> removeRow (i);
	delete	tableWidget;
}

void	Scheduler::show	() {
	myWidget. show ();
}

void	Scheduler::hide	() {
	myWidget. hide ();
}

void	Scheduler::clear	() {
int16_t	rows	= tableWidget -> rowCount ();
	myWidget. hide ();
	wakeupTimer. stop ();
	for (int i = rows; i > 0; i --)
	   tableWidget -> removeRow (i);
}

void	Scheduler::addExternalSchedule	(const QString &fileName) {
std::ifstream f (fileName. toLatin1 (). data ());
QDate currentDate = QDate::currentDate ();

	std::string str;
	size_t amount	= 256;
	while (true) {
	   if (!std::getline (f, str))
	      break;

//	just to be on the safe side
	   if (str. size () < 10)
	      continue;
	   QStringList res = QString::fromStdString (str). split ("	");
	   if (res. size () != 2)
	      continue;
	   QStringList t = QString (res [1]). split (":");
	   if (t. size () != 2)
	      continue;
	   int hours	= t [0]. toInt ();
	   int minutes	= t [1]. toInt ();
	   if ((hours < 0) || (hours >= 24))
	      continue;
	   if ((minutes < 0) || (minutes >= 60))
	      continue;
	   addRow (res [0], currentDate. dayOfYear (),  hours, minutes);
	}
}

//	To allow schedules to the next day, we compute the times
//	relative to 1 januari
void	Scheduler::addRow (const QString &name, int day,
	                              int hours, int minutes) {
int16_t	row		= tableWidget -> rowCount ();
QDate	currentDate 	= QDate::currentDate ();
QTime	current		= QTime::currentTime ();
int	dayOfYear	= currentDate. dayOfYear ();
int	wakeupTime	= hours * 60 + minutes + (day - 1) * MINUTES_PER_DAY;
QString	recordTime	= QString::number (hours / 10) +
	                           QString::number (hours % 10);
int64_t currentTime	= ((dayOfYear - 1) * MINUTES_PER_DAY + current. hour () * 60 + current. minute ()) * 60 + current. second ();

QDate	wakeupDate	= QDate::currentDate ();	// default

	wakeupTimer. stop ();
	if (day - currentDate. dayOfYear () > 0) {
	   wakeupDate = wakeupDate. addDays (day - currentDate. dayOfYear ());
	}
	recordTime. append (":");
	recordTime. append (QString::number (minutes / 10) +
	                             QString::number (minutes % 10));

	tableWidget	-> insertRow (row);
	QTableWidgetItem *item0	= new QTableWidgetItem;
	item0		-> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	tableWidget	-> setItem (row, 0, item0);

	QTableWidgetItem *item1 = new QTableWidgetItem;
	item1		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 1, item1);

	QTableWidgetItem *item2 = new QTableWidgetItem;
	item2		-> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	tableWidget	-> setItem (row, 2, item2);

	tableWidget	-> setCurrentItem (item0);
	tableWidget	-> item (row, 0) -> setText (name);
	tableWidget	-> item (row, 1) -> setText (QString::number (day));
	tableWidget	-> item (row, 2) -> setText (recordTime);
	if (wakeupTime < this -> wakeupTime) {
	   this -> wakeupTime = wakeupTime;
	   this	-> wakeupIndex = tableWidget -> rowCount () - 1;
	};

	int tempDelay	= wakeupTime * 60 - currentTime;
	fprintf (stderr, "waiting time %d hours %d minutes\n",
	                       tempDelay / 60 / 60, tempDelay / 60 %60);
	int theDelay	= this -> wakeupTime * 60  - currentTime - SWITCHTIME;
	if (theDelay <= 0) 
	   theDelay = 1000;	// milliseconds
	else
	   theDelay = 1000 * theDelay;
	if (theDelay > 600000)
	   theDelay = 600000;
	wakeupTimer. setInterval (theDelay);
	wakeupTimer. start (theDelay);
	show ();
}
//
void	Scheduler::removeRow (int row, int column) {
QDate	currentDate = QDate::currentDate ();
int	dayOfYear	= currentDate. dayOfYear ();
	(void) column;
	wakeupTimer. stop ();
	tableWidget	-> removeRow (row);
	if (tableWidget -> rowCount () == 0) {
	   wakeupTime	= 365 * MINUTES_PER_DAY;
	   wakeupTimer. stop ();
	   hide ();
	   return;
	}
//
//	and recompute the wakeupTime
	for (int i = 0; i < tableWidget -> rowCount (); i ++) {
	   QString testTime = tableWidget -> item (i, 2) -> text ();
	   QStringList test = testTime. split (":");
	   int hours	= test. at (0). toInt ();
	   int minutes	= test. at (1) . toInt ();
	   int day = tableWidget -> item (i, 1) -> text (). toInt ();
	   int targetTime = hours * 60 + minutes;
	   targetTime += (day - 1) * MINUTES_PER_DAY;
	   if (targetTime < wakeupTime) {
	      wakeupTime = targetTime;
	      wakeupIndex = i;
	   }
	}

	QTime current = QTime::currentTime ();
	int64_t currentTime = (dayOfYear - 1) * MINUTES_PER_DAY + current. hour () * 60 + current. minute ();
	currentTime	= currentTime * 60 + current. second ();
	int64_t theDelay
	                = wakeupTime * 60 - currentTime - SWITCHTIME; // seconds
	if (theDelay <= 0) 
	   theDelay = 1000;	// milliseconds
	else
	   theDelay = 1000 * theDelay;

	if (theDelay > 600000)
	   theDelay = 600000;
	wakeupTimer. setInterval (theDelay);
	wakeupTimer. start (theDelay);
}
//
//	Each 10 minutes we check the time, to avoid too large
//	delays
void	Scheduler::handle_timeOut () {
QString	service	= tableWidget -> item (wakeupIndex, 0) -> text ();
QTime current = QTime::currentTime ();
int64_t currentTime = current. hour () * 60 + current. minute ();
QDate currentDate	= QDate::currentDate ();

	currentTime += (currentDate. dayOfYear () - 1) * MINUTES_PER_DAY;
//
//	we need seconds
	currentTime = currentTime * 60 + current. second ();

	if (currentTime >= wakeupTime * 60 - SWITCHTIME) {
	   wakeupTime	= 365 * MINUTES_PER_DAY;	// minutes
	   removeRow (wakeupIndex, 0);
	   emit timeOut (service);
	   return;
	}

	int64_t theDelay	=
	           wakeupTime * 60 - currentTime - SWITCHTIME; // seconds
	if (theDelay <= 500) 
	   theDelay = 1000;	// milliseconds
	else
	   theDelay = 1000 * theDelay;	// in millseconds

	if (theDelay > 600000)
	   theDelay = 600000;
	wakeupTimer. setInterval (theDelay);
	wakeupTimer. start (theDelay);
}

