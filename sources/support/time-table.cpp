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
//
#include	"time-table.h"
#include	"radio.h"
#include	"settingNames.h"
#include	"settings-handler.h"
#include	"findfilenames.h"
#include	<QFile>
#include	<QMessageBox>
#include	<QDir>
//
//	The timeTableHandler cooperates with the epg handling, in that
//	the latter puts the SPI/EPG files in a directory, named
//	after the Eid of the ensemble, the files are coming from
//	and the timeTablehandler extracts - when a call is made for
//	showing a time table - the relevant xml file, decodes it
//	and shows the result.
//	
	timeTableHandler::timeTableHandler (const QString &path,
	                                    QSettings *dabSettings_p):
	                                          superFrame (nullptr) {

	path_for_files	= path;
	QHBoxLayout *lo	= new QHBoxLayout;
	left	= new QPushButton ("prev");
	left	-> setToolTip ("set the date one day back");
	serviceLabel	= new QLabel ();
	serviceLogo	= new QLabel ();
	dateLabel	= new QLabel ();
	right	= new QPushButton ("next");
	right	-> setToolTip ("set the date one day forward");
	rem	= new QPushButton ("remove");
	QString qss = QString("background-color:red");
	rem	-> setStyleSheet(qss);
	rem	-> setToolTip ("remove the xml files for the current date");
	lo	-> addWidget (left);
	lo	-> addWidget (serviceLabel);
	lo	-> addWidget (serviceLogo);
	lo	-> addWidget (dateLabel);
	lo	-> addWidget (right);
	lo	-> addWidget (rem);

	QVBoxLayout	*lv = new QVBoxLayout ();
	programDisplay	= new QTableWidget (0, 4);
	programDisplay	-> setColumnWidth (0, 150);
	programDisplay	-> setColumnWidth (1, 150);
	programDisplay	-> setColumnWidth (2, 20);
	programDisplay	-> setColumnWidth (3, 450);
	lv		-> addLayout (lo);
	lv		-> addWidget (programDisplay);
	this 	       -> setLayout (lv);

	show ();
	connect (left, &QPushButton::clicked,
	         this, &timeTableHandler::handleLeft);
	connect (right, &QPushButton::clicked,
	         this,  &timeTableHandler::handleRight);
//	programDisplay	-> setHorizontalHeaderLabels (
//	                     QStringList () << "program guide");
	addRow ();
	connect (rem, &QPushButton::clicked,
	         this, &timeTableHandler::deleteFiles);
}

	timeTableHandler::~timeTableHandler   () {
	clear ();
	delete	programDisplay;
}

QString timeTableHandler::find_xmlFile (QDate& theDate,
	                                uint32_t Eid, uint32_t Sid) {
	char temp [40];
	const char * formatString;
	formatString = "%4d%02d%02d_%4X_SI.xml";
	sprintf (temp, formatString,
	                 theDate. year (), theDate. month (),
	                 theDate. day (), Sid);
	return path_for_files + QString (temp);
}
  
void	timeTableHandler::setUp		(const QDate &theDate,
	                                 uint32_t Eid, uint16_t SId,
	                                 const QString &serviceName) {
	this	-> startDate	= theDate;
	this	-> currentDate	= theDate;
	this	-> currentEid	= Eid;
	this	-> ensembleId	= Eid;
	this	-> serviceId	= SId;
	this	-> serviceName	= serviceName;
	dateLabel	-> setText (theDate. toString ());
	serviceLabel	-> setText (serviceName);
	serviceLogo	-> setPixmap (QPixmap ());
	dateOffset	= 0;
	start (dateOffset);
}
//
//	The setup function is called, but it is not certain
//	that we have a logo, so the logo is handled separately
void	timeTableHandler::addLogo	(const QPixmap &p) {
	serviceLogo -> setPixmap (p. scaled (70, 70, Qt::KeepAspectRatio));
}

void	timeTableHandler::handleLeft	() {
	dateOffset --;
	clear ();
	start (dateOffset);
}
	
void	timeTableHandler::handleRight	() {
	dateOffset ++;
	clear ();
	start (dateOffset);
}


