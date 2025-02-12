#
/*
 *    Copyright (C) 2013 .. 2023
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

#pragma once

#include	"ui_config-helper.h"
#include	<QWidget>
#include	<QString>
#include	<QStringList>
#include	<QPushButton>
#include	"super-frame.h"
class		RadioInterface;
class		QSettings;
//
//	In this version, a separate config handler is introduced
//	that - as the name suggests - controls the configWidget

class	configHandler: public QObject, public Ui_configWidget {
Q_OBJECT
public:
		configHandler		(RadioInterface *parent,
	                                 QSettings *settings);
		~configHandler		();

	void	show			();
	void	hide			();
	bool	isHidden		();
	void	set_connections		();
//
//	the value extractors
	bool	upload_selector_active	();
	bool	upload_active		();
	int	get_serviceOrder	();
	int	switchDelayValue	();
	int	muteValue		();
	
	bool	closeDirect_active	();
	bool	utcSelector_active	();
	bool	epg_automatic_active	();
	bool	eti_active		();
	bool	saveSliders_active	();
	bool	epg2_active		();
	bool	onTop_active		();
	bool	logger_active		();
	bool	clearScan_Selector_active	();
	bool	localBrowserSelector_active	();
//
//	a few others
	void	showLoad		(float);
	void	setDeviceList		(const QStringList &);
	bool	findDevice		(const QString &);
	void	connectDevices		();
	void	disconnectDevices	();
	void	reconnectDevices	();
	void	mark_dumpButton		(bool);
	void	mark_dlTextButton	(bool);
	void	set_closeDirect		(bool);
	void	show_streamSelector	(bool);
	void	fill_streamTable	(const QStringList &);
	int	init_streamTable	(const QString &);
	void	connect_streamTable	();
	QString	currentStream		();

	void	enable_loadLib		();
	bool	get_correlationSelector	();
	bool	get_audioServices_only	();

private:
	RadioInterface	*myRadioInterface;
	QSettings	*dabSettings;
	superFrame	myFrame;
	int		serviceOrder;
	void		set_Colors		();
	void		set_buttonColors	(QPushButton *b,
	                                         const QString &buttonName);
private slots:
//	
//	first the color setters
	void	color_audioSelectButton	();
	void	color_fontButton	();
	void	color_fontColorButton	();
	void	color_devicewidgetButton	();
	void	color_portSelector	();
	void	color_dlTextButton	();
	void	color_resetButton	();
	void	color_scheduleButton	();
	void	color_snrButton		();
	void	color_set_coordinatesButton	();
	void	color_loadTableButton	();
	void	color_sourcedumpButton	();
	void	color_skinButton	();
	void	color_pathButton	();
//
//	and the handlers:
	void	handle_audioSelectButton	();
	void	handle_upload_selector		(int);
	void	handle_muteTimeSetting		(int);
	void	handle_switchDelaySetting	(int);
	void	handle_orderAlfabetical		();
	void	handle_orderServiceIds		();
	void	handle_ordersubChannelIds	();
	void	handle_portSelector		();
	void	handle_skinSelector		();
	void	handle_onTop			(int);
	void	handle_epgSelector		(int);
	void	handle_utc_selector		(int);
	void	handle_localBrowser		(int);
	void	handle_clearScan_Selector	(int);
	void	handle_saveSlides		(int);
	void	handle_decoderSelector		(const QString &s);
	void	handle_tiiThreshold		(int);
	void	handle_pathButton		();
	void	handle_audioServices_only	(int);
	void	handle_auto_http		(int);

	void	handle_tiiCollisions		(int);
	void	handle_tiiFilter		(int);

	void	handle_tiiSelector		(int);

	void	handle_mouseClicked		();
signals:
	void	selectDecoder		(int);
	void	set_transmitters_local	(bool);
	void	set_transmitterNames	(int);
	void	set_tii_detectorMode	(bool);

	void	handle_fontSelect	();
	void	handle_fontColorSelect	();
	void	handle_fontSizeSelect	(int);

	void	set_serviceOrder	(int);
	void	set_dcRemoval		(bool);
	void	frameClosed		();

	void	process_tiiCollisions	(int);
	void	process_tiiFilter	(bool);

	void	process_tiiSelector	(bool);

};

