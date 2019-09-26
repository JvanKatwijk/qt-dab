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

#ifndef __LIME_HANDLER__
#define	__LIME_HANDLER__

#include	<QObject>
#include	<QFrame>
#include	<QSettings>
#include	<atomic>
#include	<vector>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	<LimeSuite.h>
#include	"virtual-input.h"
#include	"ui_lime-widget.h"

#ifdef __MINGW32__
#define GETPROCADDRESS  GetProcAddress
#else
#define GETPROCADDRESS  dlsym
#endif

//	DLL and ".so" function prototypes
typedef int	(*pfn_LMS_GetDeviceList)(lms_info_str_t *dev_list);
typedef	int	(*pfn_LMS_Open)(lms_device_t **device,
	                         const lms_info_str_t info, void* args);
typedef	int	(*pfn_LMS_Close)(lms_device_t *);
typedef	int	(*pfn_LMS_Init) (lms_device_t *);
typedef	int	(*pfn_LMS_GetNumChannels) (lms_device_t *device, bool dir_tx);
typedef	int	(*pfn_LMS_EnableChannel) (lms_device_t *device, bool dir_tx,
	                                            size_t chan, bool enabled);
typedef	int	(*pfn_LMS_SetSampleRate) (lms_device_t *device,
	                                            float_type rate,
	                                             size_t oversample);
typedef	int	(*pfn_LMS_GetSampleRate)(lms_device_t *device, bool dir_tx,
	             size_t chan, float_type *host_Hz, float_type *rf_Hz);
typedef	int	(*pfn_LMS_SetLOFrequency)(lms_device_t *device, bool dir_tx,
	                                  size_t chan, float_type frequency);
typedef	int	(*pfn_LMS_GetLOFrequency)(lms_device_t *device, bool dir_tx,
	                                  size_t chan, float_type *frequency);
typedef	int	(*pfn_LMS_GetAntennaList) (lms_device_t *device, bool dir_tx,
	                                  size_t chan,  lms_name_t *list);
typedef	int	(*pfn_LMS_SetAntenna) (lms_device_t *dev, bool dir_tx,
	                                  size_t chan, size_t index);
typedef	int	(*pfn_LMS_GetAntenna)(lms_device_t *dev, bool dir_tx, size_t chan);
typedef	int	(*pfn_LMS_GetAntennaBW)(lms_device_t *dev, bool dir_tx,
	                        size_t chan, size_t index, lms_range_t *range);
typedef	int	(*pfn_LMS_SetNormalizedGain)(lms_device_t *device, bool dir_tx,
	                                   size_t chan,float_type gain);
typedef	int	(*pfn_LMS_SetGaindB)(lms_device_t *device, bool dir_tx,
	                                   size_t chan, unsigned gain);
typedef	int	(*pfn_LMS_GetNormalizedGain)(lms_device_t *device, bool dir_tx,
	                                   size_t chan, float_type *gain);
typedef	int	(*pfn_LMS_GetGaindB)(lms_device_t *device, bool dir_tx,
	                                   size_t chan, unsigned *gain);
typedef	int	(*pfn_LMS_SetLPFBW)(lms_device_t *device, bool dir_tx,
	                                   size_t chan, float_type bandwidth);
typedef	int	(*pfn_LMS_GetLPFBW)(lms_device_t *device, bool dir_tx,
                                            size_t chan, float_type *bandwidth);
typedef	int 	(*pfn_LMS_Calibrate)(lms_device_t *device, bool dir_tx,
	                              size_t chan, double bw, unsigned flags);
typedef	int	(*pfn_LMS_SetupStream)(lms_device_t *device,
	                               lms_stream_t *stream);
typedef	int	(*pfn_LMS_DestroyStream)(lms_device_t *dev,
	                                 lms_stream_t *stream);
typedef	int	(*pfn_LMS_StartStream)(lms_stream_t *stream);
typedef	int	(*pfn_LMS_StopStream)(lms_stream_t *stream);
typedef	int	(*pfn_LMS_RecvStream)(lms_stream_t *stream, void *samples,
                                      size_t sample_count,
	                              lms_stream_meta_t *meta,
	                              unsigned timeout_ms);
typedef	int	(*pfn_LMS_GetStreamStatus)(lms_stream_t *stream,
	                               lms_stream_status_t* status);

class	limeHandler: public virtualInput, public Ui_limeWidget {
Q_OBJECT
public:
			limeHandler		(QSettings *);
			~limeHandler();
	void		setVFOFrequency		(int32_t);
	int32_t		getVFOFrequency();
	bool		restartReader();
	void		stopReader();
	int32_t         getSamples              (std::complex<float> *,
                                                                  int32_t);
        int32_t         Samples();
        void            resetBuffer();
        int16_t         bitDepth();

private:
	QSettings	*limeSettings;
	QFrame		*myFrame;
	std::atomic<bool>	running;
	lms_device_t	*theDevice;
	lms_name_t	antennas [10];
	RingBuffer<std::complex<float>> *theBuffer;
	bool		load_limeFunctions();
	HINSTANCE	Handle;
	bool		libraryLoaded;
	lms_stream_meta_t meta;
        lms_stream_t    stream;
        void		run();

//	imported functions
public:
	pfn_LMS_GetDeviceList	LMS_GetDeviceList;
	pfn_LMS_Open		LMS_Open;
	pfn_LMS_Close		LMS_Close;
	pfn_LMS_Init		LMS_Init;
	pfn_LMS_GetNumChannels	LMS_GetNumChannels;
	pfn_LMS_EnableChannel	LMS_EnableChannel;
	pfn_LMS_SetSampleRate	LMS_SetSampleRate;
	pfn_LMS_GetSampleRate	LMS_GetSampleRate;
	pfn_LMS_SetLOFrequency	LMS_SetLOFrequency;
	pfn_LMS_GetLOFrequency	LMS_GetLOFrequency;
	pfn_LMS_GetAntennaList	LMS_GetAntennaList;
	pfn_LMS_SetAntenna	LMS_SetAntenna;
	pfn_LMS_GetAntenna	LMS_GetAntenna;
	pfn_LMS_GetAntennaBW	LMS_GetAntennaBW;
	pfn_LMS_SetNormalizedGain	LMS_SetNormalizedGain;
	pfn_LMS_SetGaindB	LMS_SetGaindB;
	pfn_LMS_GetNormalizedGain	LMS_GetNormalizedGain;
	pfn_LMS_GetGaindB	LMS_GetGaindB;
	pfn_LMS_SetLPFBW	LMS_SetLPFBW;
	pfn_LMS_GetLPFBW	LMS_GetLPFBW;
	pfn_LMS_Calibrate	LMS_Calibrate;

	pfn_LMS_SetupStream	LMS_SetupStream;
	pfn_LMS_DestroyStream	LMS_DestroyStream;
	pfn_LMS_StartStream	LMS_StartStream;
	pfn_LMS_StopStream	LMS_StopStream;
	pfn_LMS_RecvStream	LMS_RecvStream;
	pfn_LMS_GetStreamStatus	LMS_GetStreamStatus;
private slots:
	void		setGain		(int);
	void		setAntenna	(int);

public slots:
	void		showErrors	(int, int);
};

#endif

