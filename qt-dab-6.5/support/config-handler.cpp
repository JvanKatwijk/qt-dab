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
#include	<QSettings>
#include	<QDebug>
#include	<QStringList>
#include	<QStringListModel>
#include	<QColorDialog>
#include	"dab-constants.h"
#include	"config-handler.h"
#include	"mapport.h"
#include	"skin-handler.h"
#include	"radio.h"

#include	"settingNames.h"
#include	"position-handler.h"

static struct {
	QString	decoderName;
	int	decoderKey;
} decoders []  = {
{"fast decoder", FAST_DECODER},
{"alt1 decoder", ALT1_DECODER},
{"alt2_decoder", ALT2_DECODER},
{"", 0}
};

	configHandler::configHandler (RadioInterface *parent,
	                              QSettings *settings):
	                                     myFrame (nullptr) {
	this	-> myRadioInterface	= parent;
	this	-> dabSettings		= settings;
	this -> setupUi (&myFrame);
	set_position_and_size (settings, &myFrame, CONFIG_HANDLER);
	hide ();
	connect (&myFrame, SIGNAL (frameClosed ()),
	         this, SIGNAL (frameClosed ()));
//	inits of checkboxes etc in the configuration widget,
//	note that ONLY the GUI is set, values are not used
	int x = dabSettings -> value (MUTE_TIME_SETTING, 10). toInt ();
	this	-> muteTimeSetting -> setValue (x);

        int fontSize    =  
                 dabSettings  -> value (FONT_SIZE_SETTING, 10). toInt ();
	this	-> fontSizeSelector -> setValue (fontSize);

	x = dabSettings -> value (SWITCH_VALUE_SETTING,
	                               DEFAULT_SWITCHVALUE). toInt ();
	this -> switchDelaySetting -> setValue (x);

	x = dabSettings -> value (SERVICE_ORDER_SETTING,
	                               ALPHA_BASED). toInt ();
	if (x == ALPHA_BASED)
	   this -> orderAlfabetical -> setChecked (true);
	else
	if (x == ID_BASED)
	   this -> orderServiceIds -> setChecked (true);
	else
	   this -> ordersubChannelIds -> setChecked (true);
	serviceOrder	= x;

//	first row of checkboxes
//	unused element
//	logger is set per process, not kept
	x = dabSettings -> value (EPG2XML_SETTING, 0). toInt ();
	if (x != 0)
	   this -> epg2xmlSelector -> setChecked (true);
//
//	second row of checkboxes
	bool dm = dabSettings -> value (TII_DETECTOR_SETTING,
	                                           0). toInt () == 1;
	this -> new_tiiMode_selector -> setChecked (dm);

	bool b	= dabSettings	-> value (UTC_SELECTOR_SETTING,
	                                           0). toInt () == 1;
	this -> utc_selector -> setChecked (b);

	b = dabSettings -> value (ON_TOP_SETTING, 0). toInt () == 1;
	this ->  onTop -> setChecked (b);
//
//	third row of checkboxes
	b = dabSettings -> value (CLOSE_DIRECT_SETTING, 0). toInt () == 1;
	this -> closeDirect_selector -> setChecked (b);

	b = dabSettings -> value (EPG_FLAG_SETTING, 0). toInt () == 1;
	this -> epg_selector -> setChecked (b);

	b = dabSettings -> value (LOCAL_BROWSER_SETTING, 1). toInt () == 1;
	this -> localBrowserSelector -> setChecked (b);
//
//	fourth row of checkboxes
//	dcRemoval	not connected yet

	b = dabSettings -> value (LOCAL_TRANSMITTERS_SETTING,
	                                           0). toInt () == 1;
	this -> localTransmitterSelector -> setChecked (b);
//
//	fifth row of checkboxes
	b = dabSettings -> value (CLEAR_SCAN_RESULT_SETTING, 1). toInt () == 1;
	this -> clearScan_selector -> setChecked (b);

	b = dabSettings -> value (SAVE_SLIDES_SETTING, 0). toInt () == 1;
	this	-> saveSlides -> setChecked (b);

	b = dabSettings -> value (TRANSMITTER_NAMES_SETTING, 0). toInt () == 1;
	this -> saveTransmittersSelector -> setChecked (b);
//
#ifndef	__MSC_THREAD__
	for (int i = 0; decoders [i]. decoderName != ""; i ++) 
	  this ->  decoderSelector -> addItem (decoders [i]. decoderName);
#else
	this -> decoderSelector -> setEnabled (false);
#endif
	set_Colors ();
}

	configHandler::~configHandler	() {
	QString handlerName = CONFIG_HANDLER;
	store_widget_position (dabSettings, &myFrame, "CONFIG_HANDLER");
}

