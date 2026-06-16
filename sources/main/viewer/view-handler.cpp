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
#include	<QScrollBar>
#include	<QVBoxLayout>
#include        "settingNames.h"
#include        "settings-handler.h"
#include	"view-handler.h"
#include	"radio.h"

#define BLACK   "#000000"
#define GREEN   "#8ff0a4"
#define BLUE    "#00ffff"

//
	serviceViewer::serviceViewer (const QString &fileName,
	                              RadioInterface	*theRadio,
	                              const QStringList &channels,
	                              QSettings	*serviceSettings,
	                              QFrame *theFrame): 
	                                        theDataBase (),
	                                        channelDisplay (&theDataBase),
	                                        normalFont     ("Times", 11, 
                                                                -1, false),
                                                markedFont     ("Times", 12,
                                                                -1, true),
                                                channelFont    ("Times", 8) {

	this	-> defaultName		= fileName;
	this	-> fileName		= fileName;
	this	-> theRadio		= theRadio;
	this	-> viewSettings		= serviceSettings;
	this	-> theFrame		= theFrame;

	setupUi (theFrame);
	theFrame	-> show ();
	theTable	= new QTableWidget (0, 3);
	theTable	 -> setSelectionBehavior (QAbstractItemView::SelectRows);
	theTable	-> setColumnWidth	(0, 2);
	theTable	-> setColumnWidth	(2, 8);
	theTable	-> setHorizontalHeaderLabels (
	                             QStringList () << tr ("") <<
	                                               tr ("service") <<
	                                               tr ("chan"));
	connect (theTable, &QTableWidget::cellClicked,
	         this, &serviceViewer::clickOnService);
	theWidget	-> setWidget (theTable);
	theWidget	-> show ();

	connect (&channelDisplay, &channelViewer::selectChannel,
	         this, &serviceViewer::prepareChannel);
	QPixmap p;
        if (p. load (":res/radio-pictures/up-arrow.png", "png"))
           prevChannel -> setPixmap (p. scaled (30, 30, Qt::KeepAspectRatio));
        if (p. load (":res/radio-pictures/down-arrow.png", "png"))
           nextChannel -> setPixmap (p. scaled (30, 30, Qt::KeepAspectRatio));

	set_Colors ();	// the buttons
	theMode		= ENSEMBLEVIEW;
	serviceOrder	= 0;
	for (auto &s: channels)
	   channelSelector -> addItem (s);
	connect (prevService, &QPushButton::clicked,
	         this, &serviceViewer::handle_prevService);
	connect (nextService, &QPushButton::clicked,
	         this, &serviceViewer::handle_nextService);
	connect (viewSelector, &QPushButton::clicked,
	         this, &serviceViewer::handle_viewSelector);
	connect (channelSelector,
	             qOverload<const QString &> (&QComboBox::textActivated),
	         this, &serviceViewer::handle_channelSelector);
	connect (channelDisplayButton, &QPushButton::clicked,
	         this, &serviceViewer::handle_channelDisplay);
	connect (prevChannel, &clickablelabel::clicked_left,
	         this, &serviceViewer::handle_prevChannel);
	connect (nextChannel, &clickablelabel::clicked_left,
	         this, &serviceViewer::handle_nextChannel);
	connect (this, &serviceViewer::reduceButtons,
	         theRadio, &RadioInterface::reduceButtons);

	connect	(prevService, &smallPushButton::rightClicked,
	         this, &serviceViewer::color_prevService);
	connect	(nextService, &smallPushButton::rightClicked,
	         this, &serviceViewer::color_nextService);
	connect (viewSelector, &smallPushButton::rightClicked,
	         this, &serviceViewer::color_viewSelector);
	connect (channelDisplayButton, &smallPushButton::rightClicked,
	         this, &serviceViewer::color_channelDisplay);

	channelSelector	-> setEnabled (false);
	prevChannel	-> setEnabled (false);
	nextChannel	-> setEnabled (false);
	bool b = value_i (viewSettings, CONFIG_HANDLER, AUDIOSERVICES_ONLY, 0);
	ensembleMode	=  b ? ALL : SINGLE_CHANNEL;
	currentService	= -1;
	QString startingChannel = 
              value_s (viewSettings, DAB_GENERAL, CHANNEL_NAME, "5A");
        int k = channelSelector -> findText (startingChannel);
        if (k != -1) 
           channelSelector -> setCurrentIndex (k);
	connect (this, &serviceViewer::setChannel, 
	         theRadio, &RadioInterface::handle_channelSelector);
	connect (this, &serviceViewer::selectService,
	         theRadio, &RadioInterface::localSelect);

	QFont font		= ensembleId -> font ();
        font. setPointSize (12);
        ensembleId              -> setFont (font);
        font			= countryName   -> font ();
        font. setPointSize (11); 
        countryName		-> setFont (font);
}

	serviceViewer::~serviceViewer	() {
	   theDataBase. store (fileName);
}

