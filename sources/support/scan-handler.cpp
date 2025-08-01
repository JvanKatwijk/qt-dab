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
#include        <QDataStream>
#include	<QSettings>
#include	"scan-handler.h"
#include        "radio.h"
#include	"dab-constants.h"
#include	"settingNames.h"
#include	"position-handler.h"
#include	"settings-handler.h"

static const
char    *scanTextTable [3] = {
        "single scan",
        "scan to data",
        "scan continuously"
};

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

static inline
QString scanmodeText (int e) {
        return QString (scanTextTable [e]);
}

	scanHandler::scanHandler (RadioInterface *theRadio,
	                                         QSettings *s,
	                                         const QString &extFile):
	                                             myWidget (nullptr),
	                                             scanTable (s),
	                                             filenameFinder (s) {
	this	-> theRadio		= theRadio;
	this	-> dabSettings		= s;
	this	-> selectedBand		= nullptr;
	this	-> no_scanTables	= false;
#ifndef	__MINGW32__
	if (extFile != "") 
	   selectedBand = load_extFile (extFile);
#else
	(void)extFile;
#endif
	if (selectedBand != nullptr)
	   no_scanTables	= true;
	else {
	   QString	defaultString	= "VHF BAND III";
	   QString t       =
                value_s (dabSettings, SCANNER, S_DABBAND, defaultString);   
           selectedBand         = t == defaultString ? frequencies_1 :
	                                                frequencies_2;
	   no_scanTables	= t != defaultString;
	}
	QStringList	header;
	kopLine		= new QLabel ("scan status");
	startKnop	= new QPushButton ("start");
	startKnop	-> setToolTip ("start the scan with the current settings");
	stopKnop	= new QPushButton ("stop");
	showKnop	= new QPushButton ("show");
	showKnop	-> setToolTip ("show the scanlist, the list indicating which channels should NOT be skipped when scanning");
	dumpChecker	= new QCheckBox ("dump input");
	dumpChecker	-> setToolTip ("if set, during the scan the input data for each channel with DAB data is stored in an uff file");
	dumpDirKnop	= new QPushButton ("dumpdir");
	dumpDirKnop	-> setToolTip ("set the directory where the scan files are stored");
        connect (dumpDirKnop, &QPushButton::clicked,
                 this, &scanHandler::setDumpDir);
	clearKnop	= new QPushButton ("clear");
	clearKnop	-> setToolTip ("clear the current scantable, i.e. scan all channels");
	defaultLoad	= new QPushButton ("load default");
	defaultLoad	-> setToolTip ("load the default scantable. Note that a \'+\' in the scanTable means SCAN this channel");
	defaultStore	= new QPushButton ("store default");
	defaultStore	-> setToolTip ("store the current settings of the scantable into the qt-dab.ini file");
	loadKnop	= new QPushButton ("load scanfile");
	storeKnop	= new QPushButton ("store scanfile");
	scanModeSelector	= new QComboBox ();
        contentWidget	= new QTableWidget (0, 4);
	QHBoxLayout *LH	= new QHBoxLayout ();
	QHBoxLayout *LH_2	= new QHBoxLayout ();
	LH		-> addWidget (startKnop);
	LH		-> addWidget (stopKnop);
	LH		-> addWidget (showKnop);
	LH		-> addWidget (clearKnop);
	LH		-> addWidget (dumpDirKnop);
	LH		-> addWidget (dumpChecker);
	LH_2		-> addWidget (defaultLoad);
	LH_2		-> addWidget (defaultStore);
	LH_2		-> addWidget (loadKnop);
	LH_2		-> addWidget (storeKnop);
	QVBoxLayout *LV	= new QVBoxLayout ();
	LV		-> addWidget (kopLine);
	LV		-> addLayout (LH);
	LV		-> addLayout (LH_2);
	LV		-> addWidget (scanModeSelector);
	LV		-> addWidget (contentWidget);
        myWidget. setLayout (LV);
	myWidget. setWindowTitle ("scan monitor");
	setPositionAndSize (dabSettings, &myWidget, SCAN_HANDLER);

	if (!no_scanTables)
	   scanTable. setup_scanTable (selectedBand);
	scanModeSelector -> addItem ("single scan");
	scanModeSelector -> addItem ("until data");
	scanModeSelector -> addItem ("continuous");
	scanMode	=
	             value_i (dabSettings, SCANNER,
	                            S_SCAN_MODE,  SINGLE_SCAN);
	scanModeSelector	-> setCurrentIndex (scanMode);
	connect (startKnop, &QPushButton::clicked,
	         this, &scanHandler::handle_startKnop);
	connect (stopKnop, &QPushButton::clicked,
	         this, &scanHandler::handle_stopKnop);
	connect (scanModeSelector,
	                qOverload<int>(&QComboBox::currentIndexChanged),
	         this, &scanHandler::handle_scanMode);
	if (!no_scanTables) {
	   connect (defaultLoad, &QPushButton::clicked,
	            this, &scanHandler::handle_defaultLoad);
	   connect (defaultStore, &QPushButton::clicked,
	            this, &scanHandler::handle_defaultStore);
	   connect (loadKnop, &QPushButton::clicked,
	            this, &scanHandler::handle_loadKnop);
	   connect (storeKnop, &QPushButton::clicked,
	            this, &scanHandler::handle_storeKnop);
	   connect (showKnop, &QPushButton::clicked,
	            this, &scanHandler::handle_showKnop);
	   connect (clearKnop, &QPushButton::clicked,
	            this, &scanHandler::handle_clearKnop);
	}
	contentWidget	-> setColumnWidth (0, 60);
	contentWidget	-> setColumnWidth (1, 150);
	contentWidget	-> setColumnWidth (2, 100);
	contentWidget	-> setColumnWidth (3, 250);
	header << "channel" << "ensemble" << "nrServices" << "transmitters";
        contentWidget	-> setHorizontalHeaderLabels (header);
	addRow ();	// for the ensemble name
	totalServices	= 0;

	scanFile	= "";
	scanning. store (false);
//
	connect (this, &scanHandler::startScanning,
	         theRadio, &RadioInterface::startScanning);
	connect (this, &scanHandler::stopScanning,
	         theRadio, &RadioInterface::stopScanning);
}

	scanHandler::~scanHandler () {
	if (!myWidget. isHidden ())
	   storeWidgetPosition (dabSettings, &myWidget, SCAN_HANDLER);
	clearTable	();
	scanTable. hide_scanTable ();
        delete  contentWidget;
}