void	configHandler::show		() {
	myFrame. show ();
}

void	configHandler::hide		() {
	myFrame. hide ();
}

bool	configHandler::isHidden		() {
	return myFrame. isHidden ();
}

void	configHandler::setDeviceList	(const QStringList &sl) {
	for (auto &sle : sl)
	   deviceSelector -> addItem (sle);
}

bool	configHandler::findDevice (const QString &dev) {
	int k = deviceSelector -> findText (dev);
	if (k != -1)
	   deviceSelector -> setCurrentIndex (k);
	return k >= 0;
}

void	configHandler::connectDevices	() {
	connect (deviceSelector, SIGNAL (activated (const QString &)),
	         myRadioInterface, SLOT (doStart (const QString &)));
}

void	configHandler::disconnectDevices () {
	disconnect (deviceSelector, SIGNAL (activated (const QString &)),
	            myRadioInterface, SLOT (doStart (const QString &)));
	disconnect (deviceSelector, SIGNAL (activated (const QString &)),
	            myRadioInterface, SLOT (newDevice (const QString &)));
}

void	configHandler::reconnectDevices () {
	connect (deviceSelector, SIGNAL (activated (const QString &)),
	         myRadioInterface, SLOT (newDevice (const QString &)));
}
	
void	configHandler::set_connections () {

	connect (this, SIGNAL (selectDecoder (int)),
	         myRadioInterface, SLOT (selectDecoder (int)));
	connect (this, SIGNAL (set_transmitters_local (bool)),
	         myRadioInterface, SLOT (set_transmitters_local (bool)));
	connect (this, SIGNAL (set_tii_detectorMode (bool)),
	         myRadioInterface, SLOT (set_tii_detectorMode (bool)));
	connect (this, SIGNAL (set_dcRemoval (bool)),
	         myRadioInterface, SLOT (handle_dcRemovalSelector (bool)));
	
	connect (fontButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_fontButton ()));
	connect (fontColorButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_fontColorButton ()));
	connect (devicewidgetButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_devicewidgetButton ()));
	connect (portSelector, SIGNAL (rightClicked ()),
	         this, SLOT (color_portSelector ()));
	connect (dlTextButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_dlTextButton ()));
	connect (resetButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_resetButton ()));
	connect (scheduleButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_resetButton ()));
	connect (snrButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_snrButton ()));
	connect (set_coordinatesButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_set_coordinatesButton ()));
	connect (loadTableButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_loadTableButton ()));
	connect (dumpButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_sourcedumpButton ()));
	connect (skinButton, SIGNAL (rightClicked ()),
	         this, SLOT (color_skinButton ()));
//
//	real handlers
	connect (scheduleButton, SIGNAL (clicked ()),
                 myRadioInterface, SLOT (handle_scheduleButton ()));
	connect (muteTimeSetting, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_muteTimeSetting (int)));
	connect (switchDelaySetting, SIGNAL (valueChanged (int)),
	         this, SLOT (handle_switchDelaySetting (int)));
	connect (orderAlfabetical, SIGNAL (clicked ()),
	         this, SLOT (handle_orderAlfabetical ()));
	connect (orderServiceIds, SIGNAL (clicked ()),
	         this, SLOT (handle_orderServiceIds ()));

	connect (ordersubChannelIds, SIGNAL (clicked ()),
	         this, SLOT (handle_ordersubChannelIds ()));
//
	connect (fontButton, SIGNAL (clicked ()),
	         this, SIGNAL (handle_fontSelect ()));
	connect (fontColorButton, SIGNAL (clicked ()),
	         this, SIGNAL (handle_fontColorSelect ()));
	connect (fontSizeSelector, SIGNAL (valueChanged (int)),
	         this, SIGNAL (handle_fontSizeSelect (int)));