void	serviceViewer::clearAll		() {
	theDataBase. clearTable ();
	clearTable ();
	displayList. clear ();
}
//
//	While in most cases the startMode function operates with
//	the default filename, there are situations that
//	a user selectable filename is used
void	serviceViewer::startMode	(int Mode,
	                                 const QString &fileName,
	                                 int order, bool withPackets, bool ff) {
	if (fileName == "") {
	   this -> fileName	= defaultName;
	   startSession (Mode, order, withPackets);
	   return;
	}
	FILE *f;
	if (ff)
	   f	= fopen (fileName. toLatin1 (). data (), "w");
	else
	   f	= fopen (fileName. toLatin1 (). data (), "r");
	clearAll ();
	if (f == nullptr) {
//	   fclose (f);
	   startMode (Mode, order, withPackets);
	   return;
	}
	theDataBase. load (fileName, withPackets);
	this	-> fileName	= fileName;
	startSession (Mode, order, withPackets);
}

void	serviceViewer::startMode	(int Mode,
	                                 int order, bool withPackets) {
	clearAll ();
	this	-> fileName	= defaultName;
	theDataBase. load (fileName, withPackets);
	startSession (Mode, order, withPackets);
}

void	serviceViewer::startSession	(int Mode,
	                                 int order, bool withPackets) {
	theMode		= Mode;
	ensembleMode	= withPackets? SINGLE_CHANNEL : ALL;
	currentService	= -1;
	switch (Mode) {
	   case ENSEMBLEVIEW:
//	      extract from the database een displayview en show
	      channelSelector	-> show ();
	      channelDisplayButton -> show ();
	      prevChannel	-> show ();
	      nextChannel	-> show ();
	      viewSelector	-> show ();
	      channelSelector	-> setEnabled (true);
	      channelDisplayButton -> setEnabled (true);
	      prevChannel	-> setEnabled (true);
	      nextChannel	-> setEnabled (true);
	      if (ensembleMode == ALL)
                 displayList	= theDataBase. getData (theMode, order);
	      else
                 displayList	= theDataBase. getData (theMode,
	                                            order, currentChannel ());
	      viewSelector	-> setEnabled (true);
	      viewSelector	-> setText ("Favorites");
              show_displayList ();
	      break;
	   case FAVORITEVIEW:		// cannot happen
	      channelSelector	-> hide ();
	      channelDisplayButton -> hide ();
	      prevChannel	-> hide ();
	      nextChannel	-> hide ();
	      viewSelector	-> show ();
	      channelSelector	-> setEnabled (false);
	      channelDisplayButton -> setEnabled (false);
	      prevChannel	-> setEnabled (false);
	      nextChannel	-> setEnabled (false);
              displayList	= theDataBase. getData (theMode, order);
	      viewSelector	-> setEnabled (true);
              show_displayList ();
	      break;
	   case FILEINPUT:
	      channelSelector	-> setEnabled (false);
	      channelDisplayButton	-> hide ();
	      prevChannel	-> setEnabled (false);
	      nextChannel	-> setEnabled (false);
	      viewSelector	-> setEnabled (false);
	      channelSelector	-> hide ();
	      channelDisplayButton	-> hide ();
	      prevChannel	-> hide ();
	      nextChannel	-> hide ();
	      viewSelector	-> hide ();
              show_displayList ();
	      break;
	   default:	// cannot happen
	      break;
	}
	currentService	= -1;
}

QString	serviceViewer::currentChannel	() {
	return channelSelector	-> currentText ();
}

