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

#ifndef __PLUTO_HANDLER__
#define	__PLUTO_HANDLER__

#include	<QtNetwork>
#include        <QMessageBox>
#include        <QByteArray>
#include	<QObject>
#include	<QFrame>
#include	<QSettings>
#include	<atomic>
#include	<iio.h>
#include	"dab-constants.h"
#include	"ringbuffer.h"
#include	"device-handler.h"
#include	"ui_pluto-widget.h"

class	xml_fileWriter;

#ifndef	PLUTO_RATE
#define	PLUTO_RATE	2100000
#define	DAB_RATE	2048000
#define	DIVIDER		1000
#define	CONV_SIZE	(PLUTO_RATE / DIVIDER)
#endif

extern "C" {
typedef struct iio_channel * (*p_iio_device_find_channel)(
                const struct iio_device *dev, const char *name, bool output);
typedef struct iio_context * (*p_iio_create_default_context)(void);
typedef struct iio_context * (*p_iio_create_local_context)(void);
typedef struct iio_context * (*p_iio_create_network_context)(const char *host);
typedef const char * (*p_iio_context_get_name)(const struct iio_context *ctx);
typedef unsigned int  (*p_iio_context_get_devices_count)(
                                             const struct iio_context *ctx);
typedef struct iio_device * (*p_iio_context_find_device)(
                           const struct iio_context *ctx, const char *name);
typedef size_t (*p_iio_device_attr_write)(const struct iio_device *dev,
                                       const char *attr, const char *src);
typedef int  (*p_iio_device_attr_write_longlong)(const struct iio_device *dev,
                                       const char *attr, long long val);
typedef ssize_t (*p_iio_channel_attr_write)(const struct iio_channel *chn,
                                       const char *attr, const char *src);
typedef int (*p_iio_channel_attr_write_longlong)(const struct iio_channel *chn,
                                       const char *attr, long long val);

typedef void (*p_iio_channel_enable)(struct iio_channel *chn);

typedef struct iio_buffer * (*p_iio_device_create_buffer)(const struct iio_device *dev,
                                      size_t samples_count, bool cyclic);
typedef int  (*p_iio_buffer_set_blocking_mode)(struct iio_buffer *buf, bool blocking);
typedef void (*p_iio_buffer_destroy) (struct iio_buffer *buf);
typedef void (*p_iio_context_destroy)(struct iio_context *ctx);
typedef ssize_t (*p_iio_buffer_refill)(struct iio_buffer *buf);
typedef ptrdiff_t (*p_iio_buffer_step)(const struct iio_buffer *buf);
typedef void * (*p_iio_buffer_end)(const struct iio_buffer *buf);
typedef void * (*p_iio_buffer_first)(const struct iio_buffer *buf,
                                    const struct iio_channel *chn);
typedef void (*p_iio_strerror)(int err, char *dst, size_t len);

}

class	plutoHandler: public deviceHandler, public Ui_plutoWidget {
Q_OBJECT
public:
			plutoHandler		(QSettings *, QString &);
            		~plutoHandler		();
	void		setVFOFrequency		(int32_t);
	int32_t		getVFOFrequency		();
	bool		restartReader		(int32_t);
	void		stopReader		();
	int32_t		getSamples		(std::complex<float> *,
	                                                          int32_t);
	int32_t		Samples			();
	void		resetBuffer		();
	int16_t		bitDepth		();

	void		show			();
	void		hide			();
	bool		isHidden		();
	QString		deviceName		();
private:
	QFrame			myFrame;
	RingBuffer<std::complex<float>>	_I_Buffer;
	QSettings		*plutoSettings;
	QString			recorderVersion;
	FILE			*xmlDumper;
	xml_fileWriter		*xmlWriter;
	bool			setup_xmlDump	();
	void			close_xmlDump	();
	std::atomic<bool>	dumping;

	HINSTANCE		Handle;
	bool			load_plutoFunctions	();
	void			run		();
	int32_t			inputRate;
	int32_t			vfoFrequency;
	std::atomic<bool>	running;
	bool			debugFlag;
//      configuration items
        int64_t                 bw_hz; // Analog banwidth in Hz
        int64_t                 fs_hz; // Baseband sample rate in Hz
        int64_t                 lo_hz; // Local oscillator frequency in Hz
	bool			get_ad9361_stream_ch (struct iio_context *ctx,
                                                      struct iio_device *dev,
                                                      int chid,
	                                              struct iio_channel **);

        const char* rfport; // Port name
        struct  iio_channel     *lo_channel;
        struct  iio_channel     *gain_channel;
	struct	iio_device	*rx;
	struct	iio_context	*ctx;
	struct	iio_channel	*rx0_i;
	struct	iio_channel	*rx0_q;
	struct	iio_buffer	*rxbuf;
	bool			connected;
	std::complex<float>	convBuffer	[CONV_SIZE + 1];
	int			convIndex;
	int16_t			mapTable_int	[DAB_RATE / DIVIDER];
	float			mapTable_float	[DAB_RATE / DIVIDER];
//
//	and the functions from the library
	
	p_iio_device_find_channel	iio_device_find_channel;
	p_iio_create_default_context	iio_create_default_context;
	p_iio_create_local_context	iio_create_local_context;
	p_iio_create_network_context	iio_create_network_context;
	p_iio_context_get_name		iio_context_get_name;
	p_iio_context_get_devices_count	iio_context_get_devices_count;
	p_iio_context_find_device	iio_context_find_device;
	p_iio_device_attr_write		iio_device_attr_write;
	p_iio_device_attr_write_longlong  iio_device_attr_write_longlong;
	p_iio_channel_attr_write	iio_channel_attr_write;
	p_iio_channel_attr_write_longlong iio_channel_attr_write_longlong;
	p_iio_channel_enable		iio_channel_enable;
	p_iio_device_create_buffer	iio_device_create_buffer;
	p_iio_buffer_set_blocking_mode	iio_buffer_set_blocking_mode;
	p_iio_buffer_destroy		iio_buffer_destroy;
	p_iio_context_destroy		iio_context_destroy;
	p_iio_buffer_refill		iio_buffer_refill;
	p_iio_buffer_step		iio_buffer_step;
	p_iio_buffer_end		iio_buffer_end;
	p_iio_buffer_first		iio_buffer_first;
	p_iio_strerror			iio_strerror;
private slots:
	void		set_gainControl		(int);
	void		set_agcControl		(int);
	void		toggle_debugButton	();
	void		set_xmlDump		();
};
#endif