//
//	Now the two rows with buttons
//
	connect (devicewidgetButton, SIGNAL (clicked ()),
	         myRadioInterface, SLOT (handle_devicewidgetButton ()));
	connect (portSelector, SIGNAL (clicked ()),
	         this, SLOT (handle_portSelector ()));
	connect (dlTextButton, SIGNAL (clicked ()),
	         myRadioInterface, SLOT (handle_dlTextButton ()));
	connect (resetButton, SIGNAL (clicked ()),
	         myRadioInterface, SLOT (handle_resetButton ()));
//
//	second row
	connect (snrButton, SIGNAL (clicked ()),
	         myRadioInterface, SLOT (handle_snrButton ()));
	connect (set_coordinatesButton, SIGNAL (clicked ()),
	         myRadioInterface, SLOT (handle_set_coordinatesButton ()));
	connect (loadTableButton, SIGNAL (clicked ()),
	         myRadioInterface, SLOT (handle_loadTable ()));
//
//	however, by default loadTable is disables
	loadTableButton	-> setEnabled (false);
	connect (dumpButton, SIGNAL (clicked ()),
	         myRadioInterface, SLOT (handle_sourcedumpButton ()));
	connect (skinButton, SIGNAL (clicked ()),
	         this, SLOT (handle_skinSelector ()));
//
//	Now the checkboxes
//	top line
	connect (logger_selector, SIGNAL (stateChanged (int)),
	         myRadioInterface, SLOT (handle_LoggerButton (int)));
//	the epg2xmlSelector is just polled, no need to react on an event

//	second line
	connect (new_tiiMode_selector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_tii_detectorMode (int)));

	connect (utc_selector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_utc_selector (int)));

	connect (onTop, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_onTop (int)));
//
//	third line
//	here we expect the close without asking
	connect (epg_selector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_epgSelector (int)));
	connect (localBrowserSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_localBrowser      (int)));
//
//	fourth line
	connect (dcRemovalSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_dcRemovalSelector (int)));
//	
	connect (etiActivated_selector, SIGNAL (stateChanged (int)),
	         myRadioInterface, SLOT (handle_eti_activeSelector (int)));
//
	connect (saveTransmittersSelector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_saveTransmittersSelector (int)));
//
//	fifh line
	connect (clearScan_selector, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_clearScan_Selector (int)));

	connect (saveSlides, SIGNAL (stateChanged (int)),
	         this, SLOT (handle_saveSlides (int)));
//
	connect (localTransmitterSelector, SIGNAL (stateChanged (int)),	
	         this, SLOT (handle_localTransmitterSelector  (int)));
//
//	botton row
	connect (decoderSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (handle_decoderSelector (const QString &)));
}

