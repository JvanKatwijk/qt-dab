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

#include	"spyserver-client-8.h"
#include	"spy-handler-8.h"
#include	<chrono>
#include	<iostream>
#include	<thread>

	spyHandler_8::spyHandler_8 (spyServer_client_8 *parent,
	                            const QString &ipAddress,
	                            int port,
	                            RingBuffer<uint8_t> *outB) :
	                               inBuffer (64 * 32768),
	                               tcpHandler (ipAddress, port, &inBuffer) {
	if (!tcpHandler. is_connected ())
	   throw std::runtime_error( std::string(__FUNCTION__) + " " +
                                 "Failed to connect!" );

	this	-> parent	= parent;
	this	-> outB		= outB;
	streamingMode	= STREAM_TYPE_IQ;
	streaming. store (false);
	running. store (false);
	bool success = show_attendance ();
	if (!success)
	   throw std::runtime_error( std::string(__FUNCTION__) + " " +
                                 "Failed to establish connection!" );
	is_connected. store (true);
	cleanRecords ();
	testTimer	= new QTimer ();
	connect (testTimer, &QTimer::timeout,
	           this, &spyHandler_8::no_deviceInfo);
	connect (this, &spyHandler_8::data_ready,
	           parent, &spyServer_client_8::data_ready);
	start ();
	testTimer	-> start (10000);
}

	spyHandler_8::~spyHandler_8	() {
	running. store (false);
	while (isRunning ())
	   usleep (1000);
}

void	spyHandler_8::no_deviceInfo	() {
	if (!got_device_info)
	   running. store (false);
}

void	spyHandler_8::run		() {
struct MessageHeader	theHeader;
uint64_t volgNummer = 0;
static std::vector<uint8_t> buffer (64 * 1024);
	running. store (true);
	while (running. load ()) {
	   readHeader	(theHeader);
	   if (theHeader. SequenceNumber != volgNummer + 1) {
	      fprintf (stderr, "%d %ld\n",
	                  (int)theHeader. SequenceNumber, volgNummer);
//	      fprintf (stderr, "Buffer space = %d\n",
//	               inBuffer. GetRingBufferReadAvailable ());
	   }

	   volgNummer = theHeader. SequenceNumber;
	   if (theHeader. BodySize > buffer. size ())
	      buffer. resize (theHeader. BodySize);
	   readBody (buffer. data (), theHeader. BodySize);
	   switch (theHeader. MessageType) {
	      case MSG_TYPE_DEVICE_INFO:
	         got_device_info = process_device_info (buffer. data (),
	                                                     deviceInfo);
	         break;
	      case MSG_TYPE_UINT8_IQ:
	         process_data (buffer. data (), theHeader. BodySize);
	         break;
	      case MSG_TYPE_CLIENT_SYNC:
	         process_client_sync (buffer. data (), m_curr_client_sync);
	         break;
	   }
	}
}

bool	spyHandler_8::readHeader	(struct MessageHeader &header) {
	while (running. load () &&
	       (inBuffer. GetRingBufferReadAvailable () <
	                             (int)sizeof (struct MessageHeader)))
	    std::this_thread::sleep_for (std::chrono::milliseconds (1));
	if (!running. load ())
	   return false;
	inBuffer. getDataFromBuffer ((uint8_t *)(&header),
	                              sizeof (struct MessageHeader));
	return true;
}

bool	spyHandler_8::readBody	(uint8_t *buffer, int size) {
int	filler = 0;
	while (running. load ()) {
	   if (inBuffer. GetRingBufferReadAvailable () >  size / 2) {
	      filler += inBuffer. getDataFromBuffer (buffer, size - filler);
	      if (filler >= size)
	         return true;
	      if (!running. load ())
	         return false;
	   }
	   else
	      std::this_thread::sleep_for(std::chrono::milliseconds (1));
	}
	return false;
}

bool	spyHandler_8::show_attendance () {
const uint8_t *protocolVersionBytes =
	                      (const uint8_t *) &ProtocolVersion;
const uint8_t *softwareVersionBytes =
	                      (const uint8_t *) SoftwareID. c_str();
std::vector<uint8_t> args =
	                      std::vector<uint8_t>(sizeof(ProtocolVersion) +
	                                                SoftwareID.size());

	std::memcpy (&args[0], protocolVersionBytes, sizeof(ProtocolVersion));
	std::memcpy (&args[0] + sizeof(ProtocolVersion),
	                         softwareVersionBytes, SoftwareID.size());
	bool res =  send_command (CMD_HELLO, args);
	return res;
}
	
