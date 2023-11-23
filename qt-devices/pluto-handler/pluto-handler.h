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

#include	<QtNetwork>
#include        <QMessageBox>
#include	<QThread>
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
#include	<QLibrary>

//#ifdef __MINGW32__
//#define GETPROCADDRESS  GetProcAddress
//#else
//#define GETPROCADDRESS  dlsym
//#endif

class	xml_fileWriter;

#ifndef	PLUTO_RATE
#define	PLUTO_RATE	2112000
#define	DAB_RATE	2048000
#define	DIVIDER		1000
#define	CONV_SIZE	(PLUTO_RATE / DIVIDER)
#define	FM_RATE		2112000
#endif

enum iodev { RX, TX };

//
//	Dll and ".so" function prototypes

typedef struct iio_channel * (*pfn_iio_device_find_channel)(
                                   const struct iio_device *dev,
	                           const char *name, bool output);
typedef struct iio_context * (*pfn_iio_create_default_context)(void);
typedef struct iio_context * (*pfn_iio_create_local_context) (void);
typedef struct iio_context * (*pfn_iio_create_network_context)(const char *host);
typedef const char * (*pfn_iio_context_get_name)(const struct iio_context *ctx);
typedef unsigned int (*pfn_iio_context_get_devices_count)(
                                   const struct iio_context *ctx);
typedef struct iio_device * (*pfn_iio_context_find_device)(
                                   const struct iio_context *ctx,
	                           const char *name);
typedef void (*pfn_iio_channel_enable)(struct iio_channel *chn);

typedef int (*pfn_iio_device_attr_write_bool)(const struct iio_device *dev,
                                          const char *attr, bool val);
typedef int (*pfn_iio_device_attr_read_bool)(const struct iio_device *dev,
                                         const char *attr, bool *val);

typedef int (*pfn_iio_channel_attr_read_bool)(
	                                 const struct iio_channel *chn,
                                         const char *attr, bool *val);
typedef int (*pfn_iio_channel_attr_write_bool)(
	                                 const struct iio_channel *chn,
                                         const char *attr, bool val);

typedef ssize_t (*pfn_iio_channel_attr_write)(
	                                 const struct iio_channel *chn,
                                         const char *attr,
	                                 const char *src);
typedef int (*pfn_iio_channel_attr_write_longlong)(
	                                 const struct iio_channel *chn,
                                         const char *attr,
	                                 long long val);
typedef int (*pfn_iio_device_attr_write_longlong)(
	                                 const struct iio_device *dev,
                                         const char *attr,
	                                 long long val);
typedef ssize_t (*pfn_iio_device_attr_write_raw)(
	                                 const struct iio_device *dev,
                                         const char *attr,
	                                 const void *src, size_t len);

typedef struct iio_buffer * (*pfn_iio_device_create_buffer)(const struct iio_device *dev,
                                   size_t samples_count, bool cyclic);
typedef int  (*pfn_iio_buffer_set_blocking_mode)(struct iio_buffer *buf,
	                           bool blocking);
typedef void (*pfn_iio_buffer_destroy) (struct iio_buffer *buf);
typedef void (*pfn_iio_context_destroy) (struct iio_context *ctx);
typedef ssize_t (*pfn_iio_buffer_refill) (struct iio_buffer *buf);
typedef ptrdiff_t (*pfn_iio_buffer_start) (const struct iio_buffer *buf);
typedef ptrdiff_t (*pfn_iio_buffer_step) (const struct iio_buffer *buf);
typedef void * (*pfn_iio_buffer_end)(const struct iio_buffer *buf);
typedef void * (*pfn_iio_buffer_first)(const struct iio_buffer *buf,
                                   const struct iio_channel *chn);

struct stream_cfg {
        long long bw_hz;
        long long fs_hz;
        long long lo_hz;
        const char *rfport;
};