void	timeTableHandler::start (int dateOffset) {
	currentDate	= startDate. addDays (dateOffset);
	dateLabel	-> setText (currentDate. toString ());
	serviceLabel	-> setText (serviceName);
	QString fileName = find_xmlFile (currentDate, ensembleId,
	                                       serviceId);
	QFile f = QFile (QDir::toNativeSeparators (fileName));
	if (!f. open (QIODevice::ReadOnly)) {
	   serviceLabel -> setText ("no datafile available");
//	   fprintf (stderr, "Looking for %s\n", fileName. toLatin1 (). data ());
	   show ();
	   return;
	}
	QDomDocument doc;
	doc. setContent (&f);
	f. close ();
	QDomElement root = doc. firstChildElement ("epg");
	if (root. isNull ()) {
	   serviceLabel -> setText ("no Information");
	   show ();
	   return;
	}
	QDomElement theSchedule = root. firstChildElement ("schedule");
	if (theSchedule. isNull ()) {
	   serviceLabel -> setText ("No information");
	   show ();
	   return;
	}
	scheduleDescriptor d =
	       process_schedule (theSchedule, currentDate, 
	                         ensembleId, serviceId);
	if (!d. valid) {
	   serviceLabel -> setText ("Not enough information");
	}
	display (d);
	show ();
}

scheduleDescriptor timeTableHandler::
	            process_schedule (const QDomElement &theSchedule,
	                              QDate theDate,
	                              uint32_t &ensembleId,
	                              uint32_t &serviceId) {
xmlExtractor xmlHandler;
scheduleDescriptor theDescriptor =
	                  xmlHandler. getScheduleDescriptor (theSchedule,
	                                                     theDate,
	                                                     ensembleId,
	                                                     serviceId);
	if (!theDescriptor. valid)
	   return theDescriptor;;
	             
	QDate startDate	= theDescriptor. startTime. date ();
	QDate stopDate	= theDescriptor. stopTime. date ();
	if ((startDate > theDate) || (stopDate < theDate))
	   return theDescriptor;
	for (QDomElement child = theSchedule. firstChildElement ("programme");
	     !child. isNull ();
	     child = child. nextSiblingElement ("programme")) {
	   programDescriptor res = xmlHandler. process_programme (child);
	   if (!res. valid)
	      continue;
	  theDescriptor.thePrograms. push_back (res);
	}
	theDescriptor. name = serviceName;
	return theDescriptor;
}

void	timeTableHandler::display	(const scheduleDescriptor &schedule) {
//	addHeader (schedule);
	for (auto &program : schedule. thePrograms)
	   addProgram (program);
	show ();
}

void	timeTableHandler::addHeader	(const scheduleDescriptor &schedule) {
int	row	= programDisplay -> rowCount ();
QString name	= schedule. name;
QDate	date	= schedule. startTime. date ();
QString start	= schedule. startTime. date (). toString ();
QString	stop	= schedule. stopTime. date (). toString ();
	programDisplay	-> insertRow (row);
	QTableWidgetItem *item0 = new QTableWidgetItem;
	item0	-> setTextAlignment (Qt::AlignLeft);
	name	= name + "(" + QString::number (schedule. Sid, 16). toUpper () + ")";
	item0	-> setText (name);
	programDisplay -> setItem (row, 0, item0);
	QTableWidgetItem *item1 = new QTableWidgetItem;
	item1	-> setTextAlignment (Qt::AlignRight);
	item1	-> setText (date. toString ());
	programDisplay -> setItem (row, 1, item1);
	QTableWidgetItem *item2 = new QTableWidgetItem;
	item2	-> setTextAlignment (Qt::AlignLeft);
//	item2	-> setText (start);
	programDisplay -> setItem (row, 2, item2);
	QTableWidgetItem *item3 = new QTableWidgetItem;
	item3	-> setTextAlignment (Qt::AlignLeft);
	item3	-> setText (stop);
	programDisplay -> setItem (row, 3, item3);
}

