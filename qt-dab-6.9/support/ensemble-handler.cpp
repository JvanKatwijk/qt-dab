#
/*
 *    Copyright (C)  2015, 2023
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
#include	"ensemble-handler.h"
#include	"font-chooser.h"
#include	<QStringList>
#include	<QSettings>
#include	<QtXml>
#include	<QColorDialog>
#include	<QFontDialog>

#include	"settingNames.h"
#include	"settings-handler.h"

#define	MARKED	0100
#define	NORMAL	0000
	ensembleHandler::ensembleHandler (RadioInterface *parent,
	                                  QSettings	*ensembleSettings,
	                                  const QString &favFile):
	                                       QTableWidget (0, 2),
	                                       normalFont ("Times", 10, -1, false),
	                                       markedFont ("Times", 12, -1, true),
	                                       channelFont ("Times", 8) {
	   (void)parent;
	   this -> ensembleSettings	= ensembleSettings;
	   this	-> favFile		= favFile;
	   ensembleMode		= SHOW_ENSEMBLE;
	   QString theFont	=
	           value_s (ensembleSettings, ENSEMBLE, "theFont", "Times");
	   int fontSize		=	
	           value_i (ensembleSettings, ENSEMBLE, "fontSize", 10);
	   fontColor		= 
	           value_s (ensembleSettings, ENSEMBLE, "fontColor", "white");
	   normalFont	= QFont (theFont, fontSize, -1, false);
	   markedFont	= QFont (theFont, fontSize + 2, -1, true);
	   channelFont	= QFont (theFont, fontSize - 2, -1, false);

	   this	-> setSelectionBehavior (QAbstractItemView::SelectRows);
	   this -> setHorizontalHeaderLabels (
	                           QStringList () << tr ("service") <<
	                                             tr ("fav"));
	   connect (this, &QTableWidget::cellClicked,
	            this, &ensembleHandler::click_on_service);
	   loadFavorites (favFile);
	   handlePresets	= true;	// for now
	   serviceOrder	= value_i (ensembleSettings , ENSEMBLE,
	                                  "serviceOrder", ALPHA_BASED);
}

	ensembleHandler::~ensembleHandler () {
	   storeFavorites	(favFile);
}

void	ensembleHandler::loadFavorites	(const QString &favFile) {
QDomDocument xmlBOM;
//
//	start with an empty lis, waiting ....
	favorites. resize (0);
	QFile f (favFile);
	if (!f. open (QIODevice::ReadOnly)) 
	   return;
	xmlBOM. setContent (&f);

	f. close ();
	QDomElement root	= xmlBOM. documentElement ();
	QDomElement component	= root. firstChild (). toElement ();
	while (!component. isNull ()) {
	   if (component. tagName () == "PRESET_ELEMENT") {
	      QString  name 		= component. attribute ("SERVICE_NAME", "???");
	      QString  channel	= component. attribute ("CHANNEL", "5A");

	      add_to_favorites (name, channel);
	   }
	   component = component. nextSibling (). toElement ();
	}
}

void	ensembleHandler::storeFavorites	(const QString &favFile) {
QDomDocument the_favorites;
QDomElement root = the_favorites. createElement ("preset_db");

	the_favorites. appendChild (root);

	for (uint16_t i = 0; i < (int)(favorites. size ()); i ++) {
	   QString channel	= favorites. at (i). channel;
	   QString serviceName	= favorites. at (i). name;
	   QDomElement favorite = the_favorites.
	                            createElement ("PRESET_ELEMENT");
	   favorite. setAttribute ("SERVICE_NAME", serviceName);
	   favorite. setAttribute ("CHANNEL", channel);
	   root. appendChild (favorite);
	}

	QFile file (favFile);
	if (!file. open (QIODevice::WriteOnly | QIODevice::Text))
	   return;

	QTextStream stream (&file);
	stream << the_favorites. toString ();
	file. close ();
}

void	ensembleHandler::reportStart	(const QString &s) {
int index = inEnsembleList (s);
	if (index < 0)
	   return;
	if (ensembleList [index]. selected)
	   return;
	unSelect ();
	ensembleList [index]. selected = true;
	if (handlePresets) {
	   int ind2 = inFavorites (s);
	   if (ind2 >= 0)
	      favorites [ind2]. selected = true;
	}
	updateList ();
}

void	ensembleHandler::unSelect	() {
	for (auto &s : ensembleList)
	   if (s. selected) {
	      s. selected = false;
	      break;
	   }

	for (auto &s: favorites)
	   if (s. selected) {
	      s. selected = false;
	      break;
	   }
}

void	ensembleHandler::click_on_service	(int row, int column) {
	QTableWidgetItem *theItem_0 = this -> item (row, 0);
	QString theService	= theItem_0 -> text ();
	QTableWidgetItem *theItem_1 = this -> item (row, 1);
//
	if (ensembleMode == SHOW_ENSEMBLE) {
	   QString theMark		= theItem_1 -> text ();
	   int index = inEnsembleList (theService);
	   if (index < 0)	// should not happen
	      return;
//
//	If the click is on the second column, it is easy,
//	and we do not need to update the while list
	   if (column == 1) {
	      if (!handlePresets)	// should not occur
	         return;
	      if (theMark == " ") {	// add to favorites
	         add_to_favorites (theService, ensembleList [index]. channel);
	         if (ensembleList [index]. selected) {
	            int ind2 = inFavorites (theService);
	            if (ind2 >= 0)	// it should be
	               favorites [ind2]. selected = true;
	         }
	         theItem_1 -> setText ("*");
	      }
	      else {
	         remove_from_favorites (theService);
	         theItem_1 -> setText (" ");
	      }
	      return;
	   }
//
//	in selecting a service we first have to deselect the "old"
//	service - if any - both in ensemble list and favorites
	   else {	// column is column 0, start
	      unSelect ();
	      int index = inEnsembleList (theService);
	      selectService (ensembleList [index]. name,
	                     ensembleList [index]. channel);
	      if (index >= 0) {		// should not happen
	         ensembleList [index]. selected = true;
	      }
	   }
	   return;
	}	// apparently ensembleMode == SHOW_PRESETS

	if (!handlePresets)	// should not occur
	   return;
	int index;
	index = inFavorites (theService);
	if (index < 0)
	   return;

	if (column == 0)
	   selectService (favorites [index]. name,
                          favorites [index]. channel);
	else {
	   service theService = favorites [index];
	   favorites. erase (favorites. begin () + index);
	   favorites. insert (favorites. begin (), theService);
	}
	updateList ();
}

void	ensembleHandler::selectNextService	() {
	if (ensembleMode != SHOW_ENSEMBLE)
	   return;

	int selectedService = -1;
	for (uint16_t i = 0; i < ensembleList. size (); i ++) {
	   if (ensembleList [i]. selected) {
	      selectedService = i;
	      break;
	   }
	}
	if (selectedService < 0)
	   return;
	unSelect ();
	ensembleList [selectedService]. selected = false;
	selectedService = (selectedService + 1) % ensembleList. size ();
	selectService (ensembleList [selectedService]. name,
	               ensembleList [selectedService]. channel);
	ensembleList [selectedService]. selected = true;
	int index = inFavorites (ensembleList [selectedService]. name);
	if (index >= 0)
	   favorites [index]. selected = true;
	updateList ();
}

void	ensembleHandler::selectPrevService	() {
	if (ensembleMode != SHOW_ENSEMBLE)
	   return;
	int selectedService = -1;
	for (uint16_t i = 0; i < ensembleList. size (); i ++) {
	   if (ensembleList [i]. selected) {
	      selectedService = i;
	      break;
	   }
	}
	if (selectedService < 0)
	   return;
	unSelect ();
	ensembleList [selectedService]. selected = false;
	selectedService = (selectedService - 1 + ensembleList. size ()) %	
	                                         ensembleList. size ();
	selectService (ensembleList [selectedService]. name,
	               ensembleList [selectedService]. channel);
	ensembleList [selectedService]. selected = true;
	int index = inFavorites (ensembleList [selectedService]. name);
	if (index >= 0)
	   favorites [index]. selected = true;
	updateList ();
}

void	ensembleHandler::handle_rightMouseClick	(const QString &text) {
	if ((text == " ") || (text == "*"))
	   return;

	if (ensembleMode == SHOW_ENSEMBLE) {
	   int index = inEnsembleList (text);
	   if (index < 0)
	      return;
	   start_background_task (text);
	   this -> clearSelection ();
	}
	else { 	// SHOW_PRESETS
//	you cannot remove the currently playing service
	   int index = inFavorites (text);
	   if (index < 0)	// should not happen
	      return;
	   if (favorites [index]. selected)
	      return;
	   favorites. erase (favorites. begin () + index);
	   updateList ();
	}
}

void	ensembleHandler::clearTable	() {
	for (uint16_t i = 0; i < this -> rowCount (); i ++)
	   for (uint16_t j = 0; j < this -> columnCount (); j ++)
	      this -> item (i, j) -> setText (" ");
	for (uint16_t i = 6; i < this -> rowCount (); i ++)
	   this -> removeRow (i);
}

void	ensembleHandler::reset	() {
	clearTable ();
	ensembleList. resize (0);
}

static inline
bool	isAudio (uint32_t v) {
	return (v & 0xFF0000) == 0;
}

void	ensembleHandler::updateList	() {
int currentRow	= 0;
//bool	audioOnly	= value_i (ensembleSettings, CONFIG_HANDLER,
//	                                        "audioServices_only", 1);
	clearTable ();
	if (ensembleMode == SHOW_ENSEMBLE) {
	   this -> setHorizontalHeaderLabels (
	                           QStringList () << tr ("service") <<
	                                             tr ("fav"));
	   for (uint16_t i = 0; i < ensembleList. size (); i ++) {
	      bool toMark = false;
	      if (handlePresets)
	         toMark = inFavorites (ensembleList [i]. name) >= 0;
	      addRow (ensembleList [i]. name,
	                 ensembleList [i]. channel, toMark, currentRow);

	      if (ensembleList [i]. selected) {
	         setFont (MARKED, currentRow);
	      }
	      else
	         setFont (NORMAL, currentRow);
	      currentRow ++;
	   }
	}
	else {
	   this -> setHorizontalHeaderLabels (
	                           QStringList () << tr ("service") <<
	                                             tr ("chan"));
	   for (uint16_t i = 0; i < favorites. size (); i ++) {
	      addRow (favorites [i]. name,
	                  favorites [i]. channel, true, currentRow);
	      if (favorites [i]. selected) {
	         setFont (MARKED, currentRow);
	      }
	      else
	         setFont (NORMAL, currentRow);
	      currentRow ++;
	   }
	}
	this -> resizeColumnsToContents ();
}

int	ensembleHandler::inEnsembleList (const QString &s) {
	for (uint16_t index = 0; index < ensembleList. size (); index ++)
	   if (ensembleList [index]. name == s)
	      return index;
	return -1;
}

int	ensembleHandler::inFavorites (const QString &s) {
	for (uint16_t index = 0; index < favorites. size (); index ++)
	   if (favorites [index]. name == s)
	      return index;
	return -1;
}

void	ensembleHandler::remove_from_favorites	(const QString &s) {
	for (uint16_t i = 0; i < favorites. size (); i ++) {
	   if (favorites [i]. name == s) {
	      favorites. erase (favorites. begin () + i);
	      return;
	   }
	}
}
//
void	ensembleHandler::add_to_favorites (const QString &s, const QString &c) {
	for (auto &t: favorites)
	   if (t. name == s)
	      return;
	service theService;
	theService. name	= s;
	theService. channel	= c;
	theService. selected	= false;
	bool ok = false;
	int value = c. trimmed (). toInt (&ok, 16);
	if (!ok) {
	   favorites. push_back (theService);
	   return;
	}
	if (favorites. size () == 0) {
	   favorites. push_back (theService);
	   return;
	}
	for (uint16_t i = 0; i < favorites. size (); i ++) {
	   if (value < favorites [i]. channel. toInt (&ok, 16)) {
	      favorites. insert (favorites. begin () + i, theService);
	      return;
	   }
	}
	favorites. push_back (theService);
}
//
//	This one is called many times, e.g. after (a while) selecting a
//	service from the favorites, so here we have to check on being
//	selected

bool	ensembleHandler::alreadyIn	(serviceId &ed) {
	for (auto &s:ensembleList)
	   if (s. name == ed. name)
	      return true;
	return false;
}

bool	ensembleHandler::add_to_ensemble (serviceId &ed) {
	if (alreadyIn (ed))
	   return false;

	if (handlePresets) {
	   int index = inFavorites (ed. name);
	   ed. selected = (index >= 0) && (favorites [index]. selected);
	}
	else
	   ed. selected = false;

	ensembleList = insert (ensembleList, ed, serviceOrder);
	
	clearTable ();
	updateList ();
	return true;
}

void	ensembleHandler::add_favorite_from_scanList (const QString &service) {
	if (!handlePresets)
	   return;
#if QT_VERSION >= QT_VERSION_CHECK (5, 15, 2)
        QStringList list = service.split (":", Qt::SkipEmptyParts);
#else
        QStringList list = service.split (":", QString::SkipEmptyParts);
#endif
	if (list. size () < 2)
	   return;
	for (auto &s : favorites)
	   if (s. name == list. at (1))
	      return;		// nothing to be done
	add_to_favorites (list. at (1), list. at (0));
	int index = inEnsembleList (service);
	if ((index >= 0) && (ensembleList [index]. selected))
	   ensembleList [index]. selected = true;
	updateList ();
}

void	ensembleHandler::addRow	(const QString &service,
	                         const QString &channel, bool Mark, int row) {
	if (row < this -> rowCount ()) {
	   this -> item (row, 0) -> setText (service);
	   this -> item (row, 0) -> setFont (normalFont);
	   this -> item (row, 0) -> setForeground (QColor(fontColor));
	   if (ensembleMode == SHOW_ENSEMBLE) {
	      this -> item (row, 1) ->
	                   setTextAlignment (Qt::AlignCenter |Qt::AlignVCenter);
	      this -> item (row, 1) -> setText (Mark ? "*": " ");
	   }
	   else {
	      this -> item (row, 1) ->
	                   setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	      this -> item (row, 1) -> setText (channel);
	      this -> item (row, 1) -> setFont (channelFont);
	   }
	}
	else {
	   this -> insertRow (row);
	   QTableWidgetItem *item_0 = new QTableWidgetItem;
	   item_0	-> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	   this -> setItem (row, 0, item_0);
	   QTableWidgetItem *item_1 = new QTableWidgetItem;
	   this -> item (row, 0) -> setText (service);
	   this -> item (row, 0) -> setFont (normalFont);
	   this -> item (row, 0) -> setForeground (QColor(fontColor));
	   if (ensembleMode == SHOW_PRESETS) {
	      item_1	-> setTextAlignment (Qt::AlignRight |Qt::AlignVCenter);
	      this -> setItem (row, 1, item_1);
	      this -> item (row, 1) -> setText (channel);
	      this -> item (row, 1) -> setFont (channelFont);
	   }
	   else {
	      item_1	-> setTextAlignment (Qt::AlignHCenter |Qt::AlignVCenter);
	      this -> setItem (row, 1, item_1);
	      this -> item (row, 1) -> setText (Mark ? "*": " ");
	   }
	}
}

std::vector<serviceId>
	ensembleHandler::insert (std::vector<serviceId> &l,
	                        const serviceId &n, int order) {
std::vector<serviceId> k;

	if (l . size () == 0) {
	   k. push_back (n);
	   return k;
	}
	uint32_t baseN		= 0;
	uint16_t baseSubCh	= 0;
	QString baseS		= "";

	bool	inserted	= false;
	for (auto serv : l) {
	   if (!inserted) {
	      if (order == ID_BASED) {
	         if ((baseN <= n. SId) && (n. SId <= serv. SId)) {
	            k. push_back (n);
	            inserted = true;
	         }
	      }
	      else
	      if (order == SUBCH_BASED) {
	         if ((baseSubCh <= n. subChId) && (n. subChId <= serv. subChId)) {
	            k. push_back (n);
	            inserted = true;
	         }
	      }
	      else {
	         if ((baseS < n. name) && (n. name < serv. name)) {
	            k. push_back (n);
	            inserted = true;
	         }
	      }
	   }
	   baseS	= serv. name;
	   baseN	= serv. SId;
	   baseSubCh	= serv. subChId;
	   k. push_back (serv);
	}

	if (!inserted)
	   k. push_back (n);
	return k;
}


QStringList ensembleHandler::getSelectables	() {
QStringList res;

	for (auto &s : ensembleList) {
	   QString item = s. channel + ":" + s. name;
	   res << item;
	}
	for (auto &s : favorites) {
	   int t = inEnsembleList (s. name);
	   if (t < 0) {
	      QString item = s. channel + ":" + s. name;
	      res << item;
	   }
	}
	return res;
}

QStringList ensembleHandler::get_epgServices () {
QStringList res;
	for (auto &serv : ensembleList) {
	   if (serv. name. contains ("-EPG ", Qt::CaseInsensitive) ||
               serv. name. contains (" EPG   ", Qt::CaseInsensitive) ||
               serv. name. contains ("Spored", Qt::CaseInsensitive) ||
               serv. name. contains ("NivaaEPG", Qt::CaseInsensitive) ||
               serv. name. contains ("SPI", Qt::CaseSensitive) ||
               serv. name. contains ("BBC Guide", Qt::CaseInsensitive) ||
               serv. name. contains ("BBC  Guide", Qt::CaseInsensitive) ||
               serv. name. contains ("EPG_", Qt::CaseInsensitive) ||
               serv. name. startsWith ("EPG ", Qt::CaseInsensitive) ) {
	      res << serv. name;
	   }
	}
	return res;
}

int	ensembleHandler::get_serviceCount	() {
	return ensembleList. size ();
}

void	ensembleHandler::setFont (int mark, int index) {
	if (mark == MARKED) {
	   this -> item (index, 0) -> setFont (markedFont);
	   this -> setCurrentItem (this -> item (index, 0), QItemSelectionModel::Select);
	}
	else
	   this -> item (index, 0) -> setFont (normalFont);
}

void	ensembleHandler::handle_fontSelect () {
int fontSize;
	fontSize	= value_i (ensembleSettings, ENSEMBLE,
	                                         "fontSize", 10);
#ifdef	__NOT_NOW__
bool ok;
	QFont theFont = QFontDialog::getFont (
	                      &ok, QFont ("Helvetica", fontSize), this);
	if (!ok)
	   return;
	
	ensembleSettings	-> setValue ("theFont", theFont. toString ());
	normalFont	= QFont (theFont. toString (), fontSize, -1, false);
	markedFont	= QFont (theFont. toString (), fontSize + 2, -1, true);
	channelFont	= QFont (theFont. toString (), fontSize - 2);
	updateList ();
#else
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
	store (ensembleSettings, ENSEMBLE, "theFont", theFont);
	normalFont	= QFont (theFont, fontSize, -1, false);
	markedFont	= QFont (theFont, fontSize + 2, -1, true);
	channelFont	= QFont (theFont, fontSize - 2);
	updateList ();
#endif
}

void	ensembleHandler::handle_fontColorSelect () {
QColor	color;

	color	= QColorDialog::getColor (color, nullptr, "fontColor");
	if (!color. isValid ())
	   return;
	fontColor	= color. name ();
	store (ensembleSettings, ENSEMBLE, "fontColor", fontColor);
	updateList ();
}

void	ensembleHandler::handle_fontSizeSelect	(int fontSize) {
QString	theFont	= ensembleSettings -> value ("theFont", "Times"). toString ();

	if (fontSize < 8) 
	   return;
	store (ensembleSettings, ENSEMBLE, "fontSize", fontSize);
	normalFont	= QFont (theFont, fontSize, -1, false);
	markedFont	= QFont (theFont, fontSize + 2, -1, true);
	channelFont	= QFont (theFont, fontSize - 2);
	updateList ();
}

int	ensembleHandler::nrFavorites	() {
	return favorites. size ();
}

bool	ensembleHandler::hasFavorite	(const QString &name) {
	for (auto &s : favorites) {
//	   fprintf (stderr, "Comparing %s with %s\n",
//	                      s. name. toLatin1 (). data (),
//	                      name. toLatin1(). data ());
	   if (s. name == name)
	      return true;
	}
	return false;
}

void	ensembleHandler::setMode	(bool b) {
	handlePresets	= b;
	if (!handlePresets) {
	   ensembleMode = SHOW_ENSEMBLE;
	   updateList ();	
	}
}

int	ensembleHandler::get_showMode	() {
	return ensembleMode;
}

void	ensembleHandler::set_showMode	(int m) {
	if (!handlePresets)
	   return;
	if (ensembleMode == m)
	   return;
	ensembleMode	= m == SHOW_ENSEMBLE ? SHOW_ENSEMBLE : SHOW_PRESETS;
	updateList ();
}

uint16_t	ensembleHandler::extract_SId	(const QString &name) {
	for (auto serv: ensembleList) {
	   if (name. contains (QString::number (serv. SId, 16),
	                                   Qt::CaseInsensitive))
	      return serv. SId;
	}
	return 0;
}

void	ensembleHandler::handle_scheduledSelect (const QString &name,
	                                         const QString &channel) {
	unSelect ();
	int index = inEnsembleList (name);
	if (index >= 0) {
	   unSelect ();
	   int ind1 = inFavorites (name);
	   if (ind1 > 0)
	      favorites [ind1]. selected = true;
	   ensembleList [index]. selected = true;
	   selectService (name, channel);
	   if (ensembleMode == SHOW_PRESETS)
	      set_showMode (SHOW_ENSEMBLE);
	   updateList ();
	}
	else {		// may be in favorites?
	   int index	= inFavorites (name);
	   if (index < 0) {	// if not, just add 
	      add_to_favorites (name, channel);
	      index = inFavorites (name);
	      if (index < 0)	// should not happen
	         return;
	   }
	   favorites [index]. selected = true;
	   selectService (name, channel);
	   if (ensembleMode == SHOW_ENSEMBLE)
	      set_showMode (SHOW_PRESETS);
	}
}

void	ensembleHandler::set_serviceOrder	(int order) {
	store (ensembleSettings, ENSEMBLE,"serviceOrder", order);
	serviceOrder	= order;
}

