
/**
 *  IW0HDV Extio
 *
 *  Copyright 2015 by Andrea Montefusco IW0HDV
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *	recoding and taking parts for the airspyRadio interface
 *	for the Qt-DAB program
 *	jan van Katwijk
 *	Lazy Chair Computing
 */
#ifndef __AIRSPY_HANDLER__
#define	__AIRSPY_HANDLER__

#include	<QObject>
#include	<QSettings>
#include	<QFrame>
#include	<vector>
#include	<atomic>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"virtual-input.h"
#include	"ui_airspy-widget.h"
#ifndef	__MINGW32__
#include	"libairspy/airspy.h"
#else
#include	"libairspy/airspy.h"
#endif

class	airspyFilter;

extern "C"  {
typedef	int (*pfn_airspy_init)();
typedef int (*pfn_airspy_exit)();
typedef int (*pfn_airspy_open) (struct airspy_device**);
typedef int (*pfn_airspy_close) (struct airspy_device*);
typedef int (*pfn_airspy_get_samplerates) (struct airspy_device* device,
	                                   uint32_t* buffer,
	                                   const uint32_t len);
typedef int (*pfn_airspy_set_samplerate) (struct airspy_device* device,
	                                  uint32_t samplerate);
typedef int (*pfn_airspy_start_rx) (struct airspy_device* device,
	                            airspy_sample_block_cb_fn callback,
	                            void* rx_ctx);
typedef int (*pfn_airspy_stop_rx) (struct airspy_device* device);

typedef int (*pfn_airspy_set_sample_type) (struct airspy_device *,
	                                   airspy_sample_type);
typedef int (*pfn_airspy_set_freq) (struct airspy_device* device,
	                            const uint32_t freq_hz);

typedef int (*pfn_airspy_set_lna_gain) (struct airspy_device* device,
	                                uint8_t value);

typedef int (*pfn_airspy_set_mixer_gain) (struct airspy_device* device,
	                                  uint8_t value);

typedef int (*pfn_airspy_set_vga_gain) (struct airspy_device*
	                                device, uint8_t
	                                value);
typedef int (*pfn_airspy_set_lna_agc) (struct airspy_device* device,
	                               uint8_t value);
typedef int (*pfn_airspy_set_mixer_agc) (struct airspy_device* device,
	                                 uint8_t value);

typedef int (*pfn_airspy_set_rf_bias) (struct airspy_device* dev,
	                               uint8_t value);

typedef const char* (*pfn_airspy_error_name) (enum airspy_error errcode);
typedef int (*pfn_airspy_board_id_read) (struct airspy_device *,
	                                         uint8_t *);
typedef const char* (*pfn_airspy_board_id_name) (enum airspy_board_id board_id);
typedef int (*pfn_airspy_board_partid_serialno_read)(struct airspy_device* device, airspy_read_partid_serialno_t* read_partid_serialno);

typedef int (*pfn_airspy_set_linearity_gain) (struct airspy_device* device, uint8_t value);
typedef int (*pfn_airspy_set_sensitivity_gain)(struct airspy_device* device, uint8_t value);
}

class airspyHandler: public virtualInput, public Ui_airspyWidget {
Q_OBJECT
public:
			airspyHandler		(QSettings *);
			~airspyHandler();
	void		setVFOFrequency		(int32_t nf);
	int32_t		getVFOFrequency();
	int32_t		defaultFrequency();
	bool		restartReader();
	void		stopReader();
	int32_t		getSamples		(std::complex<float> *v,
	                                                 int32_t size);
	int32_t		Samples();
	void		resetBuffer();
	int16_t		bitDepth();
	int16_t		currentTab;
private slots:
	void		set_linearity		(int value);
	void		set_sensitivity		(int value);
	void		set_lna_gain		(int value);
	void		set_mixer_gain		(int value);
	void		set_vga_gain		(int value);
	void		set_lna_agc();
	void		set_mixer_agc();
	void		set_rf_bias();
	void		show_tab		(int);
private:
	bool		load_airspyFunctions();
//	The functions to be extracted from the dll/.so file
	pfn_airspy_init		   my_airspy_init;
	pfn_airspy_exit		   my_airspy_exit;
	pfn_airspy_error_name	   my_airspy_error_name;
	pfn_airspy_open		   my_airspy_open;
	pfn_airspy_close	   my_airspy_close;
	pfn_airspy_get_samplerates my_airspy_get_samplerates;
	pfn_airspy_set_samplerate  my_airspy_set_samplerate;
	pfn_airspy_start_rx	   my_airspy_start_rx;
	pfn_airspy_stop_rx	   my_airspy_stop_rx;
	pfn_airspy_set_sample_type my_airspy_set_sample_type;
	pfn_airspy_set_freq	   my_airspy_set_freq;
	pfn_airspy_set_lna_gain	   my_airspy_set_lna_gain;
	pfn_airspy_set_mixer_gain  my_airspy_set_mixer_gain;
	pfn_airspy_set_vga_gain	   my_airspy_set_vga_gain;
	pfn_airspy_set_linearity_gain my_airspy_set_linearity_gain;
	pfn_airspy_set_sensitivity_gain my_airspy_set_sensitivity_gain;
	pfn_airspy_set_lna_agc	   my_airspy_set_lna_agc;
	pfn_airspy_set_mixer_agc   my_airspy_set_mixer_agc;
	pfn_airspy_set_rf_bias	   my_airspy_set_rf_bias;
	pfn_airspy_board_id_read   my_airspy_board_id_read;
	pfn_airspy_board_id_name   my_airspy_board_id_name;
	pfn_airspy_board_partid_serialno_read
		                   my_airspy_board_partid_serialno_read;
//
	HINSTANCE	Handle_usb;
	HINSTANCE	Handle;
	bool		libraryLoaded;
	QFrame		*myFrame;
	bool		success;
	std::atomic<bool>	running;
	bool		lna_agc;
	bool		mixer_agc;
	bool		rf_bias;
const	char*		board_id_name();

	int16_t		vgaGain;
	int16_t		mixerGain;
	int16_t		lnaGain;
	int32_t		selectedRate;
	int16_t		convBufferSize;
	int16_t		convIndex;
	std::vector <complex<float> >	convBuffer;
	int16_t		mapTable_int   [4 * 512];
	float		mapTable_float [4 * 512];
	QSettings	*airspySettings;
	RingBuffer<std::complex<float>> *theBuffer;
	int32_t		inputRate;
	airspyFilter	*filter;
	bool		filtering;
	struct airspy_device* device;
	uint64_t 	serialNumber;
	char		serial[128];
static
	int		callback(airspy_transfer_t *);
	int		data_available (void *buf, int buf_size);
const	char *		getSerial();
	int		open();
};

#endif
