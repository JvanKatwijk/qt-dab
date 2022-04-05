#
/*
 *    Copyright (C) 2020
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
//
//
//	The band handler now manages the skipTable
//
#include	"band-handler.h"
#include	"dab-constants.h"
#include	<QHeaderView>
#include	<QDomDocument>
#include	<stdio.h>

static
dabFrequencies frequencies_1 [] = {
{"5A",	174928, false},
{"5B",	176640, false},
{"5C",	178352, false},
{"5D",	180064, false},
{"6A",	181936, false},
{"6B",	183648, false},
{"6C",	185360, false},
{"6D",	187072, false},
{"7A",	188928, false},
{"7B",	190640, false},
{"7C",	192352, false},
{"7D",	194064, false},
{"8A",	195936, false},
{"8B",	197648, false},
{"8C",	199360, false},
{"8D",	201072, false},
{"9A",	202928, false},
{"9B",	204640, false},
{"9C",	206352, false},
{"9D",	208064, false},
{"10A",	209936, false},
{"10B", 211648, false},
{"10C", 213360, false},
{"10D", 215072, false},
{"11A", 216928, false},
{"11B",	218640, false},
{"11C",	220352, false},
{"11D",	222064, false},
{"12A",	223936, false},
{"12B",	225648, false},
{"12C",	227360, false},
{"12D",	229072, false},
{"13A",	230784, false},
{"13B",	232496, false},
{"13C",	234208, false},
{"13D",	235776, false},
{"13E",	237488, false},
{"13F",	239200, false},
{nullptr, 0, false}
};

static
dabFrequencies frequencies_2 [] = {
{"LA", 1452960, false},
{"LB", 1454672, false},
{"LC", 1456384, false},
{"LD", 1458096, false},
{"LE", 1459808, false},
{"LF", 1461520, false},
{"LG", 1463232, false},
{"LH", 1464944, false},
{"LI", 1466656, false},
{"LJ", 1468368, false},
{"LK", 1470080, false},
{"LL", 1471792, false},
{"LM", 1473504, false},
{"LN", 1475216, false},
{"LO", 1476928, false},
{"LP", 1478640, false},
{nullptr, 0, false}
};


dabFrequencies alternatives [100];

	bandHandler::bandHandler (const QString &a_band,
	                           QSettings *s):
	                                theTable (nullptr) {
FILE	*f;
	selectedBand		= nullptr;
	dabSettings		= s;
	fileName 		= "";

	theTable. setColumnCount (2);
	QStringList header;
	header	<< tr ("channel") << tr ("scan");
	theTable. setHorizontalHeaderLabels (header);
	theTable. verticalHeader () -> hide ();
	theTable. setShowGrid	(true);
#ifndef	__MINGW32__
	if (a_band == QString (""))
	   return;
	if (a_band != QString ("")) {
	   f = fopen (a_band. toUtf8 (). data (), "r");
	   if (f == nullptr)
	      return;
	}

//	OK we have a file with - hopefully - some input
	size_t	amount		= 128;
	int filler		= 0;
	char *line 		= new char [512];
	while ((filler < 100) && (amount > 0)) {
	   amount = getline (&line, &amount, f);
//	   fprintf (stderr, "%s (%d)\n", line, (int)amount);
	   if ((int)amount <= 0) {	// eof detected
	      break;
	   }
	   if (((int)amount < 8) || ((int) amount > 128)) // ?????
	      continue;
	   line [amount] = 0;
	   char channelName [128];
	   int freq;
	   int res = sscanf (line, "%s %d", channelName, &freq);
	   if (res != 2)
	      continue;
	   fprintf (stderr, "adding %s %d\n", channelName, freq);
	   alternatives [filler]. key	= QString (channelName);
	   alternatives [filler]. fKHz	= freq;
	   alternatives [filler]. skip	= false;
	   filler ++;
	}

	free (line);
	alternatives [filler]. key 	= "";
	alternatives [filler]. fKHz	= 0;
	fclose (f);
	selectedBand	= alternatives;
#endif
}
//
//	note that saving settings has to be done separately
	bandHandler::~bandHandler () {
	if (!theTable. isHidden ())
	   theTable. hide ();
}
//
//	The main program calls this once, the combobox will be filled
void	bandHandler::setupChannels (QComboBox *s, uint8_t band) {
int16_t	i;
int16_t	c	= s -> count();

	if (selectedBand == nullptr) {	// no preset band
	   if (band == BAND_III)
	      selectedBand = frequencies_1;
	   else
	      selectedBand = frequencies_2;
	}

//	clear the fields in the comboBox
	for (i = 0; i < c; i ++) 
	   s	-> removeItem (c - (i + 1));
//
//	The table elements are by default all "+";
	for (int i = 0; selectedBand [i]. fKHz != 0; i ++)  {
	   s -> insertItem (i, selectedBand [i]. key, QVariant (i));
	   theTable. insertRow (i);
	   theTable. setItem (i, 0,
	                   new QTableWidgetItem (selectedBand [i]. key));
	   theTable. setItem (i, 1,
	                   new QTableWidgetItem (QString ("+")));
	}
}
//
//	Note that we only save the channels that are to be skipped
void	bandHandler::saveSettings () {
	if (!theTable. isHidden ())
	   theTable. hide ();

	if (fileName == "") {
	   dabSettings	-> beginGroup ("skipTable");
	   for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
	      if (selectedBand [i]. skip)
	         dabSettings	-> setValue (selectedBand [i]. key, 1);
	      else
	         dabSettings	-> remove (selectedBand [i]. key);
	   }
	   dabSettings	-> endGroup ();
	}
	else {
	   QDomDocument skipList;
	   QDomElement root;

	   root	= skipList. createElement ("skipList");
	   skipList. appendChild (root);

	   for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
	      if (!selectedBand [i]. skip)
	         continue;
	      QDomElement skipElement = skipList.
	                                createElement ("BAND_ELEMENT");
	      skipElement. setAttribute ("CHANNEL", selectedBand [i]. key);
	      skipElement. setAttribute ("VALUE", "-");
	      root. appendChild (skipElement);
	   }

	   QFile file (fileName);
           if (!file. open (QIODevice::WriteOnly | QIODevice::Text))
              return;

           QTextStream stream (&file);
           stream << skipList. toString ();
           file. close ();
	}
}

//
//	when setup_skipList is called, we start with blacklisting all entries
//
void	bandHandler::setup_skipList (const QString &fileName) {
	disconnect (&theTable, SIGNAL (cellDoubleClicked (int, int)),
	            this, SLOT (cellSelected (int, int)));
	for (int i = 0; selectedBand [i]. fKHz > 0; i ++) {
	   selectedBand [i]. skip = false;
	   theTable. item (i, 1) -> setText ("+");
	}

	this	-> fileName	= fileName;
	if (fileName == "")
	   default_skipList ();
	else
	   file_skipList (fileName);

	connect (&theTable, SIGNAL (cellDoubleClicked (int, int)),
	         this, SLOT (cellSelected (int, int)));
}
//
//	default setting in the ini file!!
void	bandHandler::default_skipList () {
	dabSettings	->  beginGroup ("skipTable");
	for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
	   bool skipValue =
	         dabSettings -> value (selectedBand [i]. key, 0). toInt () == 1;
	   if (skipValue) {
	      selectedBand [i]. skip = true;
	      theTable. item (i, 1) -> setText ("-");
	   }
	   
	}
	dabSettings	-> endGroup ();
}

void	bandHandler::file_skipList	(const QString &fileName) {
QDomDocument xml_bestand;

	QFile f (fileName);
	if (f. open (QIODevice::ReadOnly)) {
	   xml_bestand. setContent (&f);
	   QDomElement root	= xml_bestand. documentElement ();
	   QDomElement component	= root. firstChild (). toElement ();
	   while (!component. isNull ()) {
	      if (component. tagName () == "BAND_ELEMENT") {
	         QString channel = component. attribute ("CHANNEL", "???");
	         QString skipItem = component. attribute ("VALUE", "+");
	         if ((channel != "???") && (skipItem == "-")) 
	            updateEntry (channel);
	      }
	      component = component. nextSibling (). toElement ();
	   }
	}
}

void	bandHandler::updateEntry (const QString &channel) {
	for (int i = 0; selectedBand [i]. key != nullptr; i ++)  {
	   if (selectedBand [i]. key == channel) {
	      selectedBand [i]. skip = true;
	      theTable. item (i, 1) -> setText ("-");
	      return;
	   }
	}
}

//	find the frequency for a given channel in a given band
int32_t	bandHandler::Frequency (QString Channel) {
int32_t	tunedFrequency		= 0;
int	i;

	for (i = 0; selectedBand [i]. key != nullptr; i ++) {
	   if (selectedBand [i]. key == Channel) {
	      tunedFrequency	= KHz (selectedBand [i]. fKHz);
	      break;
	   }
	}

	if (tunedFrequency == 0)	// should not happen
	   tunedFrequency = KHz (selectedBand [0]. fKHz);

	return tunedFrequency;
}

int	bandHandler::firstChannel	() {
int index	= 0;
	while (selectedBand [index]. skip)
	   index ++;
	if (selectedBand [index]. fKHz == 0)
	   return 0;
	return index;
}

int	bandHandler::nextChannel	(int index) {
int	hulp	= index;
	do {
	   hulp ++;
	   if (selectedBand [hulp]. fKHz == 0)
	      index = 0;
	} while (selectedBand [hulp]. skip && (hulp != index));
	return hulp;
}

int	bandHandler::lastOf	(dabFrequencies *b) {
int	index;
	for (index = 0; selectedBand [index]. fKHz != 0; index ++);
	return index - 1;
}

int	bandHandler::prevChannel	(int index) {
int hulp	= index;
	do {
	   if (hulp == 0)
	      hulp = lastOf (selectedBand);
	   else
	      hulp --;
	} while (selectedBand [hulp]. skip && (hulp != index));
	return hulp;
}

void    bandHandler::cellSelected (int row, int column) {
QString s1 = theTable. item (row, 0) ->text ();
QString s2 = theTable. item (row, 1) ->text ();
int	amount_P	= 0;
	(void)column;
        if (s2 == "-") 
           theTable. item (row, 1) -> setText ("+");
	else
           theTable. item (row, 1) -> setText ("-");
	selectedBand [row]. skip = s2 != "-";
//	fprintf (stderr, "we zetten voor %s de zaak op %d\n",
//	              selectedBand [row]. key. toUtf8 (). data (),
//	              selectedBand [row]. skip);
}

void	bandHandler::show () {
	theTable. show ();
}

void	bandHandler::hide () {
	theTable. hide ();
}

bool	bandHandler::isHidden () {
	return theTable. isHidden ();
}

