# /*
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

#ifndef __SDRPLAY_HANDLER__
#define	__SDRPLAY_HANDLER__

#include	<QObject>
#include	<QFrame>
#include	<QSettings>
#include	<atomic>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"virtual-input.h"
#include	"ui_sdrplay-widget.h"
#include	"mirsdrapi-rsp.h"

typedef void (*mir_sdr_StreamCallback_t)(int16_t	*xi,
	                                 int16_t	*xq,
	                                 uint32_t	firstSampleNum, 
	                                 int32_t	grChanged,
	                                 int32_t	rfChanged,
	                                 int32_t	fsChanged,
	                                 uint32_t	numSamples,
	                                 uint32_t	reset,
	                                 uint32_t	hwRemoved,
	                                 void		*cbContext);
typedef	void	(*mir_sdr_GainChangeCallback_t)(uint32_t	gRdB,
	                                        uint32_t	lnaGRdB,
	                                        void		*cbContext);

#ifdef __MINGW32__
#define	GETPROCADDRESS	GetProcAddress
#else
#define	GETPROCADDRESS	dlsym
#endif

// Dll and ".so" function prototypes
typedef mir_sdr_ErrT (*pfn_mir_sdr_StreamInit) (int *gRdB, double fsMHz,
double rfMHz, mir_sdr_Bw_MHzT bwType, mir_sdr_If_kHzT ifType, int LNAEnable, int *gRdBsystem, int useGrAltMode, int *samplesPerPacket, mir_sdr_StreamCallback_t StreamCbFn, mir_sdr_GainChangeCallback_t GainChangeCbFn, void *cbContext); 

typedef mir_sdr_ErrT (*pfn_mir_sdr_Reinit) (int *gRdB, double fsMHz,
double rfMHz, mir_sdr_Bw_MHzT bwType, mir_sdr_If_kHzT ifType,
mir_sdr_LoModeT, int, int*, int, int*, mir_sdr_ReasonForReinitT);

typedef mir_sdr_ErrT (*pfn_mir_sdr_StreamUninit)();
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetRf)(double drfHz, int abs, int syncUpdate);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetFs)(double dfsHz, int abs, int syncUpdate, int reCal);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetGr)(int gRdB, int abs, int syncUpdate);
typedef mir_sdr_ErrT (*pfn_mir_sdr_RSP_SetGr)(int gRdB, int lnaState,
		                                       int abs, int syncUpdate);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetGrParams)(int minimumGr, int lnaGrThreshold);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetDcMode)(int dcCal, int speedUp);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetDcTrackTime)(int trackTime);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetSyncUpdateSampleNum)(unsigned int sampleNum);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetSyncUpdatePeriod)(unsigned int period);
typedef mir_sdr_ErrT (*pfn_mir_sdr_ApiVersion)(float *version);   
typedef mir_sdr_ErrT (*pfn_mir_sdr_ResetUpdateFlags)(int resetGainUpdate, int resetRfUpdate, int resetFsUpdate);   
typedef mir_sdr_ErrT (*pfn_mir_sdr_AgcControl)(uint32_t, int, int, uint32_t,
	                                       uint32_t, int, int);
typedef mir_sdr_ErrT (*pfn_mir_sdr_DCoffsetIQimbalanceControl) (uint32_t, uint32_t);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetPpm)(double);   
typedef mir_sdr_ErrT (*pfn_mir_sdr_DebugEnable)(uint32_t);   
typedef mir_sdr_ErrT (*pfn_mir_sdr_GetDevices) (mir_sdr_DeviceT *, uint32_t *, uint32_t);
typedef mir_sdr_ErrT (*pfn_mir_sdr_GetCurrentGain) (mir_sdr_GainValuesT *);
typedef mir_sdr_ErrT (*pfn_mir_sdr_GetHwVersion) (unsigned char *);
typedef mir_sdr_ErrT (*pfn_mir_sdr_RSPII_AntennaControl) (mir_sdr_RSPII_AntennaSelectT);
typedef mir_sdr_ErrT (*pfn_mir_sdr_rspDuo_TunerSel) (mir_sdr_rspDuo_TunerSelT);
typedef mir_sdr_ErrT (*pfn_mir_sdr_SetDeviceIdx) (unsigned int);
typedef mir_sdr_ErrT (*pfn_mir_sdr_ReleaseDeviceIdx) (unsigned int);