QStringList	scanHandler::getChannelNames () {
QStringList res;
	for (int i = 0; selectedBand [i]. fKHz != 0; i ++)
	   res << selectedBand [i]. key;
	return res;
}

int32_t		scanHandler::Frequency (const QString &s) {
	for (int i = 0; selectedBand [i]. fKHz != 0; i ++) {
	   if (selectedBand [i]. key == s. trimmed ())
	      return selectedBand [i]. fKHz * KHz (1);
	}
	return selectedBand [0]. fKHz * KHz (1);
}
	
void	scanHandler::clearTable	() {
int	rows	= contentWidget -> rowCount ();
	for (int i = rows; i > 0; i --) 
	   contentWidget -> removeRow (i - 1);
	addRow ();	// for the ensemble name
}

void	scanHandler::show	() {
	myWidget. show ();
}

void	scanHandler::hide	() {
	myWidget. hide ();
	scanTable. hide_scanTable ();
}

bool	scanHandler::isVisible	() {
	return !myWidget. isHidden ();
}

int16_t	scanHandler::addRow () {
int16_t row	= contentWidget -> rowCount ();

        contentWidget     -> insertRow (row);

	for (int i = 0; i < 4; i ++) {
           QTableWidgetItem *item0 = new QTableWidgetItem;
	   if (i < 2)
              item0           -> setTextAlignment (Qt::AlignHCenter |
	                                                Qt::AlignVCenter);
	   else
              item0           -> setTextAlignment (Qt::AlignRight |
	                                                Qt::AlignVCenter);
           contentWidget     -> setItem (row, i, item0);
	}
	contentWidget	-> item (row, 2) -> setText (" ");
	contentWidget	-> item (row, 3) -> setText (" ");
	return row;
}

