#
/*
 *    Copyright (C)  2026
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

#include        "font-chooser.h"
#include        <QStringList>
#include        <QSettings>
#include        <QtXml>
#include        <QColorDialog>
#include        <QFontDialog>
#include        <QHeaderView>
#include	<QHBoxLayout>
#include	<QVBoxLayout>
#include        "settingNames.h"
#include        "settings-handler.h"
#include	"settings-handler.h"
#include	"view-handler.h"

//
	serviceViewer::serviceViewer (const QString &fileName,
	                              const QStringList &channels,
	                              QSettings	*serviceSettings,
	                              QFrame *theFrame): 
	                                        theDataBase (fileName),
	                                        normalFont ("Times", 10, 
                                                              -1, false),
                                                markedFont ("Times", 12,
                                                              -1, true),
                                                channelFont ("Times", 8) {

	this	-> fileName		= fileName;
	this	-> viewSettings	= serviceSettings;
	this	-> theFrame		= theFrame;

	setupUi (theFrame);
	QScrollArea *ensembleWidget	= new QScrollArea;
	QTableWidget *theTable	= new QTableWidget (0, 3);
	theTable	 -> setSelectionBehavior (QAbstractItemView::SelectRows);
	theTable	-> setHorizontalHeaderLabels (
	                             QStringList () << tr ("fav") <<
	                                               tr ("service") <<
	                                               tr ("channel"));
	connect (theTable, &QTableWidget::cellClicked,
	         this, &serviceViewer::clickOnService);
	ensembleWidget	-> setWidget (theTable);

	theMode		= ENSEMBLEVIEW;
	for (auto &s: channels)
	   channelSelector -> addItem (s);
	connect (channelSelector, 
	         qOverload<const QString &> (&QComboBox::textActivated),
	         this, &serviceViewer::handle_channelSelector);
	connect (prevService, &QPushButton::clicked,
	         this, &serviceViewer::handle_prevService);
	connect (nextService, &QPushButton::clicked,
	         this, &serviceViewer::handle_nextService);
	connect (viewSelector, &QPushButton::clicked,
	         this, &serviceViewer::handle_modeSwitcher);
	connect (prevChannel, &clickablelabel::clicked_left,
	         this, &serviceViewer::handle_prevChannel);
	connect (nextChannel, &clickablelabel::clicked_left,
	         this, &serviceViewer::handle_nextChannel);
	channelSelector	-> setEnabled (false);
	prevChannel	-> setEnabled (false);
	nextChannel	-> setEnabled (false);
	currentService	= 0;
}

	serviceViewer::~serviceViewer	() {
	   theDataBase. store (fileName);
}

void	serviceViewer::startMode	(int Mode) {
	clearTable ();
	displayList. resize (0);
	theMode		= Mode;
	switch (Mode) {
	   case ENSEMBLEVIEW:
//	      extract from the database een displayview en show
	      channelSelector	-> setEnabled (true);
	      prevChannel	-> setEnabled (true);
	      nextChannel	-> setEnabled (true);
              displayList	= theDataBase. getData (theMode);
              show_displayList ();
	      break;
	   case FAVORITEVIEW:		// cannot happen
	      channelSelector	-> setEnabled (false);
	      prevChannel	-> setEnabled (false);
	      nextChannel	-> setEnabled (false);
              displayList	= theDataBase. getData (theMode);
              show_displayList ();
	      break;
	   case FILEINPUT:
	      channelSelector	-> setEnabled (false);
	      prevChannel	-> setEnabled (false);
	      nextChannel	-> setEnabled (false);
	      break;
	   default:	// cannot happen
	      break;
	}
	currentService	= -1;
}

QString	serviceViewer::currentChannel	() {
	return channelSelector	-> currentText ();
}
//
//	In ensembleview mode, services mght be added
void	serviceViewer::addService	(serviceDescriptor sd) {
QString channel	= channelSelector -> currentText ();
	for (auto &ssd: displayList) 
	   if ((ssd. serviceName == sd. serviceName) &&
	       (ssd. channelName == channel))
	      return;

	switch (theMode) {
	   case ENSEMBLEVIEW:
	      sd. channelName	= channel;
	      displayList. push_back (sd);
	      theDataBase. add (sd);
	      insert (sd);	// in the table
	      break;
	   case FILEINPUT:
	      sd. channelName	= "";
	      sd. isFavorite	= false;
	      displayList. push_back (sd);
	      insert (sd);
	      break;	
	   case FAVORITEVIEW:
	   default:		// cannot happen
	      break;
	}
}
//
//
//	It is verified that the service does not exist yet in the view
void	serviceViewer::insert	(const serviceDescriptor &sd) {
int row	= theTable -> rowCount ();
	displayList. push_back (sd);
	theTable     -> insertRow (row);     // 
        QTableWidgetItem *item0 = new QTableWidgetItem;
        item0           -> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
        theTable     -> setItem (row, 0, item0);

	QTableWidgetItem *item1 = new QTableWidgetItem; // serviceName
        item1           -> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        theTable     -> setItem (row, 1, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem; // channel
        item2           -> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        theTable     -> setItem (row, 2, item2);

	theTable	-> item (row, 0) -> setText (sd. isFavorite ? "*" :"");
	theTable	-> item (row, 1) -> setText (sd. serviceName);
	theTable	-> item (row, 2) -> setText (sd. channelName);
}

void	serviceViewer::setServiceOrder	(int order) {
	(void)order;
}

void	serviceViewer::mark (int index) {
	theTable -> item (index, 1) -> setFont (markedFont);
	theTable -> item (index, 2) -> setFont (markedFont);
}

void	serviceViewer::unmark (int index) {
	theTable -> item (index, 1) -> setFont (normalFont);
	theTable -> item (index, 2) -> setFont (normalFont);
}

int	serviceViewer::getMode () {
	return theMode;
}
//	from e.g. the window showing the current ensemble, a
//	service might be selected
void	serviceViewer::setupService	(const QString &serviceName) {
	if (theMode == FILEINPUT) {
	   for (int i = 0; i < (int)displayList. size (); i ++) {
	      if (displayList [i]. serviceName == serviceName) {
	         unmark (currentService);
	         currentService = i;
	         mark (currentService);
	         emit selectService (serviceName);
	         return;
	      }
	   }
	}
	else
	   setupService (channelSelector -> currentText (),
	                           serviceName);
}
//	from e.g. the scheduler a service might be selected
void	serviceViewer::setupService (const QString &channel,
	                                       const QString &serviceName) {
	if (theMode == FILEINPUT) {
	   fprintf (stderr, "should not happen\n");
	   return;
	}

	for (int i = 0; i < (int)displayList. size (); i ++) {
	   if ((displayList [i]. serviceName == serviceName) &&
	       (displayList [i]. channelName == channel)) {
	         unmark (currentService);
	         currentService = i;
	         mark (currentService);
	         emit selectService (channel, serviceName);
	         return;
	   }
	}
}

//
//	the displayList and theTable are synced
void	serviceViewer::clickOnService	(int row, int column) {
	if (row == 0)
	   return;	// clicking on the header
	serviceDescriptor t = displayList [row -1];
	if (column == 0) {	// change the fav settings
	   switch (theMode) {
	      case ENSEMBLEVIEW: 
//	mark or unmark the service as favorite
	         displayList [row -1]. isFavorite =	
	                             !displayList [row -1]. isFavorite;
	         theDataBase. update (displayList [row - 1],
	                              displayList [row - 1]. isFavorite);
	         if (displayList [row - 1]. isFavorite)
	            theTable -> item (row, 0) -> setText ("*");
	         else
	            theTable -> item (row, 0) -> setText ("");
	         break;		// cannot happen
	      case FAVORITEVIEW:
	      case FILEINPUT:
	         break;		// no semantics
	      default:		// cannot happen
	         break;
	   }
	}
	else {
	   if (currentService != -1) {
	      unmark (currentService);
	   }
	   switch (theMode) {
	      case ENSEMBLEVIEW:	
	      case FAVORITEVIEW:
	         emit (selectService (displayList [row]. channelName,
	                              displayList [row]. serviceName));
	      
	         break;
	      case FILEINPUT:
	         emit (selectService (displayList [row]. serviceName));
	         break;
	      default:		// cannot happen
	         break;
	   }
	   currentService = row;
	   mark (currentService);
	}
}

void	serviceViewer::handle_nextService	() {
	if (currentService == -1)
	   return;
	theTable -> item (currentService, 1) -> setFont (normalFont);
	theTable -> item (currentService, 2) -> setFont (normalFont);
	currentService	= (currentService + 1) % displayList. size ();
	theTable -> item (currentService, 1) -> setFont (markedFont);
	theTable -> item (currentService, 2) -> setFont (markedFont);
//	emit (selectService (displayList [currentService]. channelName,
//	                     displayList [currentService]. serviceName));
}

void	serviceViewer::handle_prevService	() {
	if (currentService == -1)
	   return;
	theTable -> item (currentService, 1) -> setFont (normalFont);
	theTable -> item (currentService, 2) -> setFont (normalFont);
	currentService	= (currentService - 1 + displayList. size ()) % displayList. size ();
	theTable -> item (currentService, 1) -> setFont (markedFont);
	theTable -> item (currentService, 2) -> setFont (markedFont);
//	emit (selectService (displayList [currentService]. channelName,
//	                     displayList [currentService]. serviceName));
}

//
//	Only active when in mode ENSEMBLEVIEW

void	serviceViewer::handle_channelSelector	(const QString &channel) {
	emit setChannel (channel);
}

void	serviceViewer::handle_nextChannel	() {
	int currentChannel	= channelSelector -> currentIndex ();
	currentChannel 		= (currentChannel + 1) %
	                                  channelSelector -> count ();
	emit setChannel (channelSelector -> currentText ());
}
	
void	serviceViewer::handle_prevChannel	() {
	int currentChannel	= channelSelector -> currentIndex ();
	currentChannel 		= (currentChannel - 1 + channelSelector -> count ()) %
	                                  channelSelector -> count ();
	emit setChannel (channelSelector -> currentText ());
}

void	serviceViewer::handle_modeSwitcher	() {
	if (theMode == FILEINPUT)
	   return;
	clearTable ();
	displayList. resize (0);
	theMode	= (theMode == ENSEMBLEVIEW) ? FAVORITEVIEW : ENSEMBLEVIEW;
	displayList	= theDataBase. getData (theMode);
	show_displayList ();
}

void	serviceViewer::handleFontSelect		() {
int fontSize;
	fontSize	= value_i (viewSettings, ENSEMBLE,
	                                         "fontSize", 10);
fontChooser selectFont ("select font");
QStringList fontList;
QString	theFont;
	fontList << QString ("Times");
	fontList << QString ("Helvetica");
	fontList << QString ("Arial");
	fontList << QString ("Cantarell");
	fontList << QString ("Sans");
	fontList << QString ("Courier");
	fontList << QString ("TypeWriter");

	for (auto &s : fontList)
	   selectFont. add (s);
	int fontIndex	= selectFont. QDialog::exec ();
	theFont	= fontList. at (fontIndex);
	store (viewSettings, ENSEMBLE, "theFont", theFont);
	normalFont	= QFont (theFont, fontSize, -1, false);
	markedFont	= QFont (theFont, fontSize + 2, -1, true);
	channelFont	= QFont (theFont, fontSize - 2);
	updateFonts	();
}

void	serviceViewer::handleFontColorSelect () {
QColor	color;

	color	= QColorDialog::getColor (color, nullptr, "fontColor");
	if (!color. isValid ())
	   return;
	QString fontColor	= color. name ();
	store (viewSettings, ENSEMBLE, "fontColor", fontColor);
	updateFonts ();
}

void	serviceViewer::handleFontSizeSelect	(int fontSize) {
QString	theFont	= viewSettings -> value ("theFont", "Times"). toString ();

	if (fontSize < 8) 
	   return;
	store (viewSettings, ENSEMBLE, "fontSize", fontSize);
	normalFont	= QFont (theFont, fontSize, -1, false);
	markedFont	= QFont (theFont, fontSize + 2, -1, true);
	channelFont	= QFont (theFont, fontSize - 2);
	updateFonts ();
}

void	serviceViewer::updateFonts	() {
	for (int i = 0; i < theTable -> rowCount (); i ++) {
	   if (i == currentService) {
	      theTable -> item (i, 1) -> setFont (markedFont);
	      theTable -> item (i, 2) -> setFont (markedFont);
	   }
	   else {
	      theTable -> item (i, 1) -> setFont (normalFont);
	      theTable -> item (i, 2) -> setFont (normalFont);
	   }
	}
}

void	serviceViewer::clearTable	() {
	for (uint16_t i = 0; i < theTable -> rowCount (); i ++)
	   for (uint16_t j = 0; j < theTable -> columnCount (); j ++)
	      theTable -> item (i, j) -> setText (" ");
	for (uint16_t i = 6; i < theTable -> rowCount (); i ++)
           theTable -> removeRow (i);
}

void	serviceViewer::show_displayList	() {
//	assert that the table is empty
	QString fontColor = value_s (viewSettings, ENSEMBLE,
	                                     "fontColor", fontColor);

	if (theTable -> rowCount () > 1) 
	   return;
	for (auto &dt: displayList) {
	   QString serviceName	= dt. serviceName;
	   QString channel	= dt. channelName;
	   bool	isFavorite	= dt. isFavorite;
	   int row = theTable -> rowCount ();
	   theTable	-> insertRow (row);
	   QTableWidgetItem *item_0 = new QTableWidgetItem;
	   item_0	-> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	   item_0	-> setFlags (Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	   theTable -> setItem (row, 0, item_0);
	   theTable -> item (row, 0) -> setText (isFavorite ? "*" : "");

	   QTableWidgetItem *item_1 = new QTableWidgetItem;
	   item_1	-> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	   theTable -> setItem (row, 1, item_1);
           theTable -> item (row, 1) -> setText (serviceName);
           theTable -> item (row, 1) -> setFont (normalFont);
	   theTable -> item (row, 1) -> setForeground (QColor(fontColor));
	
	   QTableWidgetItem *item_2 = new QTableWidgetItem;
	   item_2	-> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	   theTable -> setItem (row, 2, item_2);
           theTable -> item (row, 2) -> setText (serviceName);
           theTable -> item (row, 2) -> setFont (normalFont);
	   theTable -> item (row, 2) -> setForeground (QColor(fontColor));
	}
}
//
//	to be handled later on
void	serviceViewer::handleRightMouseClick	(const QString &text) {
	(void)text;
}