//	maps the SId to the servicename
QString	serviceViewer::extractName	(uint32_t SId) {
	for (auto &sd: displayList) 
	   if (sd. SId == SId)
	      return sd. serviceName;
	return "";
}
//
//	In ensembleview mode, services might be added
void	serviceViewer::addService	(const descriptorType & ad) {
QString channel	= channelSelector -> currentText ();
	
	for (auto &ssd: displayList) {
	   if ((ssd. serviceName == ad. serviceName) &&
	       (ssd. channelName == ad. channel))
	      return;
	}

	bool b;
	serviceDescriptor sd;
	sd.	channelName	= ad. channel;
	sd.	channel		= ad. channel. toInt (&b, 16);
	if (!b)
	   sd. channel = 90;
	sd.	serviceName	= ad. serviceName;
	sd.	SId		= ad. SId;
	sd.	SCIds		= ad. SCIds;
	sd.	subChId		= ad. subchId;

	int index	= -1;
	switch (theMode) {
	   case ENSEMBLEVIEW:
	      sd. channelName	= channel;
	      index = add_to_displayList (sd);
	      if (ensembleMode == ALL)
	         theDataBase. add (sd);
	      insert (sd, index);	// in the table
	      break;
	   case FILEINPUT:
	      sd. channelName	= "FILE";
	      sd. isFavorite	= false;
	      displayList. push_back (sd);
	      insert (sd, displayList. size () - 1);
	      break;	
	   case FAVORITEVIEW:
	   default:		// cannot happen
	      break;
	}
}

void	serviceViewer::remove	(const QString &channel,
	                            const QString &service) {
	viewLocker. lock ();
	for (uint16_t i = 0; i < displayList. size (); i ++) {
	   auto &sd = displayList [i];
	   if ((sd. serviceName == service) &&
	       (sd. channelName == channel)) {
	      theTable -> removeRow (i);
	      displayList. erase (displayList. begin () + i);
	   }
	}
	theDataBase. remove (channel, service);
	viewLocker. unlock ();
}
//
//
//	It is verified that the service does not exist yet in the view
void	serviceViewer::insert	(const serviceDescriptor &sd, int pos) {
	if (pos > theTable -> rowCount () + 1) {
	   fprintf (stderr, "H E L P\n");
	   return;
	}
QString fontColor = value_s (viewSettings, ENSEMBLE,
                                             "fontColor", "white");
	viewLocker. lock ();
	theTable     -> insertRow (pos);     // 
        QTableWidgetItem *item0 = new QTableWidgetItem;
	item0           -> setTextAlignment (Qt::AlignLeft |Qt::AlignVCenter);
	theTable     -> setItem (pos, 0, item0);

	QTableWidgetItem *item1 = new QTableWidgetItem; // serviceName
	item1           -> setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	theTable	-> setItem (pos, 1, item1);
	theTable 	-> item (pos, 1) -> setFont (normalFont);
	theTable 	-> item (pos, 1) -> setForeground (QColor(fontColor));

        QTableWidgetItem *item2 = new QTableWidgetItem; // channel
	item2           -> setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	theTable	-> setItem (pos, 2, item2);
	theTable 	-> item (pos, 2) -> setFont (normalFont);
	theTable 	-> item (pos, 2) -> setForeground (QColor(fontColor));

	theTable	-> item (pos, 0) -> setText (sd. isFavorite ? "*" :"");
	theTable	-> item (pos, 1) -> setText (sd. serviceName);
	theTable	-> item (pos, 2) -> setText (sd. channelName);
	viewLocker. unlock ();
}

void	serviceViewer::setServiceOrder	(int order) {
	this	-> serviceOrder = order;
}

void	serviceViewer::mark (int index) {
	viewLocker. lock ();
	theTable -> item (index, 1) -> setFont (markedFont);
	theTable -> item (index, 2) -> setFont (markedFont);
	theTable -> setCurrentItem (theTable -> item (index, 2));
	viewLocker. unlock ();
}

void	serviceViewer::unmark (int index) {
	viewLocker. lock ();
	theTable -> item (index, 1) -> setFont (normalFont);
	theTable -> item (index, 2) -> setFont (normalFont);
	viewLocker. unlock ();
}

int	serviceViewer::getMode () {
	return theMode;
}
//	from e.g. the content window a	service might be selected
void	serviceViewer::reportService	(const QString &serviceName) {
	if (theMode == FILEINPUT) {
	   for (int i = 0; i < (int)displayList. size (); i ++) {
	      if (displayList [i]. serviceName == serviceName) {
	         if (currentService >= 0)
	            unmark (currentService);
	         currentService = i;
	         mark (currentService);
	         return;
	      }
	   }
	}
	else
	   reportService (channelSelector -> currentText (), serviceName);
}