class	plutoHandler: public QThread, public deviceHandler,
	                                     public Ui_plutoWidget {
Q_OBJECT
public:
			plutoHandler		(QSettings *, const QString &);
            		~plutoHandler		();
	bool		restartReader		(int32_t);
	void		stopReader		();
	int32_t		getSamples		(std::complex<float> *, int32_t);
	int32_t		Samples			();
	void		resetBuffer		();
	int16_t		bitDepth		();

	QString		deviceName		();
private:
	bool			loadFunctions	();
	QLibrary		*pHandle;
	RingBuffer<std::complex<float>>	_I_Buffer;
	QSettings		*plutoSettings;
	QString			recorderVersion;
	FILE			*xmlDumper;
	xml_fileWriter		*xmlWriter;
	bool			setup_xmlDump	();
	void			close_xmlDump	();
	std::atomic<bool>	dumping;
	bool			filterOn;
	void			run		();
	int32_t			inputRate;
	std::atomic<bool>	running;
	bool			debugFlag;
//      configuration items

	struct	iio_device	*rx;
	struct	iio_context	*ctx;
	struct	iio_channel	*rx0_i;
	struct	iio_channel	*rx0_q;
	struct	iio_buffer	*rxbuf;
	struct stream_cfg	rx_cfg;
	bool			connected;
	std::complex<float>	convBuffer	[CONV_SIZE + 1];
	int			convIndex;
	int16_t			mapTable_int	[DAB_RATE / DIVIDER];
	float			mapTable_float	[DAB_RATE / DIVIDER];

	void			record_gainSettings	(int);
	void			update_gainSettings	(int);
	bool			save_gainSettings;
//
	int			ad9361_set_trx_fir_enable (
                                                 struct iio_device *dev,
	                                         int enable);
	int			ad9361_get_trx_fir_enable (
	                                         struct iio_device *dev,
	                                         int *enable);
	struct iio_device* 	get_ad9361_phy (struct iio_context *ctx);
	bool			get_ad9361_stream_dev (
	                                        struct iio_context *ctx,
                                                enum iodev d,
	                                        struct iio_device **dev);
	bool			get_ad9361_stream_ch (
	                                 __notused struct iio_context *ctx,
                                         enum iodev d,
                                         struct iio_device *dev,
                                         int chid,
                                         struct iio_channel **chn);
	bool			get_phy_chan (struct iio_context *ctx,
                                              enum iodev d,
	                                      int chid,
	                                      struct iio_channel **chn);
	bool			get_lo_chan (struct iio_context *ctx,
                                             enum iodev d,
	                                     struct iio_channel **chn);
	bool		        cfg_ad9361_streaming_ch (
	                                     struct iio_context *ctx,
                                             struct stream_cfg *cfg,
                                             enum iodev type, int chid);


	pfn_iio_device_find_channel
	                        iio_device_find_channel;
	pfn_iio_create_default_context
	                        iio_create_default_context;
	pfn_iio_create_local_context
	                        iio_create_local_context;
	pfn_iio_create_network_context
	                        iio_create_network_context;
	pfn_iio_context_get_name
	                        iio_context_get_name;
	pfn_iio_context_get_devices_count
	                        iio_context_get_devices_count;
	pfn_iio_context_find_device
	                        iio_context_find_device;

	pfn_iio_device_attr_read_bool
	                        iio_device_attr_read_bool;
	pfn_iio_device_attr_write_bool
		                iio_device_attr_write_bool;

	pfn_iio_channel_attr_read_bool
	                        iio_channel_attr_read_bool;
	pfn_iio_channel_attr_write_bool
	                         iio_channel_attr_write_bool;
	pfn_iio_channel_enable
	                        iio_channel_enable;
	pfn_iio_channel_attr_write
	                        iio_channel_attr_write;
	pfn_iio_channel_attr_write_longlong
	                        iio_channel_attr_write_longlong;

	pfn_iio_device_attr_write_longlong
	                        iio_device_attr_write_longlong;

	pfn_iio_device_attr_write_raw
		                iio_device_attr_write_raw;
	pfn_iio_device_create_buffer
	                        iio_device_create_buffer;
	pfn_iio_buffer_set_blocking_mode
	                        iio_buffer_set_blocking_mode;
	pfn_iio_buffer_destroy 
	                        iio_buffer_destroy;
	pfn_iio_context_destroy
	                        iio_context_destroy;
	pfn_iio_buffer_refill
	                        iio_buffer_refill;
	pfn_iio_buffer_step
	                        iio_buffer_step;
	pfn_iio_buffer_end
	                        iio_buffer_end;
	pfn_iio_buffer_first
		                iio_buffer_first;

signals:
	void		new_gainValue		(int);
	void		new_agcValue		(bool);
private slots:
	void		set_gainControl		(int);
	void		set_agcControl		(int);
	void		toggle_debugButton	();
	void		set_filter		();
	void		set_xmlDump		();
};