void	scanHandler::addEnsemble (const QString &channel,
	                               const QString &name)  {
	for (int i = 1; i < contentWidget -> rowCount (); i ++) {
	   QString ch = contentWidget -> item (i, 0) -> text ();
           if (ch == channel) {
	      contentWidget -> item (i, 1) -> setText (name);
	      return;
	   }
	}
        int row = addRow ();
        contentWidget -> item (row, 0) -> setText (channel);
        contentWidget -> item (row, 1) -> setText (name);
        contentWidget -> item (row, 2) -> setText ("0");
}

void	scanHandler::addTransmitters	(const QStringList &ss, 
	                                 const QString &channel) {
	if (ss. size () < 1)
	   return;
	for (int i = 1; i < contentWidget -> rowCount (); i ++) {
	   QString ch = contentWidget -> item (i, 0) -> text ();
	   if (ch == channel)  {
	      contentWidget -> item (i, 3) -> setText (ss [0]);
	      for (int j = 1; j < ss. size (); j ++) {
	         int row = addRow ();
                 contentWidget -> item (row, 3) -> setText (ss [j]);
	      }
	      return;
	   }
	}
}

void	scanHandler::addService (const QString &channel) {
	if (scan_to_data ())
	   return;
	totalServices ++;
	for (int i = 1; i < contentWidget -> rowCount (); i ++) {
	   QString ch = contentWidget -> item (i, 0) -> text ();
	   if (ch == channel) {
	      QString nr = contentWidget -> item (i, 2) -> text ();
	      bool success;
	      int amount = nr. toInt (&success);
	      if (success) 
	         amount ++;
	      else	
	         amount = 1;
	      contentWidget -> item (i, 2) -> setText (QString::number (amount));
	      contentWidget -> item (0, 2) -> setText (QString::number (totalServices));
	      return;
	   }
	}
//
//	no channel found so far
	int row	= addRow ();
	contentWidget -> item (row, 0) -> setText (channel);
	contentWidget -> item (row, 2) -> setText ("1");
	contentWidget -> item (0, 2) -> setText (QString::number (totalServices));
}

void	scanHandler::addText (const QString &line) {
	kopLine	-> setText (line);
}

void	scanHandler::handle_startKnop () {
	if (scanning. load ())
	   return;
	clearTable ();
	totalServices	= 0;
	startScanning ();
	scanning. store (true);
}

void	scanHandler::handle_stopKnop () {
	stopScanning ();
	scanning. store (false);
}

void	scanHandler::handle_scanMode (int index) {
	scanMode	= index;
	store (dabSettings, SCANNER, S_SCAN_MODE, scanMode);
}

void	scanHandler::handle_showKnop	() {
	if (scanTable. isHidden ())
	   scanTable. show_scanTable ();
	else
	   scanTable. hide_scanTable ();
}

void	scanHandler::handle_clearKnop	() {
	scanTable. clear_scanTable ();
}

void	scanHandler::handle_defaultLoad	() {
	scanTable. load_scanTable ("");
	scanFile	= "";
}

void	scanHandler::handle_defaultStore () {
	scanTable. save_scanTable ("");
}

void	scanHandler::handle_loadKnop	() {
QString saveDir	= filenameFinder. basicPath ();

	scanFile  = QFileDialog::getOpenFileName (nullptr,
	                                          "Open file ...",
	                                          saveDir,
	                                          "xml data (*.xml)");
	if (scanFile == QString ("")) 
	   return;

	scanTable. load_scanTable (scanFile);
}

void	scanHandler::handle_storeKnop	() {
QString saveDir = filenameFinder. basicPath ();

	scanFile  = QFileDialog::getSaveFileName (nullptr,
	                                          "Open file ...",
	                                          saveDir,
	                                          "xml data (*.xml)");
	if (scanFile == QString ("")) 
	   return;
	if (!scanFile. endsWith (".xml"))
	   scanFile += ".xml";
	scanTable. save_scanTable (scanFile);
}

void	scanHandler::setStop () {
	kopLine	-> setText ("Scan ended");
	scanning. store (false);
}

bool	scanHandler::scan_to_data () {
	return scanMode == SCAN_TO_DATA;
}
bool	scanHandler::scan_single	() {
	return scanMode == SCAN_SINGLE;
}

bool	scanHandler::scan_continuous	() {
	return scanMode == SCAN_CONTINUOUS;
}

