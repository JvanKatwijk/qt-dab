#
/*
 *    Copyright (C)   2026
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
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
//
//	the start up sequence consists of two parts,
//	* establishing the connection,
//	* verifying that the samplerate is usable (i.e. convertable to 2048000)
//	if completed we send a signal to the parent with the result

//
//	runMode is set if Qt-DAB issues a restart request,
//	and stopped when a "stop" request is issued
#include	<QJsonDocument>
#include	<QJsonObject>
#include	"message-handler.h"
#include	"dab-constants.h"

static
QString IQstarter	= "{ \"event_type\":\"iq_stream_enable\",\"property\":\"\",\"value\":\"%1\" }";

	messageHandler::messageHandler (const QString &hostAddress,
	                                int	portNumber,
	                                int	startFreq,
	                                RingBuffer<std::complex<float>> *b):
	                                    _I_Buffer (32 * 32768),
	                                    socketHandler (hostAddress,
	                                                   portNumber,
	                                                   &_I_Buffer) {
	_O_Buffer		= b;
	connect (this, &socketHandler::reportConnect,
	         this, &messageHandler::connection_set);
	connect (this, &socketHandler::reportDisconnect,
	         this, &messageHandler::no_connection);
	this	-> vfo_frequency	= startFreq;
	runMode				= false;
	theSamplerate			= 2000000;	// default
}

	messageHandler::~messageHandler	() {
	if (runMode)
	   iqStreamEnable (false);
}

void	messageHandler::connection_set	() {
	disconnect (this, &socketHandler::reportConnect,
	            this, &messageHandler::connection_set);
	connect (this, &socketHandler::dispatchMessage,
                 this, &messageHandler::dispatchMessage);
	connect (this, &socketHandler::binDataAvailable,
                 this, &messageHandler::binDataAvailable);
	setFrequency (vfo_frequency);
	askProperty ("device_sample_rate");
//	emit connection_success	();
}

void	messageHandler::no_connection	() {
	emit connection_failed	();
}
//
//	setFrequency is only used on startup,
//	in operation, the restart/stop functions are used
void	messageHandler::setFrequency	(int freq) {
	setProperty ("device_center_frequency", QString::number (freq));
	setProperty ("device_vfo_frequency", QString::number (freq));
	this -> vfo_frequency	= freq;
}

bool	messageHandler::restartReader	(int32_t freq, int skip) {
	(void)skip;
	setFrequency (freq);
	convIndex. store (0);		// reset conversionpointer
	runMode	= true;
	return true;
}

void	messageHandler::stopReader	() {
	runMode = false;
}

void	messageHandler::iqStreamEnable	(bool b) {
	sendMessage (IQstarter. arg (b ? "true" : "false"));
}

int32_t	messageHandler::getVFOFrequency	() {
	return vfo_frequency;
}
//
//	Transfer is in segments of 1 msec
void	messageHandler::binDataAvailable () {
	std::complex<int16_t>  *inBuffer = dynVec (std::complex<int16_t>,
	                                            theSamplerate / 1000);
	std::complex<float> *outBuffer	= dynVec (std::complex<float>,
	                                            SAMPLERATE / 1000);
	while (_I_Buffer. GetRingBufferReadAvailable () >=
	                                 (uint32_t)theSamplerate / 1000) {
	   _I_Buffer. getDataFromBuffer (inBuffer, theSamplerate / 1000);
	   if (!runMode)	// only deal with data when processing is on
	      continue;
	   if (theSamplerate == SAMPLERATE) {
	      for (int i = 0; i < SAMPLERATE / 1000; i ++)
	         outBuffer [i] =
	          std::complex<float> (real (inBuffer [i]) / 2048.0,
	                               imag (inBuffer [i]) / 2048.0);
	      _O_Buffer -> putDataIntoBuffer (outBuffer, SAMPLERATE / 1000);
	   }
	   else 
	   for (int i = 0; i < theSamplerate / 1000; i ++) {
	      convBuffer [convIndex. load ()] =
	                std::complex<float> (real (inBuffer [i]) / 2048.0,
	                                     imag (inBuffer [i]) / 2048.0);
	      convIndex. store (convIndex. load () + 1);
	      if (convIndex. load () > convBufferSize) {
	         for (int j = 0; j < SAMPLERATE / 1000; j ++) {
                    int16_t  inpBase    = mapTable_int [j];
                    float    inpRatio   = mapTable_float [j];
                    outBuffer [j]    = convBuffer [inpBase + 1] * inpRatio +
                                       convBuffer [inpBase] * (1 - inpRatio);
	         }
                 _O_Buffer ->  putDataIntoBuffer (outBuffer,SAMPLERATE / 1000);
                 convBuffer [0] = convBuffer [convBufferSize];
                 convIndex. store (1);
              }
	      if (_O_Buffer -> GetRingBufferReadAvailable () > SAMPLERATE / 10)
	         emit dataAvailable (_O_Buffer -> GetRingBufferReadAvailable ());
	   }
	}
}
//
//	messages sent by the SDRconnect are dispatched here
void	messageHandler::dispatchMessage	(const QString &m) {
	QJsonObject obj;
	QJsonDocument doc = QJsonDocument::fromJson (m. toUtf8 ());
	if (doc. isNull ())
	   return;	// cannot handle
	obj	= doc. object ();

	QString eventType	= obj ["event_type"]. toString ();
	if (eventType == "get_property_response") {
	   QString property = obj ["property"]. toString ();
	   if (property == "device_sample_rate") {
	      QString samplerate = obj ["value"]. toString ();
	      bool b;
	      double rate	= samplerate. toDouble (&b);
	      if (!b)
	         return;
//
//	we expect 2000000 and do not process (much) lower/higher rates
	      if ((rate > 2500000) || (rate < 1500000)) {
	         emit rateError ();
	         return;
	      }
	      else {
	         setProperty ("filter_bandwidth", "153600");
	         theSamplerate	= (int)rate;
//	we process  buffers with 1 msec content
	         convBufferSize          = theSamplerate / 1000;
	         float samplesPerMsec    = SAMPLERATE / 1000.0;
	         for (int i = 0; i < SAMPLERATE / 1000; i ++) {
	            float inVal  = float (theSamplerate / 1000);
	            mapTable_int [i]     =
	                      int (floor (i * (inVal / samplesPerMsec)));
	            mapTable_float [i]   =
	                      i * (inVal / samplesPerMsec) - mapTable_int [i];
	         }       
	         convIndex. store (0);
	         convBuffer. resize (convBufferSize + 1);
	      }
	      emit rateOK ();
	      iqStreamEnable (true);
	   }
	}
	if (eventType == "property_changed") {
	   QString property = obj ["property"]. toString ();
	   if (property == "device_vfo_frequency") {
	      QString vfoString = obj ["value"]. toString ();
	      bool b;
	      int vfo	= vfoString. toInt (&b);
	      if (!b)
	         return;
	      vfo_frequency	= vfo;
	      emit frequency_changed (vfo);
	   }
	   if (property == "signal_power") {
	      QString snrString = obj ["value"]. toString ();
	      QString res;
	      for (int i = 0; i < snrString. size (); i ++)
                  if (snrString. at (i) == QChar (','))
                     res. push_back (QChar ('.'));
                  else
                     res. push_back (snrString. at (i));
	      bool b;
	      double snr = res. toDouble (&b);
	      if (!b)
	         return;
	      emit signalPower (snr);
	   }
	}
}

void	messageHandler::setProperty (const QString prop, const QString val) {
QString message = "{ \"event_type\":\"set_property\",\"property\":\"%1\",\"value\":\"%2\" }";
	sendMessage (message. arg (prop).arg (val));
}

void	messageHandler::askProperty (const QString prop) {
QString  message = "{ \"event_type\":\"get_property\",\"property\":\"%1\" }";
	sendMessage (message. arg (prop));
}

