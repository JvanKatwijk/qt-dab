#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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

#ifndef __SDRPLAY_HANDLER_V3__
#define	__SDRPLAY_HANDLER_V3__

#include	<QObject>
#include	<QFrame>
#include	<QSettings>
#include	<atomic>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"virtual-input.h"
#include	"ui_sdrplay-widget-v3.h"
#include	"sdrplay_api.h"

class	controlQueue;
#ifdef __MINGW32__
#define GETPROCADDRESS  GetProcAddress
#else
#define GETPROCADDRESS  dlsym
#endif

class	sdrplayHandler_v3: public virtualInput, public Ui_sdrplayWidget_v3 {
Q_OBJECT
public:
			sdrplayHandler_v3	(QSettings *);
			~sdrplayHandler_v3	();
	void		setVFOFrequency		(int32_t);
	int32_t		getVFOFrequency();
	int32_t		defaultFrequency();

	bool		restartReader		(int32_t);
	void		stopReader();
	int32_t		getSamples		(std::complex<float> *,
	                                                          int32_t);
	int32_t		Samples			();
	void		resetBuffer		();
	int16_t		bitDepth		();

	int				denominator;
	RingBuffer<std::complex<float>>	*_I_Buffer;
	void				setTotalGain		(int);
	bool				receiverRuns;
private:
	sdrplay_api_Open_t		sdrplay_api_Open;
	sdrplay_api_Close_t		sdrplay_api_Close;
	sdrplay_api_ApiVersion_t	sdrplay_api_ApiVersion;
	sdrplay_api_LockDeviceApi_t	sdrplay_api_LockDeviceApi;
	sdrplay_api_UnlockDeviceApi_t	sdrplay_api_UnlockDeviceApi;
	sdrplay_api_GetDevices_t	sdrplay_api_GetDevices;
	sdrplay_api_SelectDevice_t	sdrplay_api_SelectDevice;
	sdrplay_api_ReleaseDevice_t	sdrplay_api_ReleaseDevice;
	sdrplay_api_GetErrorString_t	sdrplay_api_GetErrorString;
//	sdrplay_api_GetLastError_t	sdrplay_api_GetlastError;
	sdrplay_api_DebugEnable_t	sdrplay_api_DebugEnable;
	sdrplay_api_GetDeviceParams_t	sdrplay_api_GetDeviceParams;
	sdrplay_api_Init_t		sdrplay_api_Init;
	sdrplay_api_Uninit_t		sdrplay_api_Uninit;
	sdrplay_api_Update_t		sdrplay_api_Update;

	sdrplay_api_DeviceParamsT 	*deviceParams;
	sdrplay_api_CallbackFnsT 	cbFns;
	sdrplay_api_RxChannelParamsT 	*chParams;
	sdrplay_api_DeviceT		*chosenDevice;

	controlQueue		*theQueue;
	uint32_t		numofDevs;
	int16_t			deviceIndex;
	int32_t			vfoFrequency;
	std::atomic<bool>	running;
	std::atomic<bool>	threadRuns;
	HINSTANCE		Handle;

	void			run			();
	HINSTANCE		fetchLibrary		();
	bool			loadFunctions		();
	int16_t			hwVersion;
	QSettings		*sdrplaySettings;
	bool			agcMode;
	int16_t			nrBits;
private slots:
	void			set_ifgainReduction	(int);
	void			set_lnagainReduction	(int);
	void			set_agcControl		(int);
	void			set_ppmControl		(int);
	void			set_antennaSelect	(const QString &);
public slots:
	void			show_error		(int);
	void			showDeviceData		(const QString &,
	                                                     int, float);
	void			displayGain		(int);
signals:
        void                    setDeviceData   (const QString &, int, float);
        void                    error           (int);
        void                    actualGain      (int);
	void			showTotalGain	(int);

};
#endif