//	from e.g. the scheduler a service might be selected
void	serviceViewer::reportService (const QString &channel,
	                                       const QString &serviceName) {
	if (theMode == FILEINPUT) {
	   fprintf (stderr, "should not happen\n");
	   return;
	}
	
	for (int i = 0; i < (int)displayList. size (); i ++) {
	   if ((displayList [i]. serviceName == serviceName) &&
	       (displayList [i]. channelName == channel)) {
	         if (currentService >= 0)
	            unmark (currentService);
	         currentService = i;
	         mark (currentService);	
	         return;
	   }
	}
}
//
//	the displayList and theTable are synced
void	serviceViewer::clickOnService	(int row, int column) {
	serviceDescriptor t = displayList [row];
	if (column == 0) {	// change the fav settings
	   switch (theMode) {
	      case ENSEMBLEVIEW: 
//	mark or unmark the service as favorite
	         if ((displayList [row]. SId & 0xFFFF0000) != 0)
	            return;
	         displayList [row]. isFavorite =	
	                             !displayList [row]. isFavorite;
	         theDataBase. update (displayList [row],
	                              displayList [row]. isFavorite);
	         if (displayList [row]. isFavorite)
	            theTable -> item (row, 0) -> setText ("*");
	         else
	            theTable -> item (row, 0) -> setText ("");
	         return;
	      case FAVORITEVIEW:
	      case FILEINPUT:
	         return;	// no semantics
	      default:		// cannot happen
	         return;
	   }
	}
	if (currentService != -1) 
	   unmark (currentService);

	switch (theMode) {
	   case ENSEMBLEVIEW:	
	   case FAVORITEVIEW:
	      emit (selectService (displayList [row]. serviceName,
	                            displayList [row]. channelName));
	      set_channelIndex (displayList [row]. channelName);
	      break;

	   case FILEINPUT:
	      emit selectService (displayList [row]. serviceName, "");
	      break;

	   default:		// cannot happen
	      break;
	}
	currentService = row;
	mark (currentService);
}

void	serviceViewer::handle_nextService	() {
	if (currentService == -1)
	   return;
	unmark (currentService);
	currentService	= (currentService + 1) % displayList. size ();
	mark (currentService);
	emit (selectService (displayList [currentService]. serviceName,
	                     displayList [currentService]. channelName));
}

void	serviceViewer::handle_prevService	() {
	if (currentService == -1)
	   return;
	unmark (currentService);
	currentService	= (currentService - 1 + displayList. size ()) % displayList. size ();
	mark (currentService);
	emit (selectService (displayList [currentService]. serviceName,
	                     displayList [currentService]. channelName));
}
//
//	Only active when in mode ENSEMBLEVIEW
void	serviceViewer::handle_channelSelector	(const QString &channel) {
	emit setChannel (channel);
	if (ensembleMode != ALL) {
	   clearTable ();
	   displayList. clear ();
	   show_displayList ();
	}
}

void	serviceViewer::handle_nextChannel	() {
	int currentChannel	= channelSelector -> currentIndex ();
	currentChannel 		= (currentChannel + 1) %
	                                  channelSelector -> count ();
	set_channelIndex (currentChannel);
	emit setChannel (channelSelector -> currentText ());

	if (ensembleMode != ALL) {
	   clearTable ();
	   displayList. clear ();
//	   displayList       =
//	              theDataBase. getData (theMode, serviceOrder, 
//	                                    channelSelector -> currentText ());
	   show_displayList ();
	}
}

void	serviceViewer::handle_prevChannel	() {
	int currentChannel	= channelSelector -> currentIndex ();
	currentChannel 		= (currentChannel - 1 + channelSelector -> count ()) %
	                                  channelSelector -> count ();
	set_channelIndex (currentChannel);
	emit setChannel (channelSelector -> currentText ());

	if (ensembleMode != ALL) {
	   clearTable ();
	   displayList. clear ();
//	   displayList       =
//	             theDataBase. getData (theMode, serviceOrder, 
//	                                   channelSelector -> currentText ());
	   show_displayList ();
	}
}

void	serviceViewer::set_channelIndex	(const QString &channelName) {
	int k = channelSelector -> findText (channelName);
	if (k != -1) {
	   set_channelIndex (k);
	}
}

