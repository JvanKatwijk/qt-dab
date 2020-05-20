#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the sdrplayDab program
 *
 *    sdrplayDab is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    sdrplayDab is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with sdrplayDab; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __SDRPLAY_HANDLER_V3__
#define	__SDRPLAY_HANDLER_V3__

#include	<QObject>
#include	<QFrame>
#include	<QSettings>
#include	<QSemaphore>
#include	<atomic>
#include	<stdio.h>
#include	<queue>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"device-handler.h"
#include	"ui_sdrplay-widget.h"
#include	<sdrplay_api.h>

class	generalCommand;
class	RadioInterface;

#ifdef __MINGW32__
#define GETPROCADDRESS  GetProcAddress
#else
#define GETPROCADDRESS  dlsym
#endif

class	sdrplayHandler_v3: public deviceHandler, public Ui_sdrplayWidget {
Q_OBJECT
public:
			sdrplayHandler_v3	(RadioInterface *,
	                                         QSettings *,
	                                         dabProcessor *, int);
			~sdrplayHandler_v3	();
	bool		restartReader		(int32_t);
	void		stopReader		();
	void		setVFOFrequency		(int);
	int32_t		getVFOFrequency		();
	void		resetBuffer		();
	int16_t		bitDepth		();
	void		show			();
	void		hide			();
	bool		isHidden		();
	void		set_GRdB		(int);
//	some callback functions need access to:

        int			denominator;
        std::atomic<bool>       receiverRuns;
        std::atomic<bool>       threadRuns;
        dabProcessor		*base;
        void			update_PowerOverload (
                                         sdrplay_api_EventParamsT *params);
        void			setOffset	(int);
        void			setGains	(float, float);
        int			theGain;

private:
	QFrame			myFrame;
	QSettings		*sdrplaySettings;
	int			inputRate;
	void			run			();
	bool			messageHandler		(generalCommand *);
	std::atomic<int		reportIndicator;
	int32_t			vfoFrequency;

	sdrplay_api_Open_t	sdrplay_api_Open;
        sdrplay_api_Close_t	sdrplay_api_Close;
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

	sdrplay_api_DeviceT             *chosenDevice;
	sdrplay_api_DeviceParamsT       *deviceParams;
	sdrplay_api_CallbackFnsT        cbFns;
	sdrplay_api_RxChannelParamsT    *chParams;

	int16_t				hwVersion;
        uint32_t			numofDevs;
        int16_t				deviceIndex;
        HINSTANCE			Handle;
        int16_t				nrBits;
	int				lna_upperBound;
	QString				deviceName;
	float				apiVersion;
	QString				serial;
	int				gainSetPoint;
	bool				has_antennaSelect;
	bool				agcMode;
	int				lnaState;
	int				ppmValue;
	std::queue<generalCommand *>	server_queue;
	QSemaphore			serverjobs;
	HINSTANCE			fetchLibrary	();
	void				releaseLibrary	();
	bool				loadFunctions	();
private slots:
	void			set_lnagainReduction	(int);
	void			set_agcControl		(int);
	void			set_ppmControl		(int);
	void			set_antennaSelect	(const QString &);
	void			set_tunerSelect		(const QString &);
	void			set_gain		(int);
public slots:
	void			avgValue		(float);
        void			dipValue		(float);
        void			show_TotalGain		(float);
        void			freq_offset		(int);
        void			freq_error		(int);
signals:;
	void			avgValue_signal		(float);
        void			dipValue_signal		(float);
        void			show_TotalGain_signal	(float);
        void			freq_offset_signal	(int);
        void			freq_error_signal	(int);
};
#endif