void	spyHandler_8::cleanRecords () {
	deviceInfo. DeviceType		= 0;
	deviceInfo. DeviceSerial		= 0;
	deviceInfo. DecimationStageCount = 0;
	deviceInfo. GainStageCount	= 0;
	deviceInfo. MaximumSampleRate	= 0;
	deviceInfo. MaximumBandwidth	= 0;
	deviceInfo. MaximumGainIndex	= 0;
	deviceInfo. MinimumFrequency	= 0;
	deviceInfo. MaximumFrequency	= 0;
	frameNumber			= 0;
}

bool	spyHandler_8::send_command (uint32_t cmd, std::vector<uint8_t> &args) {
bool result;
uint32_t headerLen	= sizeof (CommandHeader);
uint16_t argLen		= args.size();
uint8_t buffer [headerLen + argLen];
CommandHeader header;

//	if (!is_connected) {
//	   return false;
//	}

//	for (int i = 0; i < args. size (); i ++)
//	   fprintf (stderr, "%x ", args [i]);
//	fprintf (stderr, "\n");
	header. CommandType = cmd;
	header. BodySize = argLen;

	for (uint32_t i = 0; i < sizeof (CommandHeader); i++) {
	   buffer [i] = ((uint8_t *)(&header))[i];
	}

	if (argLen > 0) {
	   for (uint16_t i = 0; i < argLen; i++) {
	      buffer [headerLen + i] = args [i];
	   }
	}

	try {
	   tcpHandler. send_data (buffer, headerLen+argLen);
	   result = true;
	} catch (std::exception &e) {
	   std::cerr << "caught exception while sending command.\n";
	   result = false;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return result;
}

bool	spyHandler_8::process_device_info (uint8_t *buffer,
	                                        DeviceInfo &deviceInfo) {
	std::memcpy (&deviceInfo, buffer, sizeof(DeviceInfo));
	std::cerr << "\n**********\nDevice Info:" 
	        << "\n   Type:                 " << deviceInfo.DeviceType
	        << "\n   Serial:               " << deviceInfo.DeviceSerial
	        << "\n   MaximumSampleRate:    " << deviceInfo.MaximumSampleRate
	        << "\n   MaximumBandwidth:     " << deviceInfo.MaximumBandwidth
	        << "\n   DecimationStageCount: " << deviceInfo.DecimationStageCount
	        << "\n   GainStageCount:       " << deviceInfo.GainStageCount
	        << "\n   MaximumGainIndex:     " << deviceInfo.MaximumGainIndex
	        << "\n   MinimumFrequency:     " << deviceInfo.MinimumFrequency
	        << "\n   MaximumFrequency:     " << deviceInfo.MaximumFrequency
	        << "\n   Resolution:           " << deviceInfo.Resolution
	        << "\n   MinimumIQDecimation:  " << deviceInfo.MinimumIQDecimation
	        << "\n   ForcedIQFormat:       " << deviceInfo.ForcedIQFormat
	        << std::endl;  
	return true;
}

bool	spyHandler_8::process_client_sync (uint8_t *buffer,
	                                         ClientSync &client_sync) {

	std::memcpy ((void *)(&client_sync), buffer, sizeof (ClientSync));
	std::memcpy ((void *)(&m_curr_client_sync), buffer, sizeof (ClientSync));
	_gain		= (double) m_curr_client_sync. Gain;
	_center_freq	= (double) m_curr_client_sync.IQCenterFrequency;
	std::cerr << "\n**********\nClient sync:"
            << std::dec
            << "\n   Control:     " << (m_curr_client_sync.CanControl == 1 ? "Yes" : "No")
            << "\n   gain:        " << m_curr_client_sync.Gain
            << "\n   dev_ctr:     " << m_curr_client_sync.DeviceCenterFrequency
            << "\n   ch_ctr:      " << m_curr_client_sync.IQCenterFrequency
            << "\n   iq_ctr:      " << m_curr_client_sync.IQCenterFrequency
            << "\n   fft_ctr:     " << m_curr_client_sync.FFTCenterFrequency            
            << "\n   min_iq_ctr:  " << m_curr_client_sync.MinimumIQCenterFrequency            
            << "\n   max_iq_ctr:  " << m_curr_client_sync.MaximumIQCenterFrequency            
            << "\n   min_fft_ctr: " << m_curr_client_sync.MinimumFFTCenterFrequency            
            << "\n   max_fft_ctr: " << m_curr_client_sync.MaximumFFTCenterFrequency            
            << std::endl;
//	emit call_parent ();
	parent	-> connect_on ();
	fprintf (stderr, "calling the parent to set up connection\n");
	got_sync_info = true;
	return true;
}

bool	spyHandler_8::get_deviceInfo (struct DeviceInfo &theDevice) {
	if (!got_device_info)
	   return false;

	theDevice. DeviceType		= deviceInfo.DeviceType;
	theDevice. DeviceSerial		= deviceInfo.DeviceSerial;
	theDevice. DecimationStageCount =
	                          deviceInfo.DecimationStageCount;
	theDevice. GainStageCount	= deviceInfo.GainStageCount;
	theDevice. MaximumSampleRate	= deviceInfo.MaximumSampleRate;
	theDevice. MaximumBandwidth	= deviceInfo.MaximumBandwidth;
	theDevice. MaximumGainIndex	= deviceInfo.MaximumGainIndex;
	theDevice. MinimumFrequency	= deviceInfo.MinimumFrequency;
	theDevice. MaximumFrequency	= deviceInfo.MaximumFrequency;
	return true;
}

bool	spyHandler_8::set_sample_rate_by_decim_stage (const uint32_t stage) {
std::vector<uint32_t> p (1);
std::vector<uint32_t> q (1);
	p [0] = stage;
	set_setting (SETTING_IQ_DECIMATION, p);
	q [0] = STREAM_FORMAT_UINT8;
	set_setting (SETTING_IQ_FORMAT, q);
	return true;
}

double	spyHandler_8::get_sample_rate	() {
	return 2500000;
}

bool	spyHandler_8::set_iq_center_freq	(double centerFrequency) {
std::vector<uint32_t> param (1);
	param [0] = centerFrequency;
	set_setting (SETTING_IQ_FREQUENCY, param);
	param [0] = STREAM_FORMAT_UINT8;
	set_setting (SETTING_IQ_FORMAT, param);
	return true;
}

bool	spyHandler_8::set_gain_mode	(bool automatic, size_t chan) {
	(void)automatic;
	(void)chan;
	return 0;
}

bool	spyHandler_8::set_gain		(double gain) {
std::vector<uint32_t> param (1);
	param [0] = (uint32_t)gain;
	set_setting (SETTING_GAIN, param);
	return true;
}

bool	spyHandler_8::is_streaming	() {
	return streaming. load ();
}

void	spyHandler_8::start_running	() {
std::vector<uint32_t> p (1);
	if (!streaming. load ()) {
	   std::cerr << "spyHandler: Starting Streaming" << std::endl;
	   streaming. store (true);
	   p [0] =  1;
	   set_setting (SETTING_STREAMING_ENABLED, p);
	}
}

void	spyHandler_8::stop_running	() {
std::vector<uint32_t> p;
	if (streaming. load ()) {
	   streaming. store (false);
	   p. push_back (0);
	   set_setting (SETTING_STREAMING_ENABLED, p);
	}
}

bool	spyHandler_8::set_setting (uint32_t settingType,
	                           std::vector<uint32_t> &params) {
std::vector<uint8_t> argBytes;

	if (params.size () > 0) {
	   argBytes = std::vector<uint8_t> (sizeof(SettingType) +
	                                    params.size() * sizeof (uint32_t));
	   uint8_t *settingBytes = (uint8_t *) &settingType;
	   for (uint32_t i = 0; i < sizeof (uint32_t); i++) {
	      argBytes [i] = settingBytes [i];
	   }

	   std::memcpy (&argBytes [0] + sizeof (uint32_t),
	                &params[0], sizeof(uint32_t) * params.size());
	} else {
	   argBytes = std::vector<uint8_t>();
	}

	return send_command (CMD_SET_SETTING, argBytes);
}

void	spyHandler_8::process_data (uint8_t *theBody, int length) {
	outB ->  putDataIntoBuffer (theBody, length);
	emit data_ready ();
}

void	spyHandler_8::connection_set () {
std::vector<uint32_t> p;
	p. push_back (streamingMode);
	set_setting (SETTING_STREAMING_MODE, p);
	p [0] = 0x0;
	set_setting (SETTING_IQ_DIGITAL_GAIN, p);
	p [0] = STREAM_FORMAT_UINT8;
	set_setting (SETTING_IQ_FORMAT, p);
//	fprintf (stderr, "Connection is gezet, waar blijft de call?\n");
}

bool	spyHandler_8::isFileInput	() {
	return true;
}

QString	spyHandler_8::deviceName	() {
	return "spy-server-8Bits :";
}