///////////////////////////////////////////////////////////////////////////
class	sdrplayHandler: public virtualInput, public Ui_sdrplayWidget {
Q_OBJECT
public:
			sdrplayHandler		(QSettings *);
			~sdrplayHandler();
	void		setVFOFrequency		(int32_t);
	int32_t		getVFOFrequency();
	int32_t		defaultFrequency();

	bool		restartReader();
	void		stopReader();
	int32_t		getSamples		(std::complex<float> *,
	                                                          int32_t);
	int32_t		Samples();
	void		resetBuffer();
	int16_t		bitDepth();
//
//	The buffer should be visible by the callback function
	RingBuffer<std::complex<float>>	*_I_Buffer;
	float		denominator;
private:
	pfn_mir_sdr_StreamInit	my_mir_sdr_StreamInit;
	pfn_mir_sdr_Reinit	my_mir_sdr_Reinit;
	pfn_mir_sdr_StreamUninit	my_mir_sdr_StreamUninit;
	pfn_mir_sdr_SetRf	my_mir_sdr_SetRf;
	pfn_mir_sdr_SetFs	my_mir_sdr_SetFs;
	pfn_mir_sdr_SetGr	my_mir_sdr_SetGr;
	pfn_mir_sdr_RSP_SetGr	my_mir_sdr_RSP_SetGr;
	pfn_mir_sdr_SetGrParams	my_mir_sdr_SetGrParams;
	pfn_mir_sdr_SetDcMode	my_mir_sdr_SetDcMode;
	pfn_mir_sdr_SetDcTrackTime my_mir_sdr_SetDcTrackTime;
	pfn_mir_sdr_SetSyncUpdateSampleNum
	                        my_mir_sdr_SetSyncUpdateSampleNum;
	pfn_mir_sdr_SetSyncUpdatePeriod
	                        my_mir_sdr_SetSyncUpdatePeriod;
	pfn_mir_sdr_ApiVersion	my_mir_sdr_ApiVersion;
	pfn_mir_sdr_ResetUpdateFlags
	                        my_mir_sdr_ResetUpdateFlags;
	pfn_mir_sdr_AgcControl	my_mir_sdr_AgcControl;
	pfn_mir_sdr_DCoffsetIQimbalanceControl
	                        my_mir_sdr_DCoffsetIQimbalanceControl;
	pfn_mir_sdr_SetPpm	my_mir_sdr_SetPpm;
	pfn_mir_sdr_DebugEnable	my_mir_sdr_DebugEnable;
	pfn_mir_sdr_GetDevices  my_mir_sdr_GetDevices;
	pfn_mir_sdr_GetCurrentGain my_mir_sdr_GetCurrentGain;
	pfn_mir_sdr_GetHwVersion my_mir_sdr_GetHwVersion;
	pfn_mir_sdr_RSPII_AntennaControl my_mir_sdr_RSPII_AntennaControl;
	pfn_mir_sdr_rspDuo_TunerSel my_mir_sdr_rspDuo_TunerSel;
	pfn_mir_sdr_SetDeviceIdx my_mir_sdr_SetDeviceIdx;
	pfn_mir_sdr_ReleaseDeviceIdx my_mir_sdr_ReleaseDeviceIdx;

	QString		errorCodes	(mir_sdr_ErrT);
	int16_t		hwVersion;
	uint32_t	numofDevs;
	int16_t		deviceIndex;
	bool		loadFunctions();
	QSettings	*sdrplaySettings;
	QFrame		*myFrame;
	int32_t		inputRate;
	int32_t		vfoFrequency;
	bool		libraryLoaded;
	std::atomic<bool>	running;
	HINSTANCE	Handle;
	bool		agcMode;
	int16_t		nrBits;
private slots:
	void		set_ifgainReduction	(int);
	void		set_lnagainReduction	(int);
	void		set_agcControl		(int);
	void		set_debugControl	(int);
	void		set_ppmControl		(int);
	void		set_antennaSelect	(const QString &);
	void		set_tunerSelect		(const QString &);
};
#endif

