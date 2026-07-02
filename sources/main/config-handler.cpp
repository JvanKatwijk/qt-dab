#
/*
 *    Copyright (C)  2015, 2026
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
#include	"radio.h"
#include	"position-handler.h"
#include	"settingNames.h"
#include	"settings-handler.h"
//#include	"audiosystem-selector.h"

#define FONT_BUTTON             QString ("fontButton")
#define FONTCOLOR_BUTTON        QString ("fontColorButton")

#define	DLTEXT_BUTTON		QString ("dlTextButton")
#define SCHEDULE_BUTTON         QString ("scheduleButton")

#define LOAD_TABLE_BUTTON       QString ("loadTableButton")


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
	                                     superFrame (nullptr),
	                                     theFilenameFinder (settings) {
	this	-> myRadioInterface	= parent;
	this	-> dabSettings		= settings;
	this	-> setupUi (this);
	setPositionAndSize (settings, this, CONFIG_HANDLER);
	hide ();
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

	uint32_t http	=
	              value_i (dabSettings, MAP_HANDLING, HTTP_PORT, 8080);
	httpPortSelector	-> setValue (http);
	connect (httpPortSelector, qOverload<int>(&QSpinBox::valueChanged),
	         myRadioInterface, &RadioInterface::handle_httpPort);


	float latitude	=
	              value_f (dabSettings, MAP_HANDLING,
	                                        HOME_LATITUDE, 52.22f);
	float longitude =
	              value_f (dabSettings, MAP_HANDLING,
	                                        HOME_LONGITUDE, 4.54f);
	this	-> latitudeSelector	-> setValue (latitude);
	this	-> longitudeSelector	-> setValue (longitude);

	connect (latitudeSelector,
	            qOverload<double>(&QDoubleSpinBox::valueChanged),
	         myRadioInterface, &RadioInterface::set_latitude);
	connect (longitudeSelector,
	            qOverload<double>(&QDoubleSpinBox::valueChanged),
	         myRadioInterface, &RadioInterface::set_longitude);

	TPEG_label	-> setStyleSheet ("color:yellow");
	int	tpegPort	=
	               value_i (dabSettings, MAP_HANDLING,
	                                         TPEG_PORT, 8888);
	tpegPortSelector	-> setValue (tpegPort);

	connect (tpegPortSelector, qOverload<int>(&QSpinBox::valueChanged),
	         myRadioInterface, &RadioInterface::set_tpegPort);

	QString path_for_files  = theFilenameFinder. basicPath ();
        path_for_files  = value_s (dabSettings, DAB_GENERAL,
                                                   BASIC_PATH, path_for_files);
	storageLabel	-> setStyleSheet ("QLabel {color: yellow}");
	fontLabel	-> setStyleSheet ("QLabel {color: yellow}");
	cpuLabel	-> setStyleSheet ("QLabel {color: yellow}");
	decoderLabel	-> setStyleSheet ("QLabel {color: yellow}");
	nrServicesLabel	-> setStyleSheet ("QLabel {color: yellow}");
	pathLabel	-> setText (path_for_files);

//	first row of checkboxes
	bool b = value_i (dabSettings, CONFIG_HANDLER, DUMPMODE_SET, 1) != 0;
	this	-> dumpmodeSelector	-> setChecked (b);
	
	b = value_i (dabSettings, CONFIG_HANDLER,
	                           LOCAL_BROWSER_SETTING, 1) != 0;
	this -> localBrowserSelector -> setChecked (b);
//
//	second row
	b = value_i (dabSettings, CONFIG_HANDLER, AUTO_HTTP, 0) != 0;
	this	-> auto_http -> setChecked (b);

	b = value_i (dabSettings, CONFIG_HANDLER, DO_UPDATECHECK, 0) != 0;
	this	-> updateChecker	-> setChecked (b);
//
	b = value_i (dabSettings, CONFIG_HANDLER,
	                          CLOSE_DIRECT_SETTING, 0) != 0;
	this -> closeDirect_selector -> setChecked (b);

	b	= value_i (dabSettings, CONFIG_HANDLER, SAVE_HTTP, 0) != 0;
	mapViewSelector		-> setChecked (b);
//
	b =  value_i (dabSettings, CONFIG_HANDLER, ON_TOP_SETTING, 0) != 0;;
	this ->  onTop -> setChecked (b);
//
//	fourth row of checkboxes
	b =  value_i (dabSettings, CONFIG_HANDLER,
	                           S_CORRELATION_ORDER, 0) != 0;
	this	-> correlationSelector -> setChecked (b);

	b =  value_i (dabSettings, CONFIG_HANDLER, DC_REMOVAL, 0) != 0;
	this -> dcRemovalSelector -> setChecked (b);
//
//	fifth row of checkboxes
	b = value_i (dabSettings, CONFIG_HANDLER, SHOWALL_SETTING, 1) != 0;;

	b = value_i (dabSettings, CONFIG_HANDLER, SAVE_SLIDES_SETTING, 1) != 0;
	this	-> saveSlides -> setChecked (b);

	b = value_i (dabSettings, CONFIG_HANDLER, AUDIOSERVICES_ONLY, 1);
	this	-> audioServices_only -> setChecked (b);

	b = value_i (dabSettings, CONFIG_HANDLER,
	                          "LOAD_SELECTION", 0) == 1;
	this -> loadSelection_selector -> setChecked (b);

	for (int i = 0; decoders [i]. decoderName != ""; i ++) 
	  this ->  decoderSelector -> addItem (decoders [i]. decoderName);

	int d	= value_i (dabSettings, CONFIG_HANDLER,
	                                 SHOWALL_TII, 1);
	this	-> allTIISelector	-> setChecked (d != 0);

	int k	= value_i (dabSettings, CONFIG_HANDLER,
	                                 DECODERS, DECODER_1);
	decoderSelector	-> setCurrentIndex (index_for_key (k));

//	tiiCollisions -> setEnabled (false);
	tiiThreshold_setter -> setMinimum (6);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (mapViewSelector, &QCheckBox::checkStateChanged,
#else
	connect (mapViewSelector, &QCheckBox::stateChanged,
#endif
	this, &configHandler::handle_mapViewSelector);

	int v = value_i (dabSettings, CONFIG_HANDLER,
	                             TII_THRESHOLD, 12);
	this -> tiiThreshold_setter -> setValue (v);
	connect (tiiThreshold_setter, qOverload<int>(&QSpinBox::valueChanged),
	         myRadioInterface, &RadioInterface::handle_tiiThreshold);

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
	connect (activeServices, &clickablelabel::clicked_left,
	         myRadioInterface, &RadioInterface::handle_activeServices);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (loadSelection_selector, &QCheckBox::checkStateChanged,
#else
	connect (loadSelection_selector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_loadSelection_selector);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (updateChecker, &QCheckBox::checkStateChanged,
#else
	connect (updateChecker, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_updateChecker);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (dumpmodeSelector, &QCheckBox::checkStateChanged,
#else
	connect (dumpmodeSelector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_dumpmodeSelector);
//
//	Tracer special
//	connect	(tracerButton, &QPushButton::clicked,
//	         this, &configHandler::handle_tracerButton);
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
	connect (this, &configHandler::set_dcRemoval,
	         myRadioInterface, &RadioInterface::set_dcRemoval);

	connect (fontButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_fontButton);
	connect (fontColorButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_fontColorButton );
	connect (dlTextButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_dlTextButton);
	connect (scheduleButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_scheduleButton);
	connect (loadTableButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_loadTableButton);

//
//	real handlers
	connect (scheduleButton, &QPushButton::clicked,
                 myRadioInterface, &RadioInterface::handle_scheduleButton);
	connect (muteTimeSetting, qOverload<int>(&QSpinBox::valueChanged),
	         this, &configHandler::handle_muteTimeSetting);
	connect (switchDelaySetting, qOverload<int>(&QSpinBox::valueChanged),
	         this, &configHandler::handle_switchDelaySetting);
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
	connect (dlTextButton, &QPushButton::clicked,
	         myRadioInterface, &RadioInterface::handle_dlTextButton);
	connect (loadTableButton, &QPushButton::clicked,
	         myRadioInterface, &RadioInterface::handle_loadTable);
	loadTableButton	-> setText ("refresh table");
//	however, by default loadTable is disabled
	loadTableButton	-> setEnabled (false);
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

//	third line

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (onTop, &QCheckBox::checkStateChanged,
#else
	connect (onTop, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_onTop);
//
//	fourthline

	connect (localBrowserSelector,
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	             &QCheckBox::checkStateChanged,
#else
		     &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_localBrowser);
//
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (dcRemovalSelector, &QCheckBox::checkStateChanged,
#else
	connect (dcRemovalSelector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_dcRemovalSelector);
//
//	fifh line

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (saveSlides, &QCheckBox::checkStateChanged,
#else
	connect (saveSlides, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_saveSlides);


//	sixth ine

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
	connect (this, &configHandler::selectDecoder,
	         myRadioInterface, &RadioInterface::selectDecoder);
}

/////////////////////////////////////////////////////////////////////////
//	
void	configHandler::set_Colors () {

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

QString	dlTextButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                              DLTEXT_BUTTON + "_color", YELLOW);
QString dlTextButton_font	=
	   value_s (dabSettings, COLOR_SETTINGS,
	                              DLTEXT_BUTTON + "_font", BLACK);

QString	scheduleButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                              SCHEDULE_BUTTON + "_color", YELLOW);
QString scheduleButton_font	=
	   value_s (dabSettings, COLOR_SETTINGS, 
	                              SCHEDULE_BUTTON + "_font", BLACK);

QString	loadTableButton_color =
	   value_s (dabSettings, COLOR_SETTINGS,
	                               LOAD_TABLE_BUTTON + "_color", RED);
QString loadTableButton_font	=
	   value_s (dabSettings, COLOR_SETTINGS,
	                               LOAD_TABLE_BUTTON + "_font", WHITE);

	QString temp = "QPushButton {background-color: %1; color: %2}";


	this -> fontButton ->
	              setStyleSheet (temp. arg (fontButton_color,
	                                        fontButton_font));

	this -> fontColorButton ->
	              setStyleSheet (temp. arg (fontColorButton_color,
	                                        fontColorButton_font));

	this -> dlTextButton ->
	              setStyleSheet (temp. arg (dlTextButton_color,
	                                        dlTextButton_font));


	this -> scheduleButton ->
	              setStyleSheet (temp. arg (scheduleButton_color,
	                                        scheduleButton_font));

	this -> loadTableButton ->
	              setStyleSheet (temp. arg (loadTableButton_color,
	                                        loadTableButton_font));
}

void	configHandler::color_fontButton	() 	{
	set_buttonColors (this -> fontButton, FONT_BUTTON);
}

void	configHandler::color_fontColorButton	() 	{
	set_buttonColors (this -> fontColorButton, FONTCOLOR_BUTTON);
}

void	configHandler::color_dlTextButton	()	{
	set_buttonColors (this ->  dlTextButton, DLTEXT_BUTTON);
}

void	configHandler::color_scheduleButton	() 	{
	set_buttonColors (this ->  scheduleButton, SCHEDULE_BUTTON);
}


void	configHandler::color_loadTableButton	() 	{
	set_buttonColors (this ->  loadTableButton, LOAD_TABLE_BUTTON);
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

void	configHandler::handle_onTop	(int d) {
bool onTop = false;
	(void)d;
	if (this ->  onTop -> isChecked ())
	   onTop = true;
	store (dabSettings, CONFIG_HANDLER, ON_TOP_SETTING, onTop ? 1 : 0);
}

void	configHandler::handle_localBrowser	(int d) {
	(void)d;
	store (dabSettings, CONFIG_HANDLER, LOCAL_BROWSER_SETTING, 
	               this ->  localBrowserSelector -> isChecked () ? 1 : 0);
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

int	configHandler::get_serviceOrder	() {
	return serviceOrder;
}

bool	configHandler::get_loadSelection	() {
	return loadSelection_selector	-> isChecked ();
}

void	configHandler::reset_loadSelection	() {
	if (!loadSelection_selector	-> isChecked ())
	   return;
	loadSelection_selector	-> setEnabled (false);
	loadSelection_selector	-> setChecked (false);
	loadSelection_selector	-> setEnabled (true);
	store (dabSettings, CONFIG_HANDLER, "LOAD_SELECTION", false);
}

bool	configHandler::upload_selector_active	() {
	return upload_selector -> isChecked ();
}

bool	configHandler::closeDirect_active	() {
	return closeDirect_selector -> isChecked ();
}

//
//column 2

bool	configHandler::get_dcRemovalSelector	() {
	return this -> dcRemovalSelector -> isChecked ();
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

//void	configHandler::mark_dumpButton (bool b) {
//	if (b)
//	   setButtonFont (dumpButton, "writing", 12);
//	else
//	   setButtonFont (dumpButton, "Raw dump", 10);
//}

void	configHandler::mark_dlTextButton (bool b) {
	if (b)
	   setButtonFont (dlTextButton, "writing", 12);
	else
	   setButtonFont (dlTextButton, "dlText", 10);
}

void	configHandler::set_closeDirect (bool b) {
	closeDirect_selector	-> setChecked (b);
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

void	configHandler::enable_loadLib	() {
	loadTableButton	-> setEnabled (true);
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

void	configHandler::handle_saveTitles	(int h) {
	(void)h;
}

bool	configHandler::get_saveTitles		() {
	return saveTitlesSelector -> isChecked ();
}

//
void	configHandler::handle_tracerButton	() {
//	traceOn	= !traceOn;
//	if  (traceOn)
//	   tracerButton	-> setText ("trace on");
//	else
//	   tracerButton	-> setText ("");
//	emit signal_dataTracer	(traceOn);
}

void	configHandler::handle_mapViewSelector	(int k) {
	(void)k;
	bool b = this ->  mapViewSelector -> isChecked ();
	store (dabSettings, CONFIG_HANDLER, SAVE_HTTP, b ? 1 : 0);
}

bool	configHandler::dumpmode_set	() {
	return dumpmodeSelector	-> isChecked ();
}

void    configHandler::handle_dcRemovalSelector (int k) {
        (void)k;
        bool b = this -> dcRemovalSelector -> isChecked ();
        store (dabSettings, CONFIG_HANDLER, DC_REMOVAL, b ? 1 : 0);
	emit set_dcRemoval	(b);
}

void	configHandler::handle_loadSelection_selector (int k) {
	(void)k;
	bool b = this -> loadSelection_selector -> isChecked ();
	store (dabSettings, CONFIG_HANDLER, "LOAD_SELECTION", b);
}

void	configHandler::handle_updateChecker	(int k) {
	(void)k;
	bool b = this -> updateChecker -> isChecked ();
	store (dabSettings, CONFIG_HANDLER, DO_UPDATECHECK, b ? 1 : 0);
}

void	configHandler::enable_scheduler		(bool b) {
	scheduleButton	-> setEnabled (b);
}

void	configHandler::handle_dumpmodeSelector	(int k) {
	(void)k;
	bool b = this -> dumpmodeSelector -> isChecked ();
	store (dabSettings, CONFIG_HANDLER, DUMPMODE_SET, b ? 1 : 0);
}
	