bool	scanHandler::active	()  {
	return scanning. load ();
}

FILE	*scanHandler::askFileName	() {
	if (scan_single ()) {
	   QMessageBox::StandardButton resultButton =
	             QMessageBox::question (nullptr, "Qt-DAB",
	                                    "save the scan?\n",
	                                    QMessageBox::No | QMessageBox::Yes,
	                                    QMessageBox::Yes);
	   if (resultButton == QMessageBox::Yes)
	     return  filenameFinder. findScanDump_fileName ();
	}
	else
	if (scan_continuous ()) {
	   QMessageBox::StandardButton resultButton =
                     QMessageBox::question (nullptr, "Qt-DAB",
                                            "save summary?\n",
                                            QMessageBox::No | QMessageBox::Yes,
                                            QMessageBox::Yes);
           if (resultButton == QMessageBox::Yes)
	      return  filenameFinder. findSummary_fileName ();
	}
	return nullptr;
}

void	scanHandler::setDumpDir		() {
QString dir     =
          QFileDialog::getExistingDirectory (nullptr,
                                             tr("Open Directory"),
                                             QDir::homePath (),
                                             QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);
        if (dir != "")
           store (dabSettings, DAB_GENERAL,  S_SCANFILE_PATH, dir);
}

QString	scanHandler::getFirstChannel	() {
	currentChannel = 0;
	while ((selectedBand [currentChannel]. fKHz != 0) &&
	                 (!selectedBand [currentChannel]. scan )) 
	   currentChannel ++;
	if (selectedBand [currentChannel]. fKHz == 0)
	   currentChannel = 0;
	addText ("scanning channel " + selectedBand [currentChannel]. key);
	return selectedBand [currentChannel]. key;
}

QString	scanHandler::getNextChannel	() {
	currentChannel ++;
	while ((selectedBand [currentChannel]. fKHz != 0) &&
                         (!selectedBand [currentChannel]. scan )) {
	   fprintf (stderr, "Skipping channel %d\n", currentChannel);
           currentChannel ++;
	}
//
//	are we at the end of the list?
	if (selectedBand [currentChannel]. fKHz == 0) {
	   if (scanMode == SCAN_SINGLE)
	      throw (21);
	   else 
	      currentChannel = 0;
	}
	addText ("scanning channel " + selectedBand [currentChannel]. key);
	return selectedBand [currentChannel]. key;
}

QString	scanHandler::getNextChannel	(const QString &s) {
	for (currentChannel = 0;
	           selectedBand [currentChannel]. fKHz != 0; currentChannel ++)
	   if (selectedBand [currentChannel]. key == s)
	      return getNextChannel ();
	currentChannel = 0;
	return selectedBand [currentChannel]. key;
}

QString	scanHandler::getChannel	(int frequency) {
	for (int i = 0; selectedBand [i]. key != nullptr; i ++) 
	   if (selectedBand [i]. fKHz == frequency / 1000)
	      return selectedBand [i]. key;
	return "";
}

int16_t scanHandler::channelIndex (const QString &channel) {
	for (int i = 0; selectedBand [i]. key != nullptr; i ++)
	   if (selectedBand [i]. key == channel)
	      return i;
	return 0;
}

bool	scanHandler::dumpInFile	() {
	return dumpChecker -> isChecked ();
}

#ifndef	__MINGW32__
dabFrequencies alternatives [128];
dabFrequencies *scanHandler::load_extFile	(const QString &extFile) {
FILE *f	= fopen (extFile. toUtf8 (). data (), "r");
	if (f == nullptr)
	   return nullptr;

//	OK we have a file with - hopefully - some input
	size_t	amount		= 128;
	int filler		= 0;
	char *line 		= (char *)malloc (amount);
	while ((filler < 100) && (amount > 0)) {
	   amount = getline (&line, &amount, f);
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
	   alternatives [filler]. key	= QString (channelName);
	   alternatives [filler]. fKHz	= freq;
	   alternatives [filler]. scan	= true;
	   filler ++;
	}

	free (line);
	alternatives [filler]. key 	= "";
	alternatives [filler]. fKHz	= 0;
	fclose (f);
	return alternatives;
}
#endif
