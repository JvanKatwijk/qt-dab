#
/*
 *    Copyright (C) 2014 .. 2020
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

#ifdef	HAVE_RTLSDR
#include	"rtlsdr-handler.h"
#define	RTLSDR_DEVICE	0200
#endif
#ifdef	HAVE_SDRPLAY_V2
#include	"sdrplay-handler-v2.h"
#define	SDRPLAY_V2_DEVICE	0201
#endif
#ifdef	HAVE_SDRPLAY_V3
#include	"sdrplay-handler-v3.h"
#define	SDRPLAY_V3_DEVICE	0202
#endif
#ifdef	HAVE_AIRSPY
#include	"airspy-handler.h"
#define	AIRSPY_DEVICE		0203
#endif
#ifdef	HAVE_HACKRF
#include	"hackrf-handler.h"
#define	HACKRF_DEVICE		0204
#endif
#ifdef	HAVE_LIME
#include	"lime-handler.h"
#define	LIME_DEVICE		0205
#endif
#ifdef	HAVE_PLUTO
#include	"pluto-handler.h"
#define	PLUTO_DEVICE		0206
#endif
#ifdef	HAVE_SOAPY
#include	"soapy-handler.h"
#define	SOAPY_DEVICE		0207
#endif
#ifdef	HAVE_UHD
#include	"uhd-handler.h"
#define	USRP_DEVICE		0210
#endif
#ifdef	__MINGW32__
#ifdef	HAVE_EXTIO
#include	"extio-handler.h"
#define	EXTIO_DEVICE		0211
#endif
#endif
#ifdef	HAVE_RTL_TCP
#include	"rtl_tcp_client.h"
#define	RTL_TCP_DEVICE		0212
#endif
#ifdef	HAVE_COLIBRI
#include	"colibri-handler.h"
#define	COLIBRI_DEVICE		0213
#endif
#ifdef	HAVE_ELAD
#include	"elad-handler.h"
#define	ELAD_S1_DEVICE		0214
#endif

#include	"rawfiles.h"
#include	"wavfiles.h"
#include	"xml-filereader.h"
//
//	Some devices are always configured
//
#define	NO_ENTRY	0100
#define	RAW_FILE_RAW	0101
#define	RAW_FILE_IQ	0102
#define	WAV_FILE	0103
#define	XML_FILE	0104

	deviceChooser::deviceChooser (QSettings *dabSettings) {
	this	-> dabSettings	= dabSettings;
	deviceList. push_back (deviceItem ("select input", NO_ENTRY));
	deviceList. push_back (deviceItem ("file input(.raw)", RAW_FILE_RAW));
	deviceList. push_back (deviceItem ("file input(.iq)", RAW_FILE_IQ));
	deviceList. push_back (deviceItem ("file input(.sdr)", WAV_FILE));
	deviceList. push_back (deviceItem ("xml files", XML_FILE));

#ifdef	HAVE_SDRPLAY_V3
	deviceList. push_back (deviceItem ("sdrplay", SDRPLAY_V3_DEVICE));
#endif
#ifdef	HAVE_SDRPLAY_V2
	deviceList. push_back (deviceItem ("sdrplay-v2", SDRPLAY_V2_DEVICE));
#endif
#ifdef	HAVE_RTLSDR
	deviceList. push_back (deviceItem ("dabstick", RTLSDR_DEVICE));
#endif
#ifdef	HAVE_AIRSPY
	deviceList. push_back (deviceItem ("airspy", AIRSPY_DEVICE));
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
#ifdef	HAVE_COLIBRI
	deviceList. push_back (deviceItem ("colibri", COLIBRI_DEVICE));
#endif
#ifdef	HAVE_ELAD
	deviceList. push_back (deviceItem ("elad-s1", ELAD_S1_DEVICE));
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

deviceHandler	*deviceChooser::createDevice (const QString &s,
	                                         const QString &version) {
deviceHandler	*inputDevice_p	= nullptr;
int	deviceNumber	= getDeviceIndex (s);
	if (deviceNumber == 0)
	   return nullptr;

	switch (deviceNumber) {
#ifdef	HAVE_SDRPLAY_V2
	   case SDRPLAY_V2_DEVICE:
	      try {
	         inputDevice_p	= new sdrplayHandler_v2 (dabSettings,
	                                                       version);
	      }
	      catch (const std::exception &e) {
	         QMessageBox::warning (nullptr, "Warning", e. what ());
	         return nullptr;
	      }
	      break;
#endif
#ifdef	HAVE_SDRPLAY_V3
	   case SDRPLAY_V3_DEVICE:
	      try {
	         inputDevice_p	= new sdrplayHandler_v3 (dabSettings,
	                                                        version);
	      }
	      catch (const std::exception &e) {
	         QMessageBox::warning (nullptr, "Warning", e. what ());
	         return nullptr;
	      }
	      break;
#endif
#ifdef	HAVE_RTLSDR
	   case RTLSDR_DEVICE:
	      try {
	         inputDevice_p	= new rtlsdrHandler (dabSettings,
	                                                        version);
	      }
	      catch (const std::exception &ex) {
	         QMessageBox::warning (nullptr, "Warning", ex. what ());
	         return nullptr;
	      }
	      break;
#endif
#ifdef	HAVE_AIRSPY
	   case AIRSPY_DEVICE:
	      try {
	         inputDevice_p	= new airspyHandler (dabSettings,
	                                                        version);
	      }
	      catch (const std::exception &e) {
	         QMessageBox::warning (nullptr, "Warning", e. what ());
	         return nullptr;
	      }
	      break;
#endif
#ifdef	HAVE_HACKRF
	   case HACKRF_DEVICE:
	      try {
	         inputDevice_p	= new hackrfHandler (dabSettings,
	                                                        version);
	      }
	      catch (const std::exception &e) {
	         QMessageBox::warning (nullptr, "Warning", e. what ());
	         return nullptr;
	      }
	      break;
#endif
#ifdef	HAVE_LIME
	   case LIME_DEVICE:
	      try {
	         inputDevice_p = new limeHandler (dabSettings,
	                                                     version);
	      }
	      catch (const std::exception &e) {
	         QMessageBox::warning (nullptr, "Warning", e. what ());
	         return nullptr;
	      }
	      break;
#endif
#ifdef	HAVE_PLUTO
	   case PLUTO_DEVICE:
	      try {
	         inputDevice_p = new plutoHandler (dabSettings, version);
	      }
	      catch (const std::exception &e) {
	         QMessageBox::warning (nullptr, "Warning", e. what ());
	         return nullptr;
	      }
	      break;
#endif
#ifdef HAVE_RTL_TCP
	   case RTL_TCP_DEVICE:
	      try {
	         inputDevice_p = new rtl_tcp_client (dabSettings);
	      }
	      catch (const std::exception &e) {
	         QMessageBox::warning (nullptr, "Warning", e. what ());
	         return nullptr;
	      }
	      break;
#endif
#ifdef HAVE_EXTIO
	   case EXTIO_DEVICE:
	      try {
	         inputDevice_p = new extioHandler (dabSettings);
	      }
	      catch (const std::exception &e) {
	         QMessageBox::warning (nullptr, "warning",  e. what ());
	         return nullptr;
	      }
	      break;
#endif
#ifdef	HAVE_SOAPY
	   case SOAPY_DEVICE:
	      try {
	         inputDevice_p	= new soapyHandler (dabSettings);
	      }
	      catch (...) {
	         QMessageBox::warning (nullptr, "Warning",
	                                     "no soapy device found\n");
	         return nullptr;
	      }
	      break;
#endif
#ifdef HAVE_UHD
	   case USRP_DEVICE:
	      try {
	         inputDevice_p = new uhdHandler (dabSettings);
	      }
	      catch (const std::exception &e) {
	         QMessageBox::warning (nullptr, "Warning", e. what ());
	         return nullptr;
	      }
	      break;
#endif
#ifdef HAVE_COLIBRI
	   case COLIBRI_DEVICE:
	      try {
	         inputDevice_p = new colibriHandler (dabSettings);
	      }
	      catch (const std::exception &e) {
	         QMessageBox::warning (nullptr, "Warning", e. what ());
	         return nullptr;
	      }
	      break;
#endif
#ifdef HAVE_ELAD_S1
	   case ELAD_S1_DEVICE:
	      try {
	         inputDevice_p = new eladHandler (dabSettings);
	      }
	      catch const std::exception &e) {
	         QMessageBox::warning (nullptr, e. what ());
	         return nullptr;
	      }
	      break;
#endif
	   case XML_FILE:
	      try {
	         inputDevice_p	= new xml_fileReader ();
	      } catch (const std::exception &e) {
	         QMessageBox::warning (nullptr, "Warning", e. what ());
	         return nullptr;
	      }
	      break;

	   case RAW_FILE_RAW:
	   case RAW_FILE_IQ:
	      try {
	         inputDevice_p	= new rawFiles (deviceNumber == RAW_FILE_IQ);
	      } catch (const std::exception &e) {
	         QMessageBox::warning (nullptr, "Warning", e. what ());
	         return nullptr;
	      }
	      break;
	   
	   case WAV_FILE:
	      try {
	         inputDevice_p = new wavFiles ();
	      } catch (const std::exception &e) {
	         QMessageBox::warning (nullptr, "Warning", e. what ());
	         return nullptr;
	      }
	      break;

	   default:
	      return nullptr;
	}
	return inputDevice_p;
}

