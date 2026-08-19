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
#include	<QMessageBox>
#include	<QStringList>
#include	<QStringListModel>
#include	<QColorDialog>
#include	<QDir>
#include	"dab-constants.h"
#include	"config-handler.h"
#include	"radio.h"
#include	"position-handler.h"
#include	"settingNames.h"
#include	"settings-handler.h"

#define FONT_BUTTON             QString ("fontButton")
#define FONTCOLOR_BUTTON        QString ("fontColorButton")


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

        int fontSize    =  
	         value_i (dabSettings, COLOR_SETTINGS, "fontSize", 10);
	this	-> fontSizeSelector -> setValue (fontSize);
//
//	Topline
	storageLabel	-> setStyleSheet ("QLabel {color: yellow}");
	QString path_for_files  = theFilenameFinder. basicPath ();
        path_for_files  = value_s (dabSettings, DAB_GENERAL,
                                                   BASIC_PATH, path_for_files);
	pathLabel	-> setText (path_for_files);

	fontLabel	-> setStyleSheet ("QLabel {color: yellow}");
	connect (fontButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_fontButton);
	connect (fontColorButton, &smallPushButton::rightClicked,
	         this, &configHandler::color_fontColorButton );
	connect (fontButton, &QPushButton::clicked,
	         this,  &configHandler::handle_fontSelect);
	connect (fontColorButton, &QPushButton::clicked,
	         this, &configHandler::handle_fontColorSelect);
	connect (fontSizeSelector, qOverload<int>(&QSpinBox::valueChanged),
	         this, &configHandler::handle_fontSizeSelect);
/////////////////////////////////////////////////////////////////////////////
//	selector for left frame labeled "various"
	variousLabel	-> setStyleSheet ("QLabel {color: yellow}");
//
//	upload selector
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