/////////////////////////////////////////////////////////////////////////
//	
void	configHandler::set_Colors () {
	dabSettings	-> beginGroup (COLOR_SETTINGS);

QString fontButton_font	=
	   dabSettings -> value (FONT_BUTTON + "_font",
	                                              "white"). toString ();
QString	fontButton_color =
	   dabSettings -> value (FONT_BUTTON + "_color",
	                                              "black"). toString ();

QString fontColorButton_font	=
	   dabSettings -> value (FONTCOLOR_BUTTON + "_font",
	                                              "white"). toString ();
QString	fontColorButton_color =
	   dabSettings -> value (FONTCOLOR_BUTTON + "_color",
	                                              "black"). toString ();

QString devicewidgetButton_color =
	   dabSettings -> value (DEVICEWIDGET_BUTTON + "_color",
	                                              "white"). toString ();
QString devicewidgetButton_font =
	   dabSettings -> value (DEVICEWIDGET_BUTTON + "_font",
	                                              "black"). toString ();

QString portSelector_font	=
	   dabSettings -> value (PORT_SELECTOR + "_font",
	                                              "white"). toString ();
QString	portSelector_color =
	   dabSettings -> value (PORT_SELECTOR + "_color",
	                                              "black"). toString ();

QString	dlTextButton_color =
	   dabSettings -> value (DLTEXT_BUTTON + "_color",
	                                              "black"). toString ();

QString dlTextButton_font	=
	   dabSettings -> value (DLTEXT_BUTTON + "_font",
	                                              "white"). toString ();

QString resetButton_color =
	   dabSettings -> value (RESET_BUTTON + "_color",
	                                              "white"). toString ();
QString resetButton_font =
	   dabSettings -> value (RESET_BUTTON + "_font",
	                                              "black"). toString ();

QString	scheduleButton_color =
	   dabSettings -> value (SCHEDULE_BUTTON + "_color",
	                                              "black"). toString ();
QString scheduleButton_font	=
	   dabSettings -> value (SCHEDULE_BUTTON + "_font",
	                                              "white"). toString ();

QString snrButton_color =
	   dabSettings -> value (SNR_BUTTON + "_color",
	                                              "white"). toString ();
QString snrButton_font =
	   dabSettings -> value (SNR_BUTTON + "_font",
	                                              "black"). toString ();

QString	set_coordinatesButton_color =
	   dabSettings -> value (SET_COORDINATES_BUTTON + "_color",
	                                              "white"). toString ();
QString set_coordinatesButton_font	=
	   dabSettings -> value (SET_COORDINATES_BUTTON + "_font",
	                                              "black"). toString ();
	
QString	loadTableButton_color =
	   dabSettings -> value (LOAD_TABLE_BUTTON + "_color",
	                                              "white"). toString ();
QString loadTableButton_font	=
	   dabSettings -> value (LOAD_TABLE_BUTTON + "_font",
	                                              "black"). toString ();

QString dumpButton_color =
	   dabSettings -> value (DUMP_BUTTON + "_color",
	                                              "white"). toString ();
QString dumpButton_font =
	   dabSettings -> value (DUMP_BUTTON + "_font",
	                                              "black"). toString ();


QString skinButton_font	=
	   dabSettings -> value (SKIN_BUTTON + "_font",
	                                              "white"). toString ();
QString	skinButton_color =
	   dabSettings -> value (SKIN_BUTTON + "_color",
	                                              "black"). toString ();
	dabSettings	-> endGroup ();

	QString temp = "QPushButton {background-color: %1; color: %2}";

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
	this -> skinButton ->
	              setStyleSheet (temp. arg (skinButton_color,
	                                        skinButton_font));
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

	dabSettings	-> beginGroup (COLOR_SETTINGS);
	dabSettings	-> setValue (buttonColor, baseColor. name ());
	dabSettings	-> setValue (buttonFont, textColor. name ());
	dabSettings	-> endGroup ();
}

void	configHandler::handle_muteTimeSetting	(int newV) {
	setConfig (MUTE_TIME_SETTING, newV);
}

void	configHandler::handle_switchDelaySetting	(int newV) {
	setConfig (SWITCH_VALUE_SETTING, newV);
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
QString oldPort	= dabSettings -> value (MAP_PORT_SETTING, "8080"). toString ();
mapPortHandler theHandler (oldPort);
	int portNumber = theHandler. QDialog::exec ();
	if (portNumber != 0)
	   setConfig (MAP_PORT_SETTING, QString::number(portNumber));
}

void	configHandler::handle_skinSelector     () {
skinHandler theSkins;
	int skinIndex = theSkins. QDialog::exec ();
	QString skinName = theSkins. skins. at (skinIndex);
	setConfig (SKIN_SETTING, skinName); 
}

void	configHandler::handle_onTop	(int d) {
bool onTop = false;
	(void)d;
	if (this ->  onTop -> isChecked ())
	   onTop = true;
	setConfig (ON_TOP_SETTING, onTop ? 1 : 0);
}

void	configHandler::handle_epgSelector	(int x) {
	(void)x;
	setConfig (EPG_FLAG_SETTING, 
	                         this ->  epg_selector -> isChecked () ? 1 : 0);
}

void	configHandler::handle_localBrowser	(int d) {
	(void)d;
	setConfig (LOCAL_BROWSER_SETTING, 
	               this ->  localBrowserSelector -> isChecked () ? 1 : 0);
}

