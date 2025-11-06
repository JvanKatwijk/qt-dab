#
/*
 *    Copyright (C) 2020
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

#pragma once

#include	<QThread>
#include	<QSettings>
#include	<QSemaphore>
#include	<atomic>
#include	<stdio.h>
#include	<queue>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"device-handler.h"
#include	"ui_sdrplay-widget-v3.h"
#include	<sdrplay_api.h>

#include	<QScopedPointer>
#include	"Rsp-device.h"

class	generalCommand;
class	xml_fileWriter;
class	errorLogger;

#ifdef __MINGW32__
//#include      "dlfcn.h"
#define GETPROCADDRESS  GetProcAddress
#else
#define GETPROCADDRESS  dlsym
#endif

class	sdrplayHandler_v3 final:
	           public deviceHandler, public Ui_sdrplayWidget_v3 {
Q_OBJECT
public:
			sdrplayHandler_v3	(QSettings *,
	                                         const QString &,
	                                         errorLogger *);
			~sdrplayHandler_v3	();

	bool		restartReader		(int32_t, int skipped = 0);
	void		stopReader		();
	int32_t		getSamples		(std::complex<float> *,
	                                                          int32_t);
	int32_t		Samples			();
	void		resetBuffer		();
	int16_t		bitDepth		();
	QString		deviceName		();

	void		startDump		();
	void		stopDump		();

	void            updatePowerOverload (
	                                 sdrplay_api_EventParamsT *params);
	RingBuffer<std::complex<int16_t>>	_I_Buffer;
	std::atomic<bool>	receiverRuns;
	int		theGain;
	sdrplay_api_CallbackFnsT	cbFns;

private:
public:
	sdrplay_api_Open_t              sdrplay_api_Open;
	sdrplay_api_Close_t             sdrplay_api_Close;
	sdrplay_api_ApiVersion_t        sdrplay_api_ApiVersion;
	sdrplay_api_LockDeviceApi_t     sdrplay_api_LockDeviceApi;
	sdrplay_api_UnlockDeviceApi_t   sdrplay_api_UnlockDeviceApi;
	sdrplay_api_GetDevices_t        sdrplay_api_GetDevices;
	sdrplay_api_SelectDevice_t      sdrplay_api_SelectDevice;
	sdrplay_api_ReleaseDevice_t     sdrplay_api_ReleaseDevice;
	sdrplay_api_GetErrorString_t    sdrplay_api_GetErrorString;
	sdrplay_api_GetLastError_t      sdrplay_api_GetLastError;
	sdrplay_api_DebugEnable_t       sdrplay_api_DebugEnable;
	sdrplay_api_GetDeviceParams_t   sdrplay_api_GetDeviceParams;
	sdrplay_api_Init_t              sdrplay_api_Init;
	sdrplay_api_Uninit_t            sdrplay_api_Uninit;
	sdrplay_api_Update_t            sdrplay_api_Update;
	sdrplay_api_SwapRspDuoActiveTuner_t sdrplay_api_SwapRspDuoActiveTuner;
	sdrplay_api_SwapRspDuoDualTunerModeSampleRate_t
		               sdrplay_api_SwapRspDuoDualTunerModeSampleRate;
	sdrplay_api_SwapRspDuoMode_t	sdrplay_api_SwapRspDuoMode;
	sdrplay_api_DeviceT             *chosenDevice;
	QScopedPointer<RspDevice>	theRsp;

	errorLogger		*theErrorLogger;
	std::atomic<bool>	failFlag;
	std::atomic<bool>	successFlag;
	float			denominator;
	std::atomic<bool>       threadRuns;
	void			run			();
	bool			messageHandler		(generalCommand *);

	QString			recorderVersion;
	
	int16_t			hwVersion;
	QSettings		*sdrplaySettings;
	bool			agcMode;
	int16_t			nrBits;
	int			lnaUpperBound;
	float			apiVersion;
	QString			serial;
	QString			deviceModel;
	int			GRdBValue;
	int			lnaState;
	HINSTANCE		Handle;
	double			ppmValue;
	bool			biasT;
	xml_fileWriter		*xmlWriter;
	bool			setupXmlDump		(bool);
	void			closeXmlDump		();
	std::atomic<bool>	dumping;
	std::queue<generalCommand *>	serverQueue;
	QSemaphore		serverJobs;
	HINSTANCE               fetchLibrary            ();
	void                    releaseLibrary          ();
	bool			loadFunctions		();
	int			errorCode;
	int			setAntennaSelect	(int);

signals:
	void			newGRdBValue		(int);
	void			newLnaValue		(int);
	void			newAgcSetting		(bool);
	void			showTunerGain		(double);
private slots:
	void			setIfGainReduction	(int);
	void			setLnaGainReduction	(int);
	void			setAgcControl		(int);
	void			setPpmControl		(int);
	void			setSelectAntenna_RSPdx	(const QString &);
	void			setSelectAntenna_RSP2	(const QString &);
	void			setSelectAntenna_duo	(const QString &);
	void			setSelectTuner		(const QString &);
	void			setBiasT		(int);
	void			setNotch		(int);
	void			reportOverloadState	(bool);
	void			displayGain		(double);
public slots:
	void			setLnaBounds		(int, int);
	void			setSerial		(const QString &);
	void			setApiVersion		(float);
	void			setXmlDump		();
	void			showLnaGain		(int);
	void			showState		(const QString &);
//
//	just for the Duo
	void			enableBiasT		(bool);
signals:
	void			setLnaBoundsSignal	(int, int);
	void			setDeviceNameSignal	(const QString &);
	void			setSerialSignal		(const QString &);
	void			setApiVersionSignal	(float);
	void			setAntennaSelectSignal	(bool);
	void			overloadStateChanged	(bool);
};

