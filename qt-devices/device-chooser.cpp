#
/*
 *    Copyright (C) 2014 .. 2023
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
 *    along with Qt-DAB-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"device-chooser.h"
#include	<QFileDialog>
#include	<QMessageBox>
#include	"device-exceptions.h"

#ifdef	HAVE_RTLSDR_V3
#include	"rtlsdr-handler-v3.h"
#define	RTLSDR_DEVICE_V3	0200
#endif
#ifdef	HAVE_RTLSDR_V4
#include	"rtlsdr-handler-v4.h"
#define	RTLSDR_DEVICE_V4	0201
#endif
#ifdef	HAVE_RTLSDR
#include	"rtlsdr-handler.h"
#define	RTLSDR_DEVICE		0222
#endif
#ifdef	HAVE_SDRPLAY_V2
#include	"sdrplay-handler-v2.h"
#define	SDRPLAY_V2_DEVICE	0202
#endif
#ifdef	HAVE_SDRPLAY_V3
#include	"sdrplay-handler-v3.h"
#define	SDRPLAY_V3_DEVICE	0203
#endif
#ifdef	HAVE_AIRSPY
#include	"airspy-handler.h"
#define	AIRSPY_DEVICE		0204
#elif	HAVE_AIRSPY_2
#include	"airspy-2.h"
#define	AIRSPY_DEVICE		0204
#endif
#ifdef	HAVE_HACKRF
#include	"hackrf-handler.h"
#define	HACKRF_DEVICE		0205
#endif
#ifdef	HAVE_LIME
#include	"lime-handler.h"
#define	LIME_DEVICE		0206
#endif
#ifdef	HAVE_PLUTO
#include	"pluto-handler.h"
#define	PLUTO_DEVICE		0207
#endif
#ifdef	HAVE_SOAPY
#include	"soapy-handler.h"
#define	SOAPY_DEVICE		0210
#endif
#ifdef	HAVE_UHD
#include	"uhd-handler.h"
#define	USRP_DEVICE		0211
#endif
#ifdef	__MINGW32__
#ifdef	HAVE_EXTIO
#include	"extio-handler.h"
#define	EXTIO_DEVICE		0212
#endif
#endif
#ifdef	HAVE_RTL_TCP
#include	"rtl_tcp_client.h"
#define	RTL_TCP_DEVICE		0213
#endif
#ifdef	HAVE_SPYSERVER_16
#include	"spyserver-client.h"
#define	SPYSERVER_DEVICE_16	0214
#endif
#ifdef	HAVE_SPYSERVER_8
#include	"spyserver-client-8.h"
#define	SPYSERVER_DEVICE_8	0215
#endif
#ifdef	HAVE_COLIBRI
#include	"colibri-handler.h"
#define	COLIBRI_DEVICE		0216
#endif
#ifdef	HAVE_ELAD
#include	"elad-handler.h"
#define	ELAD_S1_DEVICE		0217
#endif

#include	"rawfiles.h"
#include	"wavfiles.h"
#include	"xml-filereader.h"
//
//	Some devices are always configured
//
#define	NO_ENTRY	0100
#define	FILE_INPUT	0177
#define	RAW_FILE_RAW	0101
#define	RAW_FILE_IQ	0102
#define	WAV_FILE	0103
#define	XML_FILE	0104

	deviceChooser::deviceChooser (QSettings *dabSettings) {
	this	-> dabSettings	= dabSettings;
	deviceList. push_back (deviceItem ("select input", NO_ENTRY));
	deviceList. push_back (deviceItem ("file input", FILE_INPUT));
#ifdef	HAVE_SDRPLAY_V3
	deviceList. push_back (deviceItem ("sdrplay", SDRPLAY_V3_DEVICE));
#endif
#ifdef	HAVE_SDRPLAY_V2
	deviceList. push_back (deviceItem ("sdrplay-v2", SDRPLAY_V2_DEVICE));
#endif
#ifdef	__MINGW32__
#ifdef	HAVE_RTLSDR_V3
	deviceList. push_back (deviceItem ("dabstick-v3", RTLSDR_DEVICE_V3));
#elif	HAVE_RTLSDR_V4
	deviceList. push_back (deviceItem ("dabstick-v4", RTLSDR_DEVICE_V4));
#endif
#else
#ifdef	HAVE_RTLSDR
	deviceList. push_back (deviceItem ("dabstick", RTLSDR_DEVICE));
#endif
#endif
#ifdef	HAVE_AIRSPY
	deviceList. push_back (deviceItem ("airspy", AIRSPY_DEVICE));
#elif	HAVE_AIRSPY_2
	deviceList. push_back (deviceItem ("airspy-2", AIRSPY_DEVICE));
#endif
#ifdef	HAVE_HACKRF
	deviceList. push_back (deviceItem ("hackrf", HACKRF_DEVICE));
#endif
#ifdef	HAVE_LIME
	deviceList. push_back (deviceItem ("limeSDR", LIME_DEVICE));
#endif
#ifdef	HAVE_PLUTO
	deviceList. push_back (deviceItem ("pluto", PLUTO_DEVICE));
#endif
#ifdef	HAVE_RTL_TCP
	deviceList. push_back (deviceItem ("rtl_tcp", RTL_TCP_DEVICE));
#endif
#ifdef	HAVE_SOAPY
	deviceList. push_back (deviceItem ("soapy", SOAPY_DEVICE));
#endif
#ifdef  HAVE_EXTIO
	deviceList. push_back (deviceItem ("extio", EXTIO_DEVICE));
#endif
#ifdef	HAVE_UHD
	deviceList. push_back (deviceItem ("uhd", USRP_DEVICE));
#endif
#ifdef	HAVE_SPYSERVER_16
	deviceList. push_back (deviceItem ("spyServer-16",
	                                           SPYSERVER_DEVICE_16));
#endif
#ifdef	HAVE_SPYSERVER_8
	deviceList. push_back (deviceItem ("spyServer-8", SPYSERVER_DEVICE_8));
#endif
}

		deviceChooser::~deviceChooser	() {}

QStringList	deviceChooser::getDeviceList () {
QStringList res;
	for (auto &s: deviceList)
	   res << s. deviceName;
	return res;
}

int	deviceChooser::getDeviceIndex	(const QString &name) {
	for (auto &s: deviceList) 
	   if (s. deviceName == name)
	      return s. deviceNumber;
	return -1;
}

deviceHandler	*deviceChooser::createDevice (const QString &deviceName,
	                                      const QString &version) {
deviceHandler	*inputDevice;
	try {
	   inputDevice = _createDevice (deviceName, version);
	} catch (std::exception &e) {
	   QMessageBox:: warning (nullptr, "Warning", e. what ());
	   return nullptr;
	}
	catch (...) {
	   QMessageBox::warning (nullptr, "Warning", "unkknown excpetion");
	   return nullptr;
	}
	return inputDevice;
}

deviceHandler	*deviceChooser::_createDevice (const QString &s,
	                                         const QString &version) {
deviceHandler	*inputDevice_p	= nullptr;
int	deviceNumber	= getDeviceIndex (s);

	if (deviceNumber == 0)
	   return nullptr;

	switch (deviceNumber) {
#ifdef	HAVE_SDRPLAY_V2
	   case SDRPLAY_V2_DEVICE:
	      return new sdrplayHandler_v2 (dabSettings, version);
	      break;
#endif
#ifdef	HAVE_SDRPLAY_V3
	   case SDRPLAY_V3_DEVICE:
	      return new sdrplayHandler_v3 (dabSettings, version);
	      break;
#endif
#ifdef	HAVE_RTLSDR_V3
	   case RTLSDR_DEVICE_V3:
	      return new rtlsdrHandler_v3 (dabSettings, version);
	      break;
#endif
#ifdef	HAVE_RTLSDR_V4
	   case RTLSDR_DEVICE_V4:
	      return new rtlsdrHandler_v4 (dabSettings, version);
	      break;
#endif
#ifdef	HAVE_RTLSDR
	   case RTLSDR_DEVICE:
	      return new rtlsdrHandler (dabSettings, version);
	      break;
#endif
#ifdef	HAVE_AIRSPY
	   case AIRSPY_DEVICE:
	      return new airspyHandler (dabSettings, version);
	      break;
#elif 	HAVE_AIRSPY_2
	   case AIRSPY_DEVICE:
	      return new airspy_2 (dabSettings, version);
	      break;
#endif
#ifdef	HAVE_HACKRF
	   case HACKRF_DEVICE:
	      return new hackrfHandler (dabSettings, version);
#endif
#ifdef	HAVE_LIME
	   case LIME_DEVICE:
	      return new limeHandler (dabSettings, version);
	      break;
#endif
#ifdef	HAVE_PLUTO
	   case PLUTO_DEVICE:
	      return new plutoHandler (dabSettings, version);
#endif
#ifdef HAVE_RTL_TCP
	   case RTL_TCP_DEVICE:
	      return new rtl_tcp_client (dabSettings);
#endif
#ifdef HAVE_EXTIO
	   case EXTIO_DEVICE:
	      return new extioHandler (dabSettings);
	      break;
#endif
#ifdef	HAVE_SOAPY
	   case SOAPY_DEVICE:
	      return new soapyHandler (dabSettings);
	      break;
#endif
#ifdef HAVE_UHD
	   case USRP_DEVICE:
	      return new uhdHandler (dabSettings);
	      break;
#endif
#ifdef HAVE_SPYSERVER_16
	   case SPYSERVER_DEVICE_16:
	      return new spyServer_client (dabSettings);
	      break;
#endif
#ifdef HAVE_SPYSERVER_8
	   case SPYSERVER_DEVICE_8:
	      return new spyServer_client_8 (dabSettings);
	      break;
#endif
	   case FILE_INPUT:
	   {  uint8_t fileType = 0;
	      QString fileName	= getFileName (fileType);
	      if (fileName == "")
	         throw (device_exception ("no file"));
	      switch (fileType) {
	         case RAW_FILE_RAW:
	         case RAW_FILE_IQ:
	            return new rawFiles (fileName);
	         case WAV_FILE:
	            return new wavFiles (fileName);
	         case XML_FILE:
	            return new rawFiles (fileName);
	         default:
	            throw (device_exception ("no file"));
	      }
	   }
	   break;

	   default:
	      throw (device_exception ("unknown device selected"));
	      break;
	}
	return nullptr;
}

QString	 deviceChooser::getFileName	(uint8_t &fileType) {
QString selected_type;
QString TYPE_XML	= "uff-xml (*.uff)";
QString TYPE_SDR	= "sdr-wav (*.sdr)";
QString TYPE_RAW	= "raw (*.raw)";
QString TYPE_IQ		= "IQ-RAW (*.iq)";

QString fileName =
	       QFileDialog::getOpenFileName (nullptr,
                                             "Open file ...",
	                                     QDir::homePath (),
                                             TYPE_XML + ";;" +
	                                     TYPE_SDR + ";;" +
	                                     TYPE_RAW + ";;" +
	                                     TYPE_IQ,
                                             &selected_type,
	                                     QFileDialog::DontUseNativeDialog);
	
	if (fileName == "")
	   return "";		// note typeFound unset

	if (selected_type == TYPE_XML) 
	  fileType	= XML_FILE;
	else
	if (selected_type == TYPE_SDR)
	   fileType	= WAV_FILE;
	else
	if (selected_type == TYPE_RAW)
	   fileType	= RAW_FILE_RAW;
	else
	if (selected_type == TYPE_IQ)
	   fileType	= RAW_FILE_IQ;
	else
	   fileType	= 0;

	return fileName;
}