void	configHandler::handle_localTransmitterSelector (int c) {
	(void)c;
	setConfig (LOCAL_TRANSMITTERS_SETTING, 
	               this ->  localTransmitterSelector -> isChecked () ? 1 : 0);
	set_transmitters_local (
	               this -> localTransmitterSelector -> isChecked ());
}

void	configHandler::handle_clearScan_Selector (int c) {
	(void)c;
	setConfig (CLEAR_SCAN_RESULT_SETTING,
	               this ->  clearScan_selector -> isChecked () ? 1 : 0);
}

void	configHandler::handle_saveSlides	(int x) {
	(void)x;
	setConfig (SAVE_SLIDES_SETTING, 
	                         this ->  saveSlides -> isChecked () ? 1 : 0);
}

void	configHandler::handle_decoderSelector	(const QString &s) {
int	decoder	= 0100;
	for (int i = 0; decoders [i]. decoderName != ""; i ++)
	   if (decoders [i]. decoderName == s) {
	      decoder = decoders [i]. decoderKey;
	      selectDecoder (decoder);
	   }
}

void	configHandler::handle_saveTransmittersSelector	(int d) {
	(void)d;
	int transmitterNames =
	             saveTransmittersSelector -> isChecked () ? 1 : 0;
        setConfig (TRANSMITTER_NAMES_SETTING, transmitterNames);
}

void	configHandler::handle_tii_detectorMode (int d) {
	(void)d;
	set_tii_detectorMode (new_tiiMode_selector -> isChecked () );
 	setConfig (TII_DETECTOR_SETTING,
	               new_tiiMode_selector -> isChecked () ? 1 : 0);
}

void	configHandler::handle_utc_selector	(int d) {
	(void)d;
	setConfig (UTC_SELECTOR_SETTING, 
	               utc_selector -> isChecked () ? 1 : 0);
}

int	configHandler::get_serviceOrder	() {
	return serviceOrder;
}

bool	configHandler::tii_detector_active	() {
	return new_tiiMode_selector -> isChecked ();
}

bool	configHandler::closeDirect_active	() {
	return closeDirect_selector -> isChecked ();
}

bool	configHandler::clearScan_Selector_active () {
	return clearScan_selector	-> isChecked ();
}

//
//column 2
bool	configHandler::logger_active	() {
	return logger_selector	-> isChecked ();
}

bool	configHandler::utcSelector_active	() {
	return utc_selector -> isChecked ();
}

bool	configHandler::epg_automatic_active	() {
	return epg_selector	-> isChecked ();
}

bool	configHandler::eti_active	() {
	return etiActivated_selector -> isChecked ();
}

bool	configHandler::saveSliders_active	() {
	return saveSlides -> isChecked ();
}

//
//	Column 3
bool	configHandler::epg2_active	() {
	return epg2xmlSelector -> isChecked ();
}

bool	configHandler::onTop_active	() {
	return onTop	-> isChecked ();
}

bool	configHandler::localBrowserSelector_active	() {
	return localBrowserSelector -> isChecked ();
}

bool	configHandler::localTransmitterSelector_active	() {
	return localTransmitterSelector -> isChecked ();
}

bool	configHandler::transmitterNames_active	() {
	return saveTransmittersSelector	-> isChecked ();
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
	connect (streamoutSelector, SIGNAL (activated (int)),
	         myRadioInterface, SLOT (set_streamSelector (int)));
}

QString	configHandler::currentStream		() {
	if (streamoutSelector	-> count () == 0)
	   return "";
	return streamoutSelector	-> currentText ();
}

int	configHandler::switchDelayValue		() {
	return switchDelaySetting	-> value () * 1000;
}

int	configHandler::muteValue		() {
	return  muteTimeSetting -> value ();
}

void	configHandler::showLoad		(float load) {
	loadDisplay	-> display (load);
}

void	configHandler::setConfig (const QString &s, int d) {
	dabSettings	-> setValue (s, d);
}

void	configHandler::setConfig (const QString &s, const QString &d) {
	dabSettings	-> setValue (s, d);
}

void	configHandler::handle_dcRemovalSelector	(int d) {
	(void)d;
	set_dcRemoval (dcRemovalSelector -> isChecked ());
}

void	configHandler::enable_loadLib	() {
	loadTableButton	-> setEnabled (true);
}