//	save slides

	int saveSlides	= value_i (dabSettings, CONFIG_HANDLER,
	                           SAVE_SLIDES_SETTING, 1);
	saveSlidesSelector	-> setChecked (saveSlides != 0);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
           connect (saveSlidesSelector, &QCheckBox::checkStateChanged,
#else   
           connect (saveSlidesSelector, &QCheckBox::stateChanged,
#endif                                
                    this, &configHandler::handle_saveSlides);


//	dl Text selector is not set on start up
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (dlTextSelector, &QCheckBox::checkStateChanged,
#else
	connect (dlTextSelector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_dlTextSelector);
	connect (this, &configHandler::handle_dlText,
	         myRadioInterface, &RadioInterface::handle_dlText);

//	save title selector is not set on startup
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (saveTitlesSelector, &QCheckBox::checkStateChanged,
#else
	connect (saveTitlesSelector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_saveTitles);
//
//	check updated
	bool b = value_i (dabSettings, CONFIG_HANDLER, DO_UPDATECHECK, 0) != 0;
	this	-> updateChecker	-> setChecked (b);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (updateChecker, &QCheckBox::checkStateChanged,
#else
	connect (updateChecker, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_updateChecker);

//	on top selector
	b =  value_i (dabSettings, CONFIG_HANDLER, ON_TOP_SETTING, 0) != 0;;
	this ->  onTop -> setChecked (b);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (onTop, &QCheckBox::checkStateChanged,
#else
	connect (onTop, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_onTop);
//
//	close direct
	b = value_i (dabSettings, CONFIG_HANDLER,
	                          CLOSE_DIRECT_SETTING, 0) != 0;
	this -> closeDirect_selector -> setChecked (b);
//
//	for close Direct we just poll

//	dumpmode
	b = value_i (dabSettings, CONFIG_HANDLER, DUMPMODE_SET, 1) != 0;
	this	-> dumpmodeSelector	-> setChecked (b);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (dumpmodeSelector, &QCheckBox::checkStateChanged,
#else
	connect (dumpmodeSelector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_dumpmodeSelector);
//
//	end of left frame
//////////////////////////////////////////////////////////////////////////
//	middle frame labeled "tii and db selection
//	
	float latitude	=
	              value_f (dabSettings, MAP_HANDLING,
	                                        HOME_LATITUDE, 52.22f);
	this	-> latitudeSelector	-> setValue (latitude);

	connect (latitudeSelector,
	            qOverload<double>(&QDoubleSpinBox::valueChanged),
	         myRadioInterface, &RadioInterface::set_latitude);

	float longitude =
	              value_f (dabSettings, MAP_HANDLING,
	                                        HOME_LONGITUDE, 4.54f);
	this	-> longitudeSelector	-> setValue (longitude);

	connect (longitudeSelector,
	            qOverload<double>(&QDoubleSpinBox::valueChanged),
	         myRadioInterface, &RadioInterface::set_longitude);

//	tiiCollisions -> setEnabled (false);
	tiiThreshold_setter -> setMinimum (6);

	int v = value_i (dabSettings, CONFIG_HANDLER,
	                             TII_THRESHOLD, 12);
	this -> tiiThreshold_setter -> setValue (v);
	connect (tiiThreshold_setter, qOverload<int>(&QSpinBox::valueChanged),
	         myRadioInterface, &RadioInterface::handle_tiiThreshold);

	connect (tiiCollisions, qOverload<int>(&QSpinBox::valueChanged),
	         this, &configHandler::handle_tiiCollisions);


	int d	= value_i (dabSettings, CONFIG_HANDLER,
	                                 SHOWALL_TII, 1);
	this	-> allTIISelector	-> setChecked (d != 0);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (allTIISelector, &QCheckBox::checkStateChanged,
#else
	connect (allTIISelector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_allTIISelector);
//
//////////////////////////////////////////////////////////////////////////////
//
//	Third frame, no label, CPU load on top

	cpuLabel	-> setStyleSheet ("QLabel {color: yellow}");

	TPEG_label	-> setStyleSheet ("color:yellow");
	int	tpegPort	=
	               value_i (dabSettings, MAP_HANDLING,
	                                         TPEG_PORT, 8888);
	tpegPortSelector	-> setValue (tpegPort);

	connect (tpegPortSelector, qOverload<int>(&QSpinBox::valueChanged),
	         myRadioInterface, &RadioInterface::set_tpegPort);

	int audioDriver		=
	           value_i (dabSettings, SOUND_HANDLING, S_QT_AUDIO, 1);
	audioHandler	-> setChecked (audioDriver != 0);

	connect (audioHandler,
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	             &QCheckBox::checkStateChanged,
#else
		     &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_audioHandler);

	uint32_t http	=
	              value_i (dabSettings, MAP_HANDLING, HTTP_PORT, 8080);
	httpPortSelector	-> setValue (http);
	connect (httpPortSelector, qOverload<int>(&QSpinBox::valueChanged),
	         myRadioInterface, &RadioInterface::handle_httpPort);

	b = value_i (dabSettings, CONFIG_HANDLER,
	                           LOCAL_BROWSER_SETTING, 1) != 0;
	this -> localBrowserSelector -> setChecked (b);
	connect (localBrowserSelector,
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	             &QCheckBox::checkStateChanged,
#else
		     &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_localBrowser);

	b = value_i (dabSettings, CONFIG_HANDLER, AUTO_HTTP, 0) != 0;
	this	-> auto_http -> setChecked (b);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (auto_http, &QCheckBox::checkStateChanged,
#else
	connect (auto_http, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_auto_http);

	maphandlingLabel	-> setStyleSheet ("QLabel {color: yellow}");
	b	= value_i (dabSettings, CONFIG_HANDLER, SAVE_HTTP, 0) != 0;
	mapViewSelector		-> setChecked (b);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (mapViewSelector, &QCheckBox::checkStateChanged,
#else
	connect (mapViewSelector, &QCheckBox::stateChanged,
#endif
	this, &configHandler::handle_mapViewSelector);
//
///////////////////////////////////////////////////////////////////////////////
//
//	fourth frame labeled "decoding and services"
//	active services
	nrServicesLabel	-> setStyleSheet ("QLabel {color: yellow}");
	activeServices	-> setText (QString::number (0));
	connect (activeServices, &clickablelabel::clicked_left,
	         myRadioInterface, &RadioInterface::handle_activeServices);

	int x = value_i ( dabSettings, CONFIG_HANDLER, SERVICE_ORDER_SETTING,
	                                                       ALPHA_BASED);
	if (x == ALPHA_BASED)
	   this -> orderAlfabetical -> setChecked (true);
	else
	if (x == ID_BASED)
	   this -> orderServiceIds -> setChecked (true);
	else
	   this -> ordersubChannelIds -> setChecked (true);
	serviceOrder	= x;
	connect (orderAlfabetical, &QRadioButton::clicked,
	         this, &configHandler::handle_orderAlfabetical);
	connect (orderServiceIds, &QRadioButton::clicked,
	         this, &configHandler::handle_orderServiceIds);
	connect (ordersubChannelIds, &QRadioButton::clicked,
	         this, &configHandler::handle_ordersubChannelIds);
//
	decoderLabel	-> setStyleSheet ("QLabel {color: yellow}");
	for (int i = 0; !decoders [i]. decoderName. isEmpty (); i ++) 
	  this ->  decoderSelector -> addItem (decoders [i]. decoderName);

	int k	= value_i (dabSettings, CONFIG_HANDLER,
	                                 DECODERS, DECODER_1);
	decoderSelector	-> setCurrentIndex (index_for_key (k));
	connect (decoderSelector,
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 2)
	         &QComboBox::textActivated,
#else
	         qOverload<const QString &>(&QComboBox::activated),
#endif
	         this, &configHandler::handle_decoderSelector);
	connect (this, &configHandler::selectDecoder,
	         myRadioInterface, &RadioInterface::selectDecoder);

	b =  value_i (dabSettings, CONFIG_HANDLER,
	                           S_CORRELATION_ORDER, 0) != 0;
	this	-> correlationSelector -> setChecked (b);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (correlationSelector, &QCheckBox::checkStateChanged,
#else
	connect (correlationSelector, &QCheckBox::stateChanged,
#endif
	         myRadioInterface, &RadioInterface::handle_correlationSelector);


	b =  value_i (dabSettings, CONFIG_HANDLER, DC_REMOVAL, 0) != 0;
	this -> dcRemovalSelector -> setChecked (b);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (dcRemovalSelector, &QCheckBox::checkStateChanged,
#else
	connect (dcRemovalSelector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_dcRemovalSelector);

	connect (this, &configHandler::set_dcRemoval,
	         myRadioInterface, &RadioInterface::set_dcRemoval);

	b = value_i (dabSettings, CONFIG_HANDLER, AUDIOSERVICES_ONLY, 1);
	this	-> audioServices_only -> setChecked (b);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (audioServices_only, &QCheckBox::checkStateChanged,
#else
	connect (audioServices_only, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_audioServices_only);

	b = value_i (dabSettings, CONFIG_HANDLER,
	                          LOAD_SELECTION, 0) == 1;
	this -> loadSelection_selector -> setChecked (b);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	connect (loadSelection_selector, &QCheckBox::checkStateChanged,
#else
	connect (loadSelection_selector, &QCheckBox::stateChanged,
#endif
	         this, &configHandler::handle_loadSelection_selector);

	connect (switchDelaySetting, qOverload<int>(&QSpinBox::valueChanged),
	         this, &configHandler::handle_switchDelaySetting);

	x = value_i (dabSettings, CONFIG_HANDLER, SWITCH_VALUE_SETTING,
	                               DEFAULT_SWITCHVALUE);
	this -> switchDelaySetting -> setValue (x);
//
/////////////////////////////////////////////////////////////////////////////
	tiiLabel	-> setStyleSheet ("QLabel {color: yellow}");
	decodingLabel	-> setStyleSheet ("QLabel {color: yellow}");
//
//	Experimental
//	loadSelection_selector ->
//	        setStyleSheet ("QCheckBox::indicator {"
//	                     "width:   10px;"
//	                      "height: 10px;"
//	                      "background-color: white" "}");
//	loadSelection_selector ->
//	        setStyleSheet ("QCheckBox::indicator:checked {"
//	                     "width:   14px;"
//	                      "height: 14px;"
//	                      "background-color: blue;" "}");
//	loadSelection_selector ->
//	        setStyleSheet ("QCheckBox::indicator:pressed {"
//	                     "width:   14px;"
//	                      "height: 14px;"
//	                      "background-color: blue;" "}");
//	loadSelection_selector ->
//	        setStyleSheet ("QCheckBox::indicator:unchecked {"
//	                     "width: 8px;"
//	                      "height: 8px;"
//	                      "background-color: white;" "}");
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

QString temp = "QPushButton {background-color: %1; color: %2}";


	this -> fontButton ->
	              setStyleSheet (temp. arg (fontButton_color,
	                                        fontButton_font));

	this -> fontColorButton ->
	              setStyleSheet (temp. arg (fontColorButton_color,
	                                        fontColorButton_font));
}

void	configHandler::color_fontButton	() 	{
	set_buttonColors (this -> fontButton, FONT_BUTTON);
}

void	configHandler::color_fontColorButton	() 	{
	set_buttonColors (this -> fontColorButton, FONTCOLOR_BUTTON);
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

void	configHandler::handle_decoderSelector	(const QString &s) {
int	decoder	= 0100;
	for (int i = 0; !decoders [i]. decoderName. isEmpty (); i ++)
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
	store (dabSettings, CONFIG_HANDLER, LOAD_SELECTION, false);
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

void	configHandler::set_closeDirect (bool b) {
	closeDirect_selector	-> setChecked (b);
}

int	configHandler::switchDelayValue		() {
	return switchDelaySetting	-> value () * 1000;
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
        QMessageBox::warning (this, tr ("Warning"),
                                       tr ("this setting has effect of the next (warm or cold) restart"));
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

              
void	configHandler::handle_saveSlides       (int k) {
	(void)k;
        store (dabSettings, CONFIG_HANDLER, SAVE_SLIDES_SETTING, 
	                saveSlidesSelector -> isChecked ());
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
	if (b) 
	   QMessageBox::warning (this, tr ("Warning"),
                                       tr ("This setting has effect on the next (re)start"));
	store (dabSettings, CONFIG_HANDLER, LOAD_SELECTION, b);
}

void	configHandler::handle_updateChecker	(int k) {
	(void)k;
	bool b = this -> updateChecker -> isChecked ();
	store (dabSettings, CONFIG_HANDLER, DO_UPDATECHECK, b ? 1 : 0);
}

void	configHandler::handle_dumpmodeSelector	(int k) {
	(void)k;
	bool b = this -> dumpmodeSelector -> isChecked ();
	store (dabSettings, CONFIG_HANDLER, DUMPMODE_SET, b ? 1 : 0);
}

void	configHandler::handle_dlTextSelector	(int k) {
	(void)k;
	emit handle_dlText (dlTextSelector -> isChecked ());
}

void	configHandler::handle_audioHandler	(int k) {
	(void)k;
	bool b = this -> audioHandler -> isChecked ();
	store (dabSettings, SOUND_HANDLING, S_QT_AUDIO, b ? 1 : 0);
}

void	configHandler::set_audioSystem_label	(const QString &s) {
	audioSystem_label	-> setStyleSheet ("QLabel {color: yellow}");
	audioSystem_label	-> setText (s);
}


void	configHandler::set_loadTable	(bool b) {
	if (b) {	// a table was found in the user's directory
	   loadTableButton ->
	   setStyleSheet
	        ("QPushButton {background-color: lightgreen; color: black}");
	   connect (loadTableButton, &QPushButton::clicked,
	            myRadioInterface, &RadioInterface::handle_loadTable);
	   loadTableButton	-> setText ("refresh table");
	   loadTableButton	-> setEnabled (true);
	   loadTableButton	-> setToolTip ("touching this button will refresh the TII database used by the one detected in your homedirectory");
	}
	else {
	   loadTableButton ->
	   setStyleSheet
	        ("QPushButton {background-color: red; color: white}");
	   loadTableButton	-> setText ("disabled");
	   loadTableButton	-> setEnabled (false);
	   loadTableButton	-> setToolTip ("In your home directory no TII database was detected, the embedded database can not be refreshed");
	}
}
