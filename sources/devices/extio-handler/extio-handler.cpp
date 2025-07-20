#
/*
 *    Copyright (C) 2013 .. 2017
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
#
#include	<QFileDialog>
#include	<QSettings>
#include	<QMessageBox>
#include	"extio-handler.h"
#include	"device-exceptions.h"
#include	"virtual-reader.h"
#include	"common-readers.h"

#if !defined (__MINGW32__) && !defined (_WIN32)
#include	"dlfcn.h"
#endif
using namespace std;

#if defined (__MINGW32__) || defined (_WIN32)
#define	GETPROCADDRESS	GetProcAddress
#define	FREELIBRARY	FreeLibrary
#else
#define	GETPROCADDRESS	dlsym
#define	FREELIBRARY	dlclose
#endif
//

//	Simple interface routine to extio's
//	The callback from the Extio*.dll does not contain a "context",
//	so we have to create one artificially in order to access
//	our functions
//	Our context here is the instance of the class, stored in
//	a static variable
static
extioHandler	*myContext	= NULL;

static
int	extioCallback (int cnt, int status, float IQoffs, void *IQData) {
	if (cnt > 0) { 	//	we got data
	   if (myContext != NULL && myContext -> isStarted) 
	      myContext -> theReader -> processData (IQoffs, IQData, cnt);
	}
	else	// we got something 
	if (cnt < 0) {
	   fprintf (stderr, "got a status cmd %d\n", status);
	   switch (status) {
	// for the SDR14
	      case extHw_Disconnected:
	      case extHw_READY:
	      case extHw_RUNNING:
	      case extHw_ERROR:
	      case extHw_OVERLOAD:
	         break;
	      case extHw_Changed_SampleRate:	// 100	ignore
	         break;
	      case extHw_Changed_LO:		// 101
	         break;
	      case extHw_Lock_LO:		// 102
	         break;
	      case extHw_Unlock_LO:		// 103
	         break;
	      case extHw_Changed_LO_Not_TUNE:	// 104
	         break;
	      case extHw_Changed_TUNE:		// 105
	         break;
	      case extHw_Changed_MODE:		// 106
	         break;
	      case extHw_Start:			// 107
	         break;
	      case extHw_Stop:			// 108
	         break;
	      case extHw_Changed_FILTER:	// 109
	         break;
	      default:
	         break;
	   }
	}
	return 1;
}
//
//	the seemingly additional complexity is caused by the fact
//	that naming of files in windows is in MultiBytechars
//
//	We assume that if there are settings possible, they
//	are dealt with by the producer of the extio, so here 
//	no frame whatsoever.
	extioHandler::extioHandler (QSettings *s) {
#ifdef	__MINGW32__
char	temp [256];
wchar_t	*windowsName;
int16_t	wchars_num;
#endif
int32_t	inputRate	= 0;

	lastFrequency	= Khz (25000);
	base_16		= s -> value ("base_16", 128). toInt();
	base_24		= s -> value ("base_24", 32767 * 256). toInt();
	base_32		= s -> value ("base_32", 32767 * 32768). toInt();
	isStarted	= false;
	theReader	= NULL;
	dll_open	= false;

	QString	dll_file	= "foute boel";
	dll_file = QFileDialog::
	           getOpenFileName (NULL,
	                            tr ("load file .."),
	                            QDir::currentPath(),
#ifdef	__MINGW32__
	                            tr ("libs (Extio*.dll)"));
#else
	                            tr ("libs (*.so)"));
#endif
	dll_file	= QDir::toNativeSeparators (dll_file);
	if (dll_file == QString ("")) {
	   throw (device_exception ("incorrect filename"));
	}

#ifdef	__MINGW32__
	wchars_num = MultiByteToWideChar (CP_UTF8, 0,
	                              dll_file. toLatin1(). data(),
	                              -1, NULL, 0);
	windowsName = new wchar_t [wchars_num];
	MultiByteToWideChar (CP_UTF8, 0,
	                     dll_file. toLatin1(). data(),
	                     -1, windowsName, wchars_num);
	wcstombs (temp, windowsName, 128);
	Handle		= LoadLibrary (windowsName);
	fprintf (stderr, "Last error = %ld\n", GetLastError());
#else
	Handle		= dlopen (dll_file. toLatin1(). data(), RTLD_NOW);
#endif
	if (Handle == nullptr) 
	   throw (device_exception ("loading dll file failed"));

	if (!loadFunctions()) {
	   QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("loading functions failed\n"));
	   FREELIBRARY (Handle);
	   throw (device_exception ("loading functions from dll file failed"));
	}
//	apparently, the library is open, so record that
	dll_open	= true;
	myContext	= (extioHandler *)this;

	if (DllMain != nullptr) {
	   fprintf (stderr, "we have a main\n");
	   fflush (stderr);
	   DllMain (nullptr, DLL_PROCESS_ATTACH, nullptr);
	}
//	and start the rig
	rigName		= new char [128];
	rigModel	= new char [128];
	if (!((*InitHW) (rigName, rigModel, hardwareType))) {
	   FREELIBRARY (Handle);
	   throw (device_exception ("init failed"));
	}

	SetCallback (extioCallback);
	if (!(*OpenHW)()) {
	   FREELIBRARY (Handle);
	   throw (device_exception ("Opening hardware failed"));
	}

	ShowGUI();
	bool	OK = false;
	while (!OK) {
	   inputRate	= GetHWSR();
	   fprintf (stderr, "inputRate = %d\n", inputRate);
	   if ((inputRate < Khz (2000)) || (inputRate > Khz (4000)) ||
	       (1000 * (inputRate / 1000) != inputRate)) {
	       QMessageBox::StandardButton resultButton =
                        QMessageBox::question (nullptr, "extio",
                                               tr ("cannot handle this rate\n\
	                                            give up?\n"),
                                               QMessageBox::No | QMessageBox::Yes,
                                               QMessageBox::Yes);
              if (resultButton == QMessageBox::Yes) {
	         HideGUI ();
	         throw (25);
	      }
	   }
	   else {
	      QString f = QString::number (inputRate / 1000) + " KHz is OK?";
	      QMessageBox::StandardButton testButton =
	            QMessageBox::question (nullptr, "extio",
	                                     tr (f. toLatin1 (). data ()), 
	                                   QMessageBox::No | QMessageBox::Yes,
	                                     QMessageBox::Yes);
	      if (testButton == QMessageBox::Yes)
	         OK = true;
	   }
	}

	_I_Buffer	= new RingBuffer<std::complex<float>>(1024 * 1024);
	fprintf (stderr, "hardware type = %d\n", hardwareType);
	switch (hardwareType) {
	   case exthwNone:
	   case exthwSDRX:
	   case exthwHPSDR:
	   case exthwSDR14:
	   case exthwSCdata:
	   default:
	      QMessageBox::warning (NULL, tr ("sdr"),
	                               tr ("device not supported\n"));
	      return;

	   case exthwUSBdata16:
	      theReader	= new reader_16 (_I_Buffer, base_16, inputRate);
	      break;
	   case exthwUSBdata24:
	      theReader	= new reader_24 (_I_Buffer, base_24, inputRate);
	      break;
	   case exthwUSBdata32:
	      theReader	= new reader_32 (_I_Buffer, base_32, inputRate);
	      break;
	   case exthwUSBfloat32:
	      theReader	= new reader_float (_I_Buffer, inputRate);
	      break;
	}

	fprintf (stderr, "Hw open successful\n");
}

	extioHandler::~extioHandler (void) {
	if (dll_open) {
	   HideGUI();
	   StopHW();
	   CloseHW();
	}

	if (Handle != NULL)
	   FREELIBRARY (Handle);

	if (theReader != NULL)
	   delete theReader;
}

bool	extioHandler::loadFunctions (void) {
//	start binding addresses, 
	DllMain		= nullptr;
	DllMain		= (pfnDllMain)GETPROCADDRESS (Handle, "DllMain");
	
	InitHW		= (pfnInitHW)GETPROCADDRESS (Handle, "InitHW");
	if (InitHW == NULL) {
	   fprintf (stderr, "Failed to load InitHW\n");
	   return false;
	}

	OpenHW		= (pfnOpenHW)GETPROCADDRESS (Handle, "OpenHW");
	if (OpenHW == NULL) {
	   fprintf (stderr, "Failed to load OpenHW\n");
	   return false;
	}

	StartHW		= (pfnStartHW)GETPROCADDRESS (Handle, "StartHW");
	if (StartHW == NULL) {
	   fprintf (stderr, "Failed to load StartHW\n");
	   return false;
	}

	StopHW		= (pfnStopHW)GETPROCADDRESS (Handle, "StopHW");
	if (StopHW == NULL) {
	   fprintf (stderr, "Failed to load StopHW\n");
	   return false;
	}

	CloseHW		= (pfnCloseHW)GETPROCADDRESS (Handle, "CloseHW");
	if (CloseHW == NULL) {
	   fprintf (stderr, "Failed to load CloseHW\n");
	   return false;
	}

//	GetHWLO		= (pfnGetHWLO)GETPROCADDRESS (Handle, "GetHWLO");
//	if (GetHWLO == NULL) {
//	   fprintf (stderr, "Failed to load GetHWLO\n");
//	   return false;
//	}

	SetHWLO		= (pfnSetHWLO)GETPROCADDRESS (Handle, "SetHWLO");
	if (SetHWLO == NULL) {
	   fprintf (stderr, "Failed to load SetHWLO\n");
	   return false;
	}

	GetStatus	= (pfnGetStatus)GETPROCADDRESS (Handle, "GetStatus");
	if (GetStatus == NULL) {
	   fprintf (stderr, "Failed to load GetStatus\n");
	   return false;
	}

	SetCallback	= (pfnSetCallback)
	                      GETPROCADDRESS (Handle, "SetCallback");
	if (SetCallback == NULL) {
	   fprintf (stderr, "Failed to load SetCallback\n");
	   return false;
	}
//
//	the "non essentials", packed in envelope functions:
	L_ShowGUI	= (pfnShowGUI)GETPROCADDRESS (Handle, "ShowGUI");
	L_HideGUI	= (pfnHideGUI)GETPROCADDRESS (Handle, "HideGUI");
	L_GetHWSR	= (pfnGetHWSR)GETPROCADDRESS (Handle, "GetHWSR");
	L_GetFilters	= (pfnGetFilters)GETPROCADDRESS (Handle, "GetFilters");
	L_GetTune	= (pfnGetTune)GETPROCADDRESS (Handle, "GetTune");
	L_GetMode	= (pfnGetMode)GETPROCADDRESS (Handle, "GetMode");
	L_GetHWLO	= (pfnGetHWLO)GETPROCADDRESS (Handle, "GetHWLO");
//
	return true;
}

int32_t	extioHandler::getRate	(void) {
	return GetHWSR();
}

void	extioHandler::setVFOFrequency (int32_t f) {
	fprintf (stderr, "setting freq to %d\n", f);
int	h =  (*SetHWLO) ((int)f);
	lastFrequency = f;
}

int32_t	extioHandler::getVFOFrequency (void) {
//	lastFrequency = (*GetHWLO)();
	return lastFrequency;
}

//
//	envelopes for functions that might or might not
//	be available
void	extioHandler::ShowGUI		(void) {
	if (L_ShowGUI != NULL)
	   (*L_ShowGUI)();
}

void	extioHandler::HideGUI		(void) {
	if (L_HideGUI != NULL)
	   (*L_HideGUI)();
}

long	extioHandler::GetHWSR		(void) {
	return L_GetHWSR != NULL ? (*L_GetHWSR)() : 192000;
}

long	extioHandler::GetHWLO		(void) {
	return L_GetHWLO != NULL ? (*L_GetHWLO)() : Mhz (200);
}

//
//
//	Handling the data
bool	extioHandler::restartReader	(int32_t freq, int skipped) {
//	fprintf (stderr, "setting freq to %d\n", freq);
	toSkip = skipped;
int	h =  (*SetHWLO) ((int)freq);
	lastFrequency = freq;
	fprintf (stderr, "restart reader entered (%d)\n", lastFrequency);
int32_t	size	= (*StartHW)(lastFrequency);
	fprintf (stderr, "restart reader returned with %d\n", size);
	theReader -> restartReader (size);
	fprintf (stderr, "now we have restarted the reader\n");
	isStarted	= true;
	return true;
}

void	extioHandler::stopReader	(void) {
	if (isStarted) {
	   (*StopHW)();
	   theReader	-> stopReader();
	   isStarted = false;
	}
}

int32_t	extioHandler::Samples		(void) {
int32_t	x = _I_Buffer -> GetRingBufferReadAvailable();
	if (x < 0)
	   fprintf (stderr, "toch een fout in ringbuffer\n");
	return x;
}

int32_t	extioHandler::getSamples	(std::complex<float> *buffer,
	                                 int32_t number) {
	return _I_Buffer -> getDataFromBuffer (buffer, number);
}

int16_t	extioHandler::bitDepth		(void) {
	return	theReader	-> bitDepth();
}

int32_t	extioHandler::defaultFrequency (void) {
	return Khz (220000);
}

