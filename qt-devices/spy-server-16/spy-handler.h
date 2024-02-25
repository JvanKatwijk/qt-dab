#
/*
 *    Copyright (C) 2016 .. 2023
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
#include	<stdexcept>
#include	<atomic>
#include	<chrono>
#include	<QThread>
#include	<QTimer>
#include	<QString>
#include	"tcp-client.h"
#include	"spyserver-protocol.h"
class	spyServer_client;
#include	"spy-handler.h"
#include	"ringbuffer.h"

class spyHandler:   public QThread {
Q_OBJECT
public:
		spyHandler		(spyServer_client *,
	                                 const QString &, int,
	                                 RingBuffer<int16_t> *outB);
		~spyHandler		();
	bool	get_deviceInfo		(struct DeviceInfo &theDevice);
	int	get_iq_data		(const int size,
	                                       int16_t *outputItems);
	bool	set_sample_rate_by_decim_stage (const uint32_t decim_stage);
	double	get_sample_rate		();
	bool	set_iq_center_freq	(double freq);
	bool	set_gain_mode		(bool automatic, size_t chan = 0);
	bool	set_gain		(double gain);
	bool	is_streaming		();
	void	start_running		();
	void	stop_running		();

	void	connection_set		();
	bool	isFileInput		();
private:
	RingBuffer<uint8_t>	inBuffer;
	tcp_client	tcpHandler;
	RingBuffer<int16_t>	*outB;
	spyServer_client	*parent;
	void		run		();
	bool	process_device_info	(uint8_t *, struct DeviceInfo &);
	bool	process_client_sync	(uint8_t *, ClientSync &);
	void	cleanRecords		();
	bool	show_attendance		();
	bool	readHeader	(struct MessageHeader &);
	bool	readBody	(uint8_t *, int);
	void	process_data	(uint8_t *, int);
	bool	send_command	(uint32_t, std::vector<uint8_t> &);
	bool	set_setting	(uint32_t, std::vector<uint32_t> &);

	QTimer	*testTimer;
	int	streamingMode;
	std::atomic<bool>	is_connected;
	std::atomic<bool>	streaming;
	std::atomic<bool>	running;
	std::atomic<bool>	got_device_info;
	std::atomic<bool>	got_sync_info;

	DeviceInfo		deviceInfo;
	ClientSync		m_curr_client_sync;
	MessageHeader		header;
	uint64_t		frameNumber;

	double			_center_freq;
	double			_gain;

	
	const uint32_t ProtocolVersion = SPYSERVER_PROTOCOL_VERSION;
	const std::string SoftwareID = std::string("gr-osmosdr");
	const std::string NameNoDevice = std::string("SpyServer - No Device");
	const std::string NameAirspyOne = std::string("SpyServer - Airspy One");
	const std::string NameAirspyHF = std::string("SpyServer - Airspy HF+");
	const std::string NameRTLSDR = std::string("SpyServer - RTLSDR");
	const std::string NameUnknown = std::string("SpyServer - Unknown Device");

signals:
	void	call_parent		();
	void	data_ready		();
private slots:
	void	no_deviceInfo		();
};