void	serviceViewer::set_channelIndex (int channelIndex) {
	bool isEnabled	= false;
	if (channelSelector	-> isEnabled ()) {
	   isEnabled = true;
	   channelSelector	-> setEnabled (false);
	}
	channelSelector		-> setCurrentIndex (channelIndex);
	if (isEnabled)
	   channelSelector	-> setEnabled (true);
}
	
void	serviceViewer::handle_viewSelector	() {
	if (theMode == FILEINPUT)
	   return;
	int order	= theRadio -> get_serviceOrder (); 
	switch (theMode) {
	   case FAVORITEVIEW: {
	      serviceDescriptor oldService;
	      theMode		= ENSEMBLEVIEW;
	      if (currentService != -1) 
	         oldService = displayList [currentService];
	      channelSelector	-> setEnabled (true);
	      prevChannel	-> setEnabled (true);
	      nextChannel	-> setEnabled (true);
	      viewSelector	-> setEnabled (true);
	      channelSelector	-> show ();
	      prevChannel	-> show ();
	      nextChannel	-> show ();
	      viewSelector	-> show ();
	      viewSelector	-> setText ("Favorites");
	      clearTable ();
	      displayList. clear ();
	      if (ensembleMode == ALL)
	         displayList	= theDataBase. getData (theMode, order);
	      else
	         displayList	= theDataBase. getData (theMode, order,
	                                                 currentChannel ());
	      show_displayList ();
	      if (currentService != -1) {
	         currentService = locate (oldService);
	         if (currentService != -1)
	            mark (currentService);
	      }
	   }
	   break;
	   case ENSEMBLEVIEW: {
	      serviceDescriptor oldService;
	      if ((currentService != -1) &&
	         displayList [currentService]. isFavorite)
	         oldService = displayList [currentService];
	      else
	         oldService. isFavorite = false;
	      theMode		= FAVORITEVIEW;
	      channelSelector	-> setEnabled (false);
	      prevChannel	-> setEnabled (false);
	      nextChannel	-> setEnabled (false);
	      viewSelector	-> setEnabled (true);
	      channelSelector	-> hide ();
	      prevChannel	-> hide ();
	      nextChannel	-> hide ();
	      viewSelector	-> show ();
	      clearTable ();
	      displayList. clear ();
	      viewSelector	-> setText ("EnsembleView");
	      displayList	= theDataBase. getData (theMode, order);
	      show_displayList ();
	      currentService	= -1;
	      if (oldService. isFavorite) {
	         currentService = locate (oldService);
	         if (currentService != -1)
	            mark (currentService);
	      }
	   }
	   break;
	   default:;
	      // cannot happen
	}
	emit (reduceButtons (theMode == FAVORITEVIEW));
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
//
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
QString fontColor = value_s (viewSettings, ENSEMBLE,
                                             "fontColor", "white");
	viewLocker. lock ();
	for (int i = 0; i < theTable -> rowCount (); i ++) {
	   if (i == currentService) {
	      theTable -> item (i, 1) -> setFont (markedFont);
	      theTable -> item (i, 1) -> setForeground (QColor(fontColor));
	      theTable -> item (i, 2) -> setFont (markedFont);
	      theTable -> item (i, 2) -> setForeground (QColor(fontColor));
	   }
	   else {
	      theTable -> item (i, 1) -> setFont (normalFont);
	      theTable -> item (i, 1) -> setForeground (QColor(fontColor));
	      theTable -> item (i, 2) -> setFont (normalFont);
	      theTable -> item (i, 2) -> setForeground (QColor(fontColor));
	   }
	}
	viewLocker. unlock ();
}

void	serviceViewer::clearTable	() {
	viewLocker. lock ();
	for (uint16_t i = 0; i < theTable -> rowCount (); i ++)
	   for (uint16_t j = 0; j < theTable -> columnCount (); j ++)
	      theTable -> item (i, j) -> setText (" ");
	
	while (theTable -> rowCount () > 0)
	   theTable -> removeRow (0);
	viewLocker. unlock ();
}