void	timeTableHandler::addProgram (const programDescriptor &program) {
int	row	= programDisplay -> rowCount ();
QString start		= program. startTime. time (). toString ();
QString duration;
	if (program. duration < 60)
	   duration	= QString::number (program. duration) + "M";
	else
	   duration	= QString::number (program. duration / 60) + "H " +
	                  QString::number (program. duration % 60) + "M";
	programDisplay	-> insertRow (row);
	QTableWidgetItem *item0 = new QTableWidgetItem;
	item0	-> setTextAlignment (Qt::AlignLeft);
	item0	-> setText (start);
	programDisplay -> setItem (row, 0, item0);
	QTableWidgetItem *item1 = new QTableWidgetItem;
	item1	-> setTextAlignment (Qt::AlignRight);
	item1	-> setText (duration);
	programDisplay -> setItem (row, 1, item1);
	QTableWidgetItem *item2 = new QTableWidgetItem;
	item2	-> setTextAlignment (Qt::AlignLeft);
	item2	-> setText ("");
	programDisplay -> setItem (row, 2, item2);
	QTableWidgetItem *item3 = new QTableWidgetItem;
	item3	-> setTextAlignment (Qt::AlignLeft);
	QString bodyText;
	if (program. longDescriptor. size () >= 10)
	   bodyText = program. longDescriptor;
	else
	if (program. shortDescriptor. size () >= 10)
	   bodyText = program. shortDescriptor;
	else
	if (program. longName. size () >= 10)
	   bodyText = program. longName;
	else
	if (program. mediumName. size () >= 10)
	   bodyText = program. mediumName;
	else
	if (program. longName. size () > 0)
	   bodyText = program. longName;
	else
	if (program. mediumName. size () > 0)
	   bodyText = program. mediumName;
	else
	if (program. shortName. size () > 0)
	   bodyText = program. shortName;
	else
	   bodyText = "No Information";
	bodyText = bodyText. replace ("\n", " ");
	if (bodyText. size () >= 45)
	   bodyText = bodyText. left (55) + "...";
	item3	-> setText (bodyText);
	programDisplay -> setItem (row, 3, item3);
//	fprintf (stderr,
//	    "<tr>\n<th>%s</th>\n<th>%s</th>\n<th>%s</th>\n</tr>\n",
//	      start. toLatin1 (). data (),
//	      duration. toLatin1 (). data (),
//	      bodyText. toLatin1 (). data ());
}

void	timeTableHandler::addRow	() {	// add empty row
int	row	= programDisplay -> rowCount ();
	programDisplay	-> insertRow (row);
	for (int i = 0; i < 4; i ++) {
	   QTableWidgetItem *item = new QTableWidgetItem;
	   item	-> setText ("");
	   programDisplay -> setItem (row, i, item);
	}
}

void	timeTableHandler::clear () {
int	rows    = programDisplay -> rowCount ();
	for (int i = rows; i > 0; i --)
	   programDisplay -> removeRow (i - 1);
}

//void	timeTableHandler::show      () {
//	this	-> show ();
//}
//
//void	timeTableHandler::hide      () {
//	this	-> hide ();
//}

bool	timeTableHandler::isVisible () {
	return !this -> isHidden ();
}

static
QString	subString (const QString &s, int start, int length) {
QString res;
	for (int i = start; i < start + length; i ++)
	   res. push_back (s. at (i));
	return res;
}

void	timeTableHandler::deleteFiles () {
QString dirName = path_for_files + QString::number (currentEid, 16). toUpper () + "/";
QDir directory (dirName);
QStringList files = directory.entryList(QStringList() << "*.xml" ,QDir::Files);
	
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question (nullptr, "delete files", "Are you sure?",
	                               QMessageBox::Yes|QMessageBox::No);
	if (reply == QMessageBox::Yes) {
	   fprintf (stderr, "we gaan deleten\n");
	} else {
	   fprintf (stderr, "cancelen\n");
	   return;
	}
	for (auto &s : files) {
	   bool ok = false;
	   int year	= subString (s, 0, 4). toInt (&ok);
	   if (!ok)
	      continue;
	   int month	= subString (s, 4, 2). toInt (&ok);
	   if (!ok)
	      continue;;
	   int day	= subString (s, 6, 2). toInt (&ok);
	   if (!ok)
	      continue;
	   if (QDate (year, month, day) == currentDate) {
//	      fprintf (stderr, "we gaan %s deleten\n", s. toLatin1 (). data ());
	      directory. remove (s);
	   }
	}
}

