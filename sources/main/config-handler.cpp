#
/*
 *    Copyright (C)  2015, 2025
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
#include	<QSettings>
#include	<QDebug>
#include	<QStringList>
#include	<QStringListModel>
#include	<QColorDialog>
#include	<QDir>
#include	"dab-constants.h"
#include	"config-handler.h"
#include	"mapport.h"
#include	"skin-handler.h"
#include	"radio.h"
#include	"position-handler.h"
#include	"settingNames.h"
#include	"settings-handler.h"
#include	"audiosystem-selector.h"

#define AUDIOSELECT_BUTTON      QString ("audioSelectButton")
#define FONT_BUTTON             QString ("fontButton")
#define FONTCOLOR_BUTTON        QString ("fontColorButton")

#define DEVICEWIDGET_BUTTON     QString ("devicewidgetButton")
#define PORT_SELECTOR           QString ("portSelector")
#define	DLTEXT_BUTTON		QString ("dlTextButton")
#define RESET_BUTTON            QString ("resetButton")
#define SCHEDULE_BUTTON         QString ("scheduleButton")

#define SNR_BUTTON              QString ("snrButton")
#define SET_COORDINATES_BUTTON  QString ("set_coordinatesButton")
#define LOAD_TABLE_BUTTON       QString ("loadTableButton")
#define SKIN_BUTTON             QString ("skinButton")
#define DUMP_BUTTON             QString ("dumpButton")

#define	PATH_BUTTON		QString ("pathButton")

#define	WHITE	"#ffffff"
#define	BLACK	"#000000"
#define	GREEN	"#8ff0a4"
#define	BLUE	"#00ffff"
#define	RED	"#ff007f"
#define	YELLOW	"#f9f06b"

static struct {
	QString	decoderName;
	int	decoderKey;
} decoders []  = {
{"decoder_a", DECODER_1},
{"decoder_b", DECODER_2},
{"decoder_c", DECODER_3},
{"decoder_d", DECODER_4},
{"", 0}
};

static
int	index_for_key (int key) {
	for (int i = 0; decoders [i]. decoderKey != 0; i ++)
	   if (decoders [i]. decoderKey == key)
	      return i;
	return 0;
}

	configHandler::configHandler (RadioInterface *parent,
	                              QSettings *settings):
	                                     superFrame (nullptr) {
	this	-> myRadioInterface	= parent;
	this	-> dabSettings		= settings;
	this	-> setupUi (this);
	setPositionAndSize (settings, this, CONFIG_HANDLER);
	hide ();
//	connect (&myFrame, &superFrame::makePicture,
//	         this, &configHandler::handle_mouseClicked);
//	inits of checkboxes etc in the configuration widget,
//	note that ONLY the GUI is set, values are not used
	
	int x =  value_i (dabSettings, CONFIG_HANDLER, MUTE_TIME_SETTING, 10);
	this	-> muteTimeSetting -> setValue (x);

        int fontSize    =  
	         value_i (dabSettings, COLOR_SETTINGS, "fontSize", 10);
	this	-> fontSizeSelector -> setValue (fontSize);

	x = value_i (dabSettings, CONFIG_HANDLER, SWITCH_VALUE_SETTING,
	                               DEFAULT_SWITCHVALUE);
	this -> switchDelaySetting -> setValue (x);

	x = value_i (dabSettings, CONFIG_HANDLER, "localDB", 1);
	this	-> localDB_selector	-> setChecked (x);

	x = value_i (dabSettings, CONFIG_HANDLER, SWITCH_STAY_SETTING,
	                                           10);
	this	-> switchStaySetting -> setValue (x);

	x = value_i ( dabSettings, CONFIG_HANDLER, SERVICE_ORDER_SETTING,
	                               ALPHA_BASED);
	if (x == ALPHA_BASED)
	   this -> orderAlfabetical -> setChecked (true);
	else
	if (x == ID_BASED)
	   this -> orderServiceIds -> setChecked (true);
	else
	   this -> ordersubChannelIds -> setChecked (true);
	serviceOrder	= x;
//
//	first row of checkboxes
//	unused element
	x =  value_i (dabSettings, CONFIG_HANDLER, LOG_MODE, 1);
	if (x != 0)
	   this -> logger_selector -> setChecked (true);
//
	bool b	= value_i (dabSettings, CONFIG_HANDLER,
                                        UTC_SELECTOR_SETTING, 0) == 1;
	this -> utc_selector -> setChecked (b);

	b =  value_i (dabSettings, CONFIG_HANDLER,
	                                ON_TOP_SETTING, 0) == 1;
	this ->  onTop -> setChecked (b);
//
	b = value_i (dabSettings, CONFIG_HANDLER,
	                          "close_map", 0) != 0;
	this	-> close_mapSelector	-> setChecked (b);

//	third row of checkboxes
	b = value_i (dabSettings, CONFIG_HANDLER,
	                          CLOSE_DIRECT_SETTING, 0) != 0;
	this -> closeDirect_selector -> setChecked (b);

	b = value_i (dabSettings, CONFIG_HANDLER,
	                           LOCAL_BROWSER_SETTING, 1) != 0;
	this -> localBrowserSelector -> setChecked (b);
//
//	fifth row of checkboxes
	b = value_i (dabSettings, CONFIG_HANDLER,
	                           SHOWALL_SETTING, 1) == 1;
	this -> showAll_selector -> setChecked (b);

	b = value_i (dabSettings, CONFIG_HANDLER,
	                           SAVE_SLIDES_SETTING, 1) == 1;
	this	-> saveSlides -> setChecked (b);

	b = value_i (dabSettings, CONFIG_HANDLER,
	                           TRANSMITTER_NAMES_SETTING, 0) == 1;
	b =  value_i (dabSettings, CONFIG_HANDLER,
	                           S_CORRELATION_ORDER, 0) != 0;
	this	-> correlationSelector -> setChecked (b);

	b = value_i (dabSettings, CONFIG_HANDLER, AUDIOSERVICES_ONLY, 1);
	this	-> audioServices_only -> setChecked (b);

	b = value_i (dabSettings, CONFIG_HANDLER, AUTO_HTTP, 9) != 0;
	this	-> auto_http -> setChecked (b);

	b = value_i (dabSettings, CONFIG_HANDLER,
	                          "dcRemoval", 1) == 1;
	this -> dcRemoval -> setChecked (b);

	int c = value_i (dabSettings, CONFIG_HANDLER, "tiiCollisions", 0);
	this	-> tiiCollisions -> setValue (c);

#ifndef	__MSC_THREAD__
	for (int i = 0; decoders [i]. decoderName != ""; i ++) 
	  this ->  decoderSelector -> addItem (decoders [i]. decoderName);
#else
	this -> decoderSelector -> setEnabled (false);
#endif

	int d	= value_i (dabSettings, CONFIG_HANDLER,
	                                 SHOWALL_TII, 1);
	this	-> allTIISelector	-> setChecked (d != 0);

	int k	= value_i (dabSettings, CONFIG_HANDLER,
	                                 DECODERS, DECODER_1);
	decoderSelector	-> setCurrentIndex (index_for_key (k));

	tiiCollisions -> setEnabled (false);
	tiiThreshold_setter -> setMinimum (6);

	int v = value_i (dabSettings, CONFIG_HANDLER,
	                             TII_THRESHOLD, 12);
	this -> tiiThreshold_setter -> setValue (v);
	connect (tiiThreshold_setter, qOverload<int>(&QSpinBox::valueChanged),
	         myRadioInterface, &RadioInterface::handle_tiiThreshold);

	connect (this, &configHandler::process_tiiCollisions,
	         myRadioInterface, &RadioInterface::handle_tiiCollisions);
	connect (pathButton, &QPushButton::clicked,
	         this, &configHandler::handle_pathButton);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (auto_http, &QCheckBox::checkStateChanged,
#else
	connect (auto_http, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_auto_http);
	connect (tiiCollisions, qOverload<int>(&QSpinBox::valueChanged),
	         this, &configHandler::handle_tiiCollisions);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (allTIISelector, &QCheckBox::checkStateChanged,
#else
	connect (allTIISelector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_allTIISelector);
	connect (activeServices, &clickablelabel::clicked,
	         myRadioInterface, &RadioInterface::handle_activeServices);
	connect (this, &configHandler::set_dcRemoval,
	         myRadioInterface, &RadioInterface::handle_dcRemoval);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (close_mapSelector, &QCheckBox::checkStateChanged,
#else
	connect (close_mapSelector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_close_mapSelector);
//
//	Tracer special
	connect	(tracerButton, &QPushButton::clicked,
	         this, &configHandler::handle_tracerButton);
	traceOn	= false;
	set_Colors ();
}

	configHandler::~configHandler	() {
	hide ();
}

void	configHandler::storePosition () {
	if (!isHidden ())
	   storeWidgetPosition (dabSettings, this, CONFIG_HANDLER);
}

void	configHandler::set_connections () {
	connect (audioSelectButton, &smallPushButton::clicked,
	         this, &configHandler::handle_audioSelectButton);
	connect (this, &configHandler::selectDecoder,
	         myRadioInterface, &RadioInterface::selectDecoder);
	connect (this, &configHandler::set_transmitters_local,
	         myRadioInterface, &RadioInterface::set_transmitters_local);

	connect (audioSelectButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_audioSelectButton);
	connect (fontButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_fontButton);
	connect (fontColorButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_fontColorButton );
	connect (devicewidgetButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_devicewidgetButton);
	connect (portSelector, &smallPushButton::rightClicked,
	         this, &configHandler::color_portSelector);
	connect (dlTextButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_dlTextButton);
	connect (resetButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_resetButton);
	connect (scheduleButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_scheduleButton);
	connect (snrButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_snrButton);
	connect (set_coordinatesButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_set_coordinatesButton);
	connect (loadTableButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_loadTableButton);
	connect (dumpButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_sourcedumpButton);
	connect (pathButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_pathButton);
	connect (skinButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_skinButton);

//
//	real handlers
	connect (scheduleButton, &QPushButton::clicked,
                 myRadioInterface, &RadioInterface::handle_scheduleButton);
	connect (muteTimeSetting, qOverload<int>(&QSpinBox::valueChanged),
	         this, &configHandler::handle_muteTimeSetting);
	connect (switchDelaySetting, qOverload<int>(&QSpinBox::valueChanged),
	         this, &configHandler::handle_switchDelaySetting);
	connect (switchStaySetting, qOverload<int>(&QSpinBox::valueChanged),
	         this, &configHandler::handle_switchStaySetting);
	connect (orderAlfabetical, &QRadioButton::clicked,
	         this, &configHandler::handle_orderAlfabetical);
	connect (orderServiceIds, &QRadioButton::clicked,
	         this, &configHandler::handle_orderServiceIds);
	connect (ordersubChannelIds, &QRadioButton::clicked,
	         this, &configHandler::handle_ordersubChannelIds);
//
	connect (fontButton, &QPushButton::clicked,
	         this,  &configHandler::handle_fontSelect);
	connect (fontColorButton, &QPushButton::clicked,
	         this, &configHandler::handle_fontColorSelect);
	connect (fontSizeSelector, qOverload<int>(&QSpinBox::valueChanged),
	         this, &configHandler::handle_fontSizeSelect);
//
//	Now the two rows with buttons
//
	connect (devicewidgetButton, &QPushButton::clicked,
	         myRadioInterface, &RadioInterface::handle_devicewidgetButton);
	connect (portSelector, &QPushButton::clicked,
	         this, &configHandler::handle_portSelector);
	connect (dlTextButton, &QPushButton::clicked,
	         myRadioInterface, &RadioInterface::handle_dlTextButton);
	connect (resetButton, &QPushButton::clicked,
	         myRadioInterface, &RadioInterface::handle_resetButton);
//
//	second row
	connect (snrButton, &QPushButton::clicked,
	         myRadioInterface, &RadioInterface::handle_snrButton);
	connect (set_coordinatesButton, &QPushButton::clicked,
	         myRadioInterface, &RadioInterface::handle_set_coordinatesButton );
	connect (loadTableButton, &QPushButton::clicked,
	         myRadioInterface, &RadioInterface::handle_loadTable);
	loadTableButton	-> setText ("refresh table");
//	however, by default loadTable is disabled
	loadTableButton	-> setEnabled (false);
	connect (dumpButton, &QPushButton::clicked,
	         myRadioInterface, &RadioInterface::handle_sourcedumpButton);
	connect (skinButton, &QPushButton::clicked,
	         this, &configHandler::handle_skinSelector);
//
//	Now the checkboxes
//	top line
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (audioServices_only, &QCheckBox::checkStateChanged,
#else
	connect (audioServices_only, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_audioServices_only);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (correlationSelector, &QCheckBox::checkStateChanged,
#else
	connect (correlationSelector, &QCheckBox::stateChanged,
#endif
	         myRadioInterface, &RadioInterface::handle_correlationSelector);
//
//	second line
	int upload = value_i (dabSettings, CONFIG_HANDLER,
	                              "UPLOAD_ENABLED", 0);
	if (upload != 0)
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	   connect (upload_selector, &QCheckBox::checkStateChanged,
#else
	   connect (upload_selector, &QCheckBox::stateChanged,
#endif
	            this, &configHandler::handle_upload_selector);
	else
	   upload_selector -> setEnabled (false);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (logger_selector, &QCheckBox::checkStateChanged,
#else
	connect (logger_selector, &QCheckBox::stateChanged,
#endif
	         myRadioInterface, &RadioInterface::handle_LoggerButton);

//	third line

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (utc_selector, &QCheckBox::checkStateChanged,
#else
	connect (utc_selector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_utc_selector);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (onTop, &QCheckBox::checkStateChanged,
#else
	connect (onTop, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_onTop);
//
//	fourthline

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (localDB_selector, &QCheckBox::checkStateChanged,
#else
	connect (localDB_selector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_localDB_Selector);
	connect (localBrowserSelector,
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	             &QCheckBox::checkStateChanged,
#else
		     &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_localBrowser);
//
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (etiActivated_selector, &QCheckBox::checkStateChanged,
#else
	connect (etiActivated_selector, &QCheckBox::stateChanged,
#endif
	         myRadioInterface, &RadioInterface::handle_eti_activeSelector);
//
//	fifh line
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (showAll_selector, &QCheckBox::checkStateChanged,
#else
	connect (showAll_selector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_showAll_Selector);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (saveSlides, &QCheckBox::checkStateChanged,
#else
	connect (saveSlider, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_saveSlides);


//	sixth ine
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (dcRemoval, &QCheckBox::checkStateChanged,
#else
	connect (dcRemoval, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_dcRemoval);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (saveTitlesSelector, &QCheckBox::checkStateChanged,
#else
	connect (saveTitlesSelector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_saveTitles);
//
//	botton row
	connect (decoderSelector,
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 2)
	         &QComboBox::textActivated,
#else
	         qOverload<const QString &>(&QComboBox::activated),
#endif
	         this, &configHandler::handle_decoderSelector);
}

/////////////////////////////////////////////////////////////////////////
//	
void	configHandler::set_Colors () {
QString	audioSelectButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                              AUDIOSELECT_BUTTON + "_color", GREEN);
QString audioSelectButton_font	=
	   value_s (dabSettings, COLOR_SETTINGS,
	                              AUDIOSELECT_BUTTON + "_font", BLACK);

QString fontButton_font	=
	   value_s (dabSettings, COLOR_SETTINGS,
	                              FONT_BUTTON + "_font", BLACK);
QString	fontButton_color =
	   value_s (dabSettings, COLOR_SETTINGS, 
	                              FONT_BUTTON + "_color", WHITE);

QString fontColorButton_font	=
	   value_s (dabSettings, COLOR_SETTINGS,
	                              FONTCOLOR_BUTTON + "_font", WHITE);
QString	fontColorButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                              FONTCOLOR_BUTTON + "_color", BLACK);

QString devicewidgetButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                              DEVICEWIDGET_BUTTON + "_color", YELLOW);
QString devicewidgetButton_font =
	   value_s (dabSettings, COLOR_SETTINGS, 
	                              DEVICEWIDGET_BUTTON + "_font", BLACK);

QString portSelector_font	=
	   value_s (dabSettings, COLOR_SETTINGS,
	                              PORT_SELECTOR + "_font", BLACK);
QString	portSelector_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                              PORT_SELECTOR + "_color", YELLOW);

QString	dlTextButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                              DLTEXT_BUTTON + "_color", YELLOW);
QString dlTextButton_font	=
	   value_s (dabSettings, COLOR_SETTINGS,
	                              DLTEXT_BUTTON + "_font", BLACK);

QString resetButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                              RESET_BUTTON + "_color", RED);
QString resetButton_font =
	   value_s (dabSettings, COLOR_SETTINGS,
	                              RESET_BUTTON + "_font", WHITE);

QString	scheduleButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                              SCHEDULE_BUTTON + "_color", YELLOW);
QString scheduleButton_font	=
	   value_s (dabSettings, COLOR_SETTINGS, 
	                              SCHEDULE_BUTTON + "_font", BLACK);

QString snrButton_color =
	   value_s (dabSettings, COLOR_SETTINGS, 
	                              SNR_BUTTON + "_color", BLUE);
QString snrButton_font =
	   value_s (dabSettings, COLOR_SETTINGS,
	                              SNR_BUTTON + "_font", BLACK);

QString	set_coordinatesButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                              SET_COORDINATES_BUTTON + "_color", BLUE);
QString set_coordinatesButton_font	=
	   value_s (dabSettings, COLOR_SETTINGS,
	                              SET_COORDINATES_BUTTON + "_font", BLACK);
	
QString	loadTableButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                               LOAD_TABLE_BUTTON + "_color", RED);
QString loadTableButton_font	=
	   value_s (dabSettings, COLOR_SETTINGS,
	                               LOAD_TABLE_BUTTON + "_font", WHITE);

QString dumpButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                               DUMP_BUTTON + "_color", YELLOW);
QString dumpButton_font =
	   value_s (dabSettings, COLOR_SETTINGS,
	                               DUMP_BUTTON + "_font", BLACK);

QString pathButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                               PATH_BUTTON + "_color", GREEN);
QString pathButton_font =
	   value_s (dabSettings, COLOR_SETTINGS,
	                               PATH_BUTTON + "_font", BLACK);

QString skinButton_font	=
	   value_s (dabSettings, COLOR_SETTINGS,
	                               SKIN_BUTTON + "_font", BLACK);
QString	skinButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                               SKIN_BUTTON + "_color", YELLOW);

	QString temp = "QPushButton {background-color: %1; color: %2}";

	this -> audioSelectButton ->
	              setStyleSheet (temp. arg (audioSelectButton_color,
	                                        audioSelectButton_font));

	this -> fontButton ->
	              setStyleSheet (temp. arg (fontButton_color,
	                                        fontButton_font));

	this -> fontColorButton ->
	              setStyleSheet (temp. arg (fontColorButton_color,
	                                        fontColorButton_font));

	this -> devicewidgetButton ->
	              setStyleSheet (temp. arg (devicewidgetButton_color,
	                                        devicewidgetButton_font));

	this -> portSelector ->
	              setStyleSheet (temp. arg (portSelector_color,
	                                        portSelector_font));

	this -> dlTextButton ->
	              setStyleSheet (temp. arg (dlTextButton_color,
	                                        dlTextButton_font));

	this ->  resetButton ->
	              setStyleSheet (temp. arg (resetButton_color,	
	                                        resetButton_font));

	this -> scheduleButton ->
	              setStyleSheet (temp. arg (scheduleButton_color,
	                                        scheduleButton_font));

	this -> snrButton	->
	              setStyleSheet (temp. arg (snrButton_color,
	                                        snrButton_font));

	this -> set_coordinatesButton ->
	              setStyleSheet (temp. arg (set_coordinatesButton_color,
	                                        set_coordinatesButton_font));

	this -> loadTableButton ->
	              setStyleSheet (temp. arg (loadTableButton_color,
	                                        loadTableButton_font));
	this -> dumpButton ->
	              setStyleSheet (temp. arg (dumpButton_color,
	                                        dumpButton_font));
	this	-> pathButton ->
	              setStyleSheet (temp. arg (pathButton_color,
	                                        pathButton_font));
	
	this -> skinButton ->
	              setStyleSheet (temp. arg (skinButton_color,
	                                        skinButton_font));
}

void	configHandler::color_audioSelectButton	() {
	set_buttonColors (this -> audioSelectButton, AUDIOSELECT_BUTTON);
}

void	configHandler::color_fontButton	() 	{
	set_buttonColors (this -> fontButton, FONT_BUTTON);
}

void	configHandler::color_fontColorButton	() 	{
	set_buttonColors (this -> fontColorButton, FONTCOLOR_BUTTON);
}

void	configHandler::color_devicewidgetButton	() {
	set_buttonColors (this -> devicewidgetButton, DEVICEWIDGET_BUTTON);
}

void	configHandler::color_portSelector	() 	{
	set_buttonColors (this ->  portSelector, PORT_SELECTOR);
}

void	configHandler::color_dlTextButton	()	{
	set_buttonColors (this ->  dlTextButton, DLTEXT_BUTTON);
}

void	configHandler::color_resetButton	() {
	set_buttonColors (this ->  resetButton, RESET_BUTTON);
}

void	configHandler::color_scheduleButton	() 	{
	set_buttonColors (this ->  scheduleButton, SCHEDULE_BUTTON);
}

void	configHandler::color_snrButton		() {
	set_buttonColors (this ->  snrButton, SNR_BUTTON);
}

void	configHandler::color_set_coordinatesButton	() 	{
	set_buttonColors (this ->  set_coordinatesButton,
	                                          SET_COORDINATES_BUTTON);
}

void	configHandler::color_loadTableButton	() 	{
	set_buttonColors (this ->  loadTableButton, LOAD_TABLE_BUTTON);
}

void	configHandler::color_sourcedumpButton	()	{
	set_buttonColors (this ->  dumpButton, DUMP_BUTTON);
}

void	configHandler::color_pathButton		()	{
	set_buttonColors (this ->  pathButton, PATH_BUTTON);
}

void	configHandler::color_skinButton	() 	{
	set_buttonColors (this ->  skinButton, SKIN_BUTTON);
}


void	configHandler::set_buttonColors	(QPushButton *b,
	                                         const QString &buttonName) {
QColor	baseColor;
QColor	textColor;
QColor	color;

	color = QColorDialog::getColor (baseColor, nullptr, "baseColor");
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
	store (dabSettings, COLOR_SETTINGS, buttonColor, baseColor_name);
	store (dabSettings, COLOR_SETTINGS, buttonFont, textColor_name);
}

void	configHandler::handle_muteTimeSetting	(int newV) {
	store (dabSettings, CONFIG_HANDLER, MUTE_TIME_SETTING, newV);
}

void	configHandler::handle_switchDelaySetting	(int newV) {
	store (dabSettings, CONFIG_HANDLER, SWITCH_VALUE_SETTING, newV);
}

void	configHandler::handle_switchStaySetting	(int newV) {
	store (dabSettings, CONFIG_HANDLER, SWITCH_STAY_SETTING, newV);
}

void	configHandler::handle_orderAlfabetical		() {
	set_serviceOrder (ALPHA_BASED);
	serviceOrder	= ALPHA_BASED;
}

void	configHandler::handle_orderServiceIds		() {
	set_serviceOrder (ID_BASED);
	serviceOrder	= ID_BASED;
}

void	configHandler::handle_ordersubChannelIds	() {
	set_serviceOrder (SUBCH_BASED);
	serviceOrder	= SUBCH_BASED;
}

void	configHandler::handle_portSelector () {
QString oldPort	= value_s (dabSettings, MAP_HANDLING,
	                                 MAP_PORT_SETTING, "8080");
mapPortHandler theHandler (oldPort);
	int portNumber = theHandler. QDialog::exec ();
	if (portNumber != 0) {
	   QString ss = QString::number (portNumber);
	   store (dabSettings, MAP_HANDLING, MAP_PORT_SETTING, ss);
	}
}

void	configHandler::handle_skinSelector     () {
skinHandler theSkins;
	int skinIndex = theSkins. QDialog::exec ();
	QString skinName = theSkins. skins. at (skinIndex);
	store (dabSettings, "SKIN_HANDLING", SKIN_SETTING, skinName); 
}

void	configHandler::handle_onTop	(int d) {
bool onTop = false;
	(void)d;
	if (this ->  onTop -> isChecked ())
	   onTop = true;
	store (dabSettings, CONFIG_HANDLER, ON_TOP_SETTING, onTop ? 1 : 0);
}

void	configHandler::handle_localDB_Selector	(int x) {
	(void)x;
	store (dabSettings, CONFIG_HANDLER, "localDB", 
	                         localDB_selector -> isChecked () ? 1 : 0);
}

void	configHandler::handle_localBrowser	(int d) {
	(void)d;
	store (dabSettings, CONFIG_HANDLER, LOCAL_BROWSER_SETTING, 
	               this ->  localBrowserSelector -> isChecked () ? 1 : 0);
}

void	configHandler::handle_showAll_Selector (int c) {
	(void)c;
	store (dabSettings,  CONFIG_HANDLER, SHOWALL_SETTING,
	               this ->  showAll_selector -> isChecked () ? 1 : 0);
}

void	configHandler::handle_saveSlides	(int x) {
	(void)x;
	store (dabSettings, CONFIG_HANDLER, SAVE_SLIDES_SETTING, 
	                         this ->  saveSlides -> isChecked () ? 1 : 0);
}

void	configHandler::handle_decoderSelector	(const QString &s) {
int	decoder	= 0100;
	for (int i = 0; decoders [i]. decoderName != ""; i ++)
	   if (decoders [i]. decoderName == s) {
	      decoder = decoders [i]. decoderKey;
	      selectDecoder (decoder);
	      store (dabSettings, CONFIG_HANDLER, DECODERS, decoder);
	      break;
	   }
}

void	configHandler::handle_upload_selector (int d) {
	(void)d;
}

void	configHandler::handle_utc_selector	(int d) {
	(void)d;
	store (dabSettings, CONFIG_HANDLER, UTC_SELECTOR_SETTING, 
	                          utc_selector -> isChecked () ? 1 : 0);
}

int	configHandler::get_serviceOrder	() {
	return serviceOrder;
}

bool	configHandler::upload_selector_active	() {
	return upload_selector -> isChecked ();
}

bool	configHandler::closeDirect_active	() {
	return closeDirect_selector -> isChecked ();
}

bool	configHandler::showAll_Selector_active () {
	return showAll_selector	-> isChecked ();
}

//
//column 2
bool	configHandler::logger_active	() {
	return logger_selector	-> isChecked ();
}

bool	configHandler::utcSelector_active	() {
	return utc_selector -> isChecked ();
}

bool	configHandler::eti_active	() {
	return etiActivated_selector -> isChecked ();
}

bool	configHandler::saveSliders_active	() {
	return saveSlides -> isChecked ();
}

//
bool	configHandler::onTop_active	() {
	return onTop	-> isChecked ();
}

bool	configHandler::localBrowserSelector_active	() {
	return localBrowserSelector -> isChecked ();
}

static inline
void	setButtonFont (QPushButton *b, QString text, int size) {
	QFont font	= b -> font ();
	font. setPointSize (size);
	b		-> setFont (font);
	b		-> setText (text);
	b		-> update ();
}

void	configHandler::mark_dumpButton (bool b) {
	if (b)
	   setButtonFont (dumpButton, "writing", 12);
	else
	   setButtonFont (dumpButton, "Raw dump", 10);
}

void	configHandler::mark_dlTextButton (bool b) {
	if (b)
	   setButtonFont (dlTextButton, "writing", 12);
	else
	   setButtonFont (dlTextButton, "dlText", 10);
}

void	configHandler::set_closeDirect (bool b) {
	closeDirect_selector	-> setChecked (b);
}

void	configHandler::show_streamSelector	(bool b) {
	if (b)
	   streamoutSelector -> show ();
	else
	   streamoutSelector -> hide ();
}

void	configHandler::fill_streamTable	(const QStringList &sl) {
	for (int i = streamoutSelector -> count () - 1; i >= 0; i --)
	   streamoutSelector -> removeItem (i);
	for (auto sle : sl)
	   streamoutSelector -> addItem (sle);
}

int	configHandler::init_streamTable	(const QString &s) {
int k	=streamoutSelector -> findText (s);
	if (k > 0) 
	   streamoutSelector	-> setCurrentIndex (k);
	return k;
}

void	configHandler::connect_streamTable	() {
	connect (streamoutSelector, qOverload<int>(&QComboBox::activated),
	         myRadioInterface, &RadioInterface::set_streamSelector);
}

QString	configHandler::currentStream		() {
	if (streamoutSelector	-> count () == 0)
	   return "";
	return streamoutSelector	-> currentText ();
}

int	configHandler::switchDelayValue		() {
	return switchDelaySetting	-> value () * 1000;
}

int	configHandler::switchStayValue		() {
	return switchStaySetting	-> value () * 60000;
}

int	configHandler::muteValue		() {
	return  muteTimeSetting -> value ();
}

void	configHandler::showLoad		(float load) {
	loadDisplay	-> display (load);
}

void	configHandler::enable_loadLib	() {
	loadTableButton	-> setEnabled (true);
}

void	configHandler::handle_audioSelectButton	() {
audiosystemSelector the_selector (dabSettings);
	(void)the_selector. QDialog::exec ();
}

bool	configHandler::get_correlationSelector () {
	return correlationSelector -> isChecked ();
}

bool	configHandler::get_allTIISelector () {
	return allTIISelector -> isChecked () ? true : false;
}

void	configHandler::handle_allTIISelector	(int d) {
	(void)d;
	store (dabSettings, CONFIG_HANDLER, SHOWALL_TII, 
	      allTIISelector -> isChecked () ? 1 : 0);
}

void	configHandler::handle_tiiThreshold	(int t) {
	store (dabSettings, CONFIG_HANDLER, TII_THRESHOLD, t);
}

void	configHandler::handle_pathButton	() {
QString dir	=
	  QFileDialog::getExistingDirectory (nullptr,
	                                     tr("Open Directory"),
	                                     QDir::homePath (),
                                             QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);
	if (dir != "")
	   store (dabSettings, DAB_GENERAL,  S_FILE_PATH, dir);
}

bool	configHandler::get_audioServices_only () {
	return audioServices_only -> isChecked ();
}

void	configHandler::handle_auto_http	(int state) {
uint8_t x	= auto_http -> isChecked ();
	(void)state;
	store (dabSettings, CONFIG_HANDLER, "auto_http", x);
}

void	configHandler::handle_audioServices_only	(int state) {
uint8_t x       = audioServices_only -> isChecked ();
        (void)state;
        store (dabSettings, CONFIG_HANDLER, "audioServices_only", x);
}

void	configHandler::handle_tiiCollisions     (int value) {
        store (dabSettings, CONFIG_HANDLER, "tiiCollisions", value);
	process_tiiCollisions (value);
}

void	configHandler::handle_mouseClicked () {
//QString tempPath        = QDir::homePath () + "/Qt-DAB-files/";
//        tempPath                =
//               value_s (dabSettings, "CONFIG_HANDLER", S_FILE_PATH, tempPath);
//        if (!tempPath. endsWith ('/'))
//           tempPath             += '/';
//	QDir::fromNativeSeparators (tempPath);
//	QString fileName	= tempPath + "config-handler.png";
//	fprintf (stderr, "file : %s\n", fileName. toLatin1 (). data ());
//	myFrame. grab (). save (fileName);
}

void	configHandler::set_activeServices	(int activeS) {
	activeServices	-> setText (QString::number (activeS));
}

void	configHandler::handle_close_mapSelector (int h) {
	(void)h;
	bool b = this ->  close_mapSelector -> isChecked ();
	store (dabSettings, CONFIG_HANDLER, "close_map", b ? 1 : 0);
}

bool	configHandler::get_close_mapSelector	() {
	return close_mapSelector	-> isChecked ();
}

void	configHandler::handle_dcRemoval		(int h) {
	(void)h;
	bool b = this ->  dcRemoval -> isChecked ();
	store (dabSettings, CONFIG_HANDLER, "dcRemoval", b ? 1 : 0);
	emit set_dcRemoval (b);
}

bool	configHandler::get_dcRemoval		() {
	return dcRemoval -> isChecked ();
}

void	configHandler::handle_saveTitles	(int h) {
	(void)h;
}

bool	configHandler::get_saveTitles		() {
	return saveTitlesSelector -> isChecked ();
}

bool	configHandler::get_clearScanList       () {
	return clearScanList_selector	-> isChecked ();
}
//
void	configHandler::handle_tracerButton	() {
	traceOn	= !traceOn;
	if  (traceOn)
	   tracerButton	-> setText ("trace on");
	else
	   tracerButton	-> setText ("");
	emit signal_dataTracer	(traceOn);
}