void	serviceViewer::show_displayList	() {
//	assert that the table is empty
QString fontColor = value_s (viewSettings, ENSEMBLE,
	                                     "fontColor", "white");
	if (theTable -> rowCount () > 1) 
	   return;

	viewLocker. lock ();
	for (auto &dt: displayList) {
	   QString serviceName	= dt. serviceName;
	   QString channel	= dt. channelName;
	   bool	isFavorite	= dt. isFavorite;
	   int row = theTable -> rowCount ();
	   theTable	-> insertRow (row);
	   QTableWidgetItem *item_0 = new QTableWidgetItem;
	   item_0	-> setTextAlignment (Qt::AlignLeft |Qt::AlignVCenter);
	   item_0	-> setFlags (Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	   theTable -> setItem (row, 0, item_0);
	   theTable -> item (row, 0) -> setText (isFavorite ? "*" : "");

	   QTableWidgetItem *item_1 = new QTableWidgetItem;
	   item_1	-> setTextAlignment (Qt::AlignLeft |Qt::AlignVCenter);
	   theTable -> setItem (row, 1, item_1);
	   theTable -> item (row, 1) -> setText (serviceName);
	   theTable -> item (row, 1) -> setFont (normalFont);
	   theTable -> item (row, 1) -> setForeground (QColor(fontColor));
	
	   QTableWidgetItem *item_2 = new QTableWidgetItem;
	   item_2	-> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	   theTable -> setItem (row, 2, item_2);
	   theTable -> item (row, 2) -> setText (channel);
	   theTable -> item (row, 2) -> setFont (normalFont);
	   theTable -> item (row, 2) -> setForeground (QColor(fontColor));
	}
	viewLocker. unlock ();
}

//
//	to be handled later on
void	serviceViewer::handleRightMouseClick	(const QString &text) {
	(void)text;
}

QStringList	serviceViewer::getSelectables	() {
QStringList res;

	for (auto &sd : displayList) {
	   if (sd. isFavorite) {
	      QString ss = sd. channelName + ":" + sd. serviceName;
	      res << ss;
	   }
	}
	return res;
}

void	serviceViewer::set_Colors () {
QString prevService_color =
	   value_s (viewSettings, COLOR_SETTINGS, PREV_SERVICE + "_color",
	                                             GREEN);
QString prevService_font =
	   value_s (viewSettings, COLOR_SETTINGS, PREV_SERVICE + "_font",
	                                              BLACK);

QString nextService_color =
	   value_s (viewSettings, COLOR_SETTINGS, NEXT_SERVICE + "_color",
	                                              GREEN);
QString nextService_font =
	   value_s (viewSettings, COLOR_SETTINGS, NEXT_SERVICE + "_font",
	                                              BLACK);

QString viewSelector_color =
	   value_s (viewSettings, COLOR_SETTINGS, VIEW_SELECTOR + "_color",
	                                              GREEN);
QString viewSelector_font =
	   value_s (viewSettings, COLOR_SETTINGS, VIEW_SELECTOR + "_font",
	                                              BLACK);

QString channelDisplay_color =
	   value_s (viewSettings, COLOR_SETTINGS, CHANNEL_DISPLAY + "_color",
	                                              BLUE);
QString channelDisplay_font =
	   value_s (viewSettings, COLOR_SETTINGS, CHANNEL_DISPLAY + "_font",
	                                              BLACK);

	QString temp = "QPushButton {background-color: %1; color: %2}";
	prevService ->
	              setStyleSheet (temp. arg (prevService_color,
	                                        prevService_font));
	nextService	->
	              setStyleSheet (temp. arg (nextService_color,
	                                        nextService_font));
	viewSelector	->
	              setStyleSheet (temp. arg (viewSelector_color,
	                                        viewSelector_font));
	channelDisplayButton	->
	              setStyleSheet (temp. arg (channelDisplay_color,
	                                        channelDisplay_font));
}

void	serviceViewer::color_prevService	() 	{
	setButtonColors (prevService, PREV_SERVICE);
}

void	serviceViewer::color_nextService	() 	{
	setButtonColors (nextService, NEXT_SERVICE);
}

void	serviceViewer::color_viewSelector	() 	{
	setButtonColors (viewSelector, VIEW_SELECTOR);
}

void	serviceViewer::color_channelDisplay	() 	{
	setButtonColors (channelDisplayButton, CHANNEL_DISPLAY);
}

void	serviceViewer::setButtonColors		(QPushButton *b,
	                                         const QString &buttonName) {
QColor	baseColor, textColor;

	QColor color = QColorDialog::getColor (baseColor, nullptr, "baseColor");
	if (!color. isValid ())
	   return;
	baseColor	= color;
	color = QColorDialog::getColor (textColor, nullptr, "textColor");
	if (!color. isValid ())
	   return;
	textColor	= color;
	QString temp = "QPushButton {background-color: %1; color: %2}";
	b	-> setStyleSheet (temp. arg (baseColor. name (),
	                                     textColor. name ()));

	QString buttonColor	= buttonName + "_color";
	QString buttonFont	= buttonName + "_font";
	QString baseColor_name	= baseColor. name ();
	QString textColor_name	= textColor. name ();
	store (viewSettings, COLOR_SETTINGS, buttonColor, baseColor_name);
	store (viewSettings, COLOR_SETTINGS, buttonFont, textColor_name);
}

int	serviceViewer::locate	(serviceDescriptor &sd) {
	for (uint16_t i = 0; i < displayList. size (); i ++)
	   if ((displayList [i]. serviceName == sd. serviceName) &&
	       (displayList [i]. channelName == sd. channelName))
	   return i;
	return -1;
}
static
QString hextoString (uint32_t v) {
QString res;
        for (int i = 0; i < 4; i ++) {
           const uint8_t t = (v & 0xF000) >> 12;
           QChar c = t <= 9 ? static_cast<char> ('0' + t) :
                              static_cast<char> ('A' + t - 10);
           res. append (c);
           v <<= 4;
        }        
        return res;
}

void	serviceViewer::set_countryName	(const QString &name) {
	countryName	-> setText (name);
	if (theMode == ENSEMBLEVIEW)
	   theDataBase. set_countryName (name, currentChannel ());
}

void	serviceViewer::set_ensembleId	(const QString &name, int id) {
	ensembleId      -> setText (name + QString ("(") +
                  hextoString (static_cast<uint32_t>(id))+ QString (")"));
	if (theMode == ENSEMBLEVIEW)
	   theDataBase. set_ensembleName (name, currentChannel ());
	connect (&timer_1, &QTimer::timeout,
	         this, &serviceViewer::startButtons);
	timer_1. start (1000);
}

void	serviceViewer::clear_ensembleId	() {
	ensembleId	-> setText ("");
	countryName	-> setText ("");
	timer_1. stop ();
	disconnect (&timer_1, &QTimer::timeout,
	            this, &serviceViewer:: startButtons);
	disconnect (ensembleId, &clickablelabel::clicked_left,
	            theRadio, &RadioInterface::handle_contentButton);
	disconnect (ensembleId, &clickablelabel::clicked_right,
	            theRadio, &RadioInterface::handle_dump);
	if ((theMode == ENSEMBLEVIEW) &&
	    (ensembleMode == SINGLE_CHANNEL))
	   clearAll ();
}

void	serviceViewer::startButtons () {
	timer_1. stop ();
	disconnect (&timer_1, &QTimer::timeout,
	            this, &serviceViewer:: startButtons);
	connect (ensembleId, &clickablelabel::clicked_left,
	         theRadio, &RadioInterface::handle_contentButton);
	if (theMode == ENSEMBLEVIEW)
	   connect (ensembleId, &clickablelabel::clicked_right,
	            theRadio, &RadioInterface::handle_dump);
}

int	serviceViewer::add_to_displayList (const serviceDescriptor &sd) {
//	if list is empty, just add,

	if (displayList. size () == 0) {
	   displayList. append (sd);
           return displayList. size () - 1;
	}
//	insert at front
	if (displayList [0]. channel > sd. channel) {
	   displayList. insert (0, sd);
	   return 0;
	}

	for (int i = 0; i < displayList. size () - 1; i ++) {
	   if (displayList [i]. channel > sd. channel) {
	      displayList . insert (i, sd);
	      return i;
	   }
	}
	displayList. append (sd);
	return displayList. size () - 1;
}

void	serviceViewer::handle_channelDisplay	() {
	if (channelDisplay. isVisible ())
	   channelDisplay. hide ();
	else {
	   channelDisplay. clear ();
	   channelDisplay. reload ();
	   channelDisplay. show ();
	}
}
//
//	called from the channeldisplay
void	serviceViewer::prepareChannel	(const QString &channel) {
	if (theMode != ENSEMBLEVIEW)
	   return;

	disconnect (channelSelector,
                     qOverload<const QString &> (&QComboBox::textActivated),
                 this, &serviceViewer::handle_channelSelector);
	int k = channelSelector -> findText (channel);
        if (k != -1)  
           channelSelector -> setCurrentIndex (k);
	connect (channelSelector,
                     qOverload<const QString &> (&QComboBox::textActivated),
                 this, &serviceViewer::handle_channelSelector);
	handle_channelSelector (channel);
}


