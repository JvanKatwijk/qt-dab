#
/*
 *    Copyright (C) 2016 .. 2023
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
 *    along with dab-scanner; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	<QSettings>
#include	<QDir>
#include	"timetable-control.h"
#include	"time-table.h"
#include	"findfilenames.h"
#include	"settingNames.h"
#include	"settings-handler.h"

static
QString	checkDir (const QString &s) {
QString	dir = s;

	if (!dir. endsWith (QChar ('/')))
	   dir += QChar ('/');

	if (QDir (dir). exists())
	   return dir;
	QDir (). mkpath (dir);
	return dir;
}

	timeTableControl::timeTableControl (QString &ensemble, 
	                                    uint32_t EId,
	                                    std::vector<basicService> &services,
	                                    QDate today,
	                                    QSettings *dabSettings):
	                                        superFrame (nullptr) {
findfileNames theFilenameFinder (dabSettings);
	this	-> ensemble	= ensemble;
	this	-> EId		= EId;
	this	-> services	= services;
	this	-> today	= today;
	this	-> dabSettings	= dabSettings;

	this	-> serviceHandler		= nullptr;
	contentWidget   = new QTableWidget	(0, 2);
	contentWidget   -> setColumnWidth	(0, 150);
	contentWidget   -> setColumnWidth	(1, 50);
	auto layout	= new QVBoxLayout	();
	layout		-> addWidget		(contentWidget);
	this		-> setLayout (layout);
        contentWidget   -> setHorizontalHeaderLabels (
                                  QStringList () << ensemble << QString::number (EId, 16). toUpper ());
	connect (contentWidget, &QTableWidget::cellClicked,
	         this, &timeTableControl::selectService);

	QString tempPath        = theFilenameFinder. basicPath ();     
        path_for_files          =
                              value_s (dabSettings, DAB_GENERAL,
                                                S_FILE_PATH, tempPath);
	path_for_files               = checkDir (path_for_files);
	if (path_for_files == "")
	   return;
	path_for_files +=
                      QString::number (EId, 16). toUpper () + "/";
        path_for_files = QDir::toNativeSeparators (path_for_files); 
        if (!QDir (path_for_files). exists ())
           QDir (). mkpath (path_for_files);
	
	QString	f1	= QString ("*") + "_SI.xml";
	QString f2	= QString ("*") + "_si.xml";
	QStringList theFilters;
	theFilters << f1 << f2;
	QDir theDirectory (path_for_files);
	theDirectory.setNameFilters (theFilters);
	QStringList files =
	       theDirectory. entryList (theDirectory. nameFilters ());
	show ();
	for (auto &s: services) {
	   element q;
	   q. hasTable		= checkFor (files, s.SId);
	   q. SId		= s. SId;
	   q. serviceName	= s. serviceName;
	   addRow  (s. serviceName, s. SId, q. hasTable);
	   otherTable. push_back (q);
	}
}

	timeTableControl::~timeTableControl () {
	hide ();
	if (serviceHandler != nullptr)
	   delete serviceHandler;
	int rows	= contentWidget -> rowCount ();
	for (int i = rows - 1; i >= 0; i --)
	   contentWidget -> removeRow (i);
	delete contentWidget;
}

void	timeTableControl::addRow (const QString &serviceName, int32_t EId,
	                                       bool hasTables) {
int16_t row     = contentWidget -> rowCount ();

        contentWidget     -> insertRow (row);

	QTableWidgetItem *item0 = new QTableWidgetItem;
	item0           -> setTextAlignment (Qt::AlignLeft |
                                                        Qt::AlignVCenter);
	item0		-> setText (serviceName);
	if (hasTables)
	   item0	-> setData (Qt::BackgroundRole, QColor ("green"));
	else
	   item0	-> setData (Qt::BackgroundRole, QColor ("red"));
	contentWidget	-> setItem (row, 0, item0);

	QTableWidgetItem *item1 = new QTableWidgetItem;
	item1           -> setTextAlignment (Qt::AlignRight |
                                                        Qt::AlignVCenter);
	item1		-> setText (QString::number (EId, 16));
	contentWidget	-> setItem (row, 1, item1);
}

bool	timeTableControl::checkFor (const QStringList &files, int32_t SId) {
QString XXX	= QString::number (SId, 16). toUpper ();
	for (auto &ss : files)
	  if (ss. contains (XXX))
	     return true;
	return false;
}


void	timeTableControl::selectService (int row, int column) {
	QString service	= contentWidget	-> item (row, 1) -> text ();
	bool b;
	int SId		= service. toInt (&b, 16);
	if (!b)
	   return;
	if (serviceHandler != nullptr)
	   delete serviceHandler;
	serviceHandler	= nullptr;
	for (auto &e :otherTable) {
	   if ((e. SId == SId) && e. hasTable) {
	      serviceHandler	= new timeTableHandler (path_for_files,
	                                                   dabSettings);
	      connect (serviceHandler, &superFrame::frameClosed,
	               this, &timeTableControl::cleanup_serviceHandler);
	      serviceHandler	-> setUp (today, EId, SId, e. serviceName);
//	      QOixmap p;
//	      if (get_serviceLogo (p, channel. currentService. SId))
//	         serviceHandler. addLogo (p);
	      break;
	   }
	}
}
//
//	the timetable window was closed, we close the timetable itself
void	timeTableControl::cleanup_serviceHandler () {
	delete serviceHandler;	
	serviceHandler	= nullptr;
}

