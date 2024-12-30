#
/*
 *    Copyright (C) 2014 .. 2022
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
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"ofdm-handler.h"

#include	<utility>
#include	"radio.h"
#include	"dab-constants.h"
#include	"process-params.h"
#include	"dab-params.h"
#include	"timesyncer.h"
#include	"freqsyncer.h"
#include	"ringbuffer.h"
#include	"estimator.h"
#include	"correlator.h"
#include	"logger.h"
#include	"settingNames.h"
#include	"settings-handler.h"
//

/**
  *	\brief ofdmHandler
  *	The ofdmHandler class is the driver of the processing
  *	of the samplestream.
  *	It is the main interface to the "radio" program, that
  *	controls the GUI and the various processing elements
  */

	ofdmHandler::ofdmHandler	(RadioInterface	*mr,
	                                 deviceHandler	*inputDevice,
	                                 processParams	*p,
	                                 QSettings	*dabSettings,
	                                 logger		*theLogger):
	                                    radioInterface_p (mr),
	                                    params (p -> dabMode),
	                                    settings_p (dabSettings),
	                                    theReader (mr,
	                                              inputDevice,
	                                              p -> spectrumBuffer),
	                                    theFicHandler (mr, p -> dabMode),
	                                    theEtiGenerator (p -> dabMode,
	                                                  &theFicHandler),
	                                    theOfdmDecoder (mr,
	                                                 p -> dabMode,
	                                                 inputDevice -> bitDepth(),
	                                                 p -> stdDevBuffer,
	                                                 p -> iqBuffer),
	                                    theMscHandler (mr, p -> dabMode,
	                                                p -> frameBuffer,
	                                                theLogger) {

	this	-> p			= p;
	this	-> theLogger		= theLogger;
	this	-> threshold		= p -> threshold;
	this	-> tiiBuffer_p		= p -> tiiBuffer;
	this	-> nullBuffer_p		= p -> nullBuffer;
	this	-> snrBuffer_p		= p -> snrBuffer;
	this	-> channelBuffer_p	= p -> channelBuffer;
	this	-> T_null		= params. get_T_null ();
	this	-> T_s			= params. get_T_s ();
	this	-> T_u			= params. get_T_u ();
	this	-> T_g			= T_s - T_u;
	this	-> T_F			= params. get_T_F ();
	this	-> nrBlocks		= params. get_L ();
	this	-> carriers		= params. get_carriers ();
	this	-> carrierDiff		= params. get_carrierDiff ();

	this	-> tii_delay		= p -> tii_delay;
	this	-> tii_counter		= 0;
	this	-> correlationOrder	= 
	         value_i (dabSettings, CONFIG_HANDLER,
	                             S_CORRELATION_ORDER, 0) != 0;
	this	-> dxMode		=
	         value_i (dabSettings, CONFIG_HANDLER, S_DX_MODE, 0) != 0;

	this	-> decoder		= value_i (dabSettings, CONFIG_HANDLER, 
	                                           "decoders", DECODER_1); 

	this	-> eti_on		= false;
	ofdmBuffer. resize (2 * T_s);
	fineOffset			= 0;	
	coarseOffset			= 0;	
	correctionNeeded		= true;
	attempts			= 0;

	goodFrames			= 0;
	badFrames			= 0;
	totalFrames			= 0;
	scanMode			= false;

	connect (this, &ofdmHandler::set_synced,
	         radioInterface_p, &RadioInterface::set_synced);
	connect (this, &ofdmHandler::set_sync_lost,
	         radioInterface_p, &RadioInterface::set_sync_lost);
	connect (this, &ofdmHandler::show_tiiData,
	         radioInterface_p, &RadioInterface::show_tiiData);
	connect (this, &ofdmHandler::show_tii_spectrum,
	         radioInterface_p, &RadioInterface::show_tii_spectrum);
	connect (this, static_cast<void (ofdmHandler::*)(float)>(&ofdmHandler::show_snr),
	         mr, &RadioInterface::show_snr);
	connect (this, &ofdmHandler::show_clock_error,
	         mr,  &RadioInterface::show_clock_error);
	connect (this, &ofdmHandler::show_null,
	         mr, &RadioInterface::show_null);
//	for older versions, this is a dummy
	connect (this, &ofdmHandler::show_channel,
	         mr, &RadioInterface::show_channel);
//	end of dummy
	connect (this, &ofdmHandler::show_Corrector,
	         mr,  &RadioInterface::show_Corrector);
	tiiThreshold = value_i (settings_p, CONFIG_HANDLER,
                                             TII_THRESHOLD, 6);
	tiiCollisions_active = value_i (settings_p, CONFIG_HANDLER,
	                                      "tiiCollisions", 1) != 0;
	tiiFilter_active = value_i (settings_p, CONFIG_HANDLER,
	                                      "tiiFilter", 1) != 0;
//	theTIIDetector. reset();
	theOfdmDecoder. handle_decoderSelector (decoder);
}

	ofdmHandler::~ofdmHandler () {
	   if (isRunning()) {
	      theReader. setRunning (false);
	                                // exception to be raised
	                        	// through the getSample(s) functions.
	      msleep (100);
	      while (isRunning()) {
	         usleep (100);
	   }
	}
}

void	ofdmHandler::set_tiiThreshold	(int16_t threshold) {
	tiiThreshold = threshold;
}

void	ofdmHandler::set_tiiCollisions	(bool b) {
	tiiCollisions_active = b;
}

void	ofdmHandler::set_tiiFilter	(bool b) {
	tiiFilter_active	= b;
}

void	ofdmHandler::start () {
	theFicHandler. restart	();
	transmitters. clear ();
	theOfdmDecoder. reset	();
	theFicHandler.  restart	();
	if (!scanMode)
	   theMscHandler. reset_Channel ();
	QThread::start ();
}

void	ofdmHandler::stop	() {
	theReader. setRunning (false);
	while (isRunning ())
	   wait ();
	usleep (10000);
	theFicHandler. stop ();
}
/***
   *	\brief run
   *	The main thread, reading samples,
   *	time synchronization and frequency synchronization
   *	Identifying blocks in the DAB frame
   *	and sending them to the ofdmDecoder who will transfer the results
   *	Finally, estimating the small frequency error
   */
void	ofdmHandler::run	() {
int32_t		startIndex;
timeSyncer	myTimeSyncer (&theReader);
phaseTable	theTable (p -> dabMode);
TII_Detector	theTIIDetector (p -> dabMode, &theTable);
freqSyncer	myFreqSyncer (radioInterface_p, p, &theTable);
estimator	myEstimator  (radioInterface_p, p, &theTable);
correlator	myCorrelator (radioInterface_p, p, &theTable);
std::vector<int16_t> ibits;
int	frameCount	= 0;
int	sampleCount	= 0;
int	totalSamples	= 0;
int	cCount		= 0;
float	snr		= 0;
bool	inSync		= false;
QVector<Complex> tester (T_u / 2);
int	snrCount	= 0;
	ibits. resize (2 * params. get_carriers());
	fineOffset		= 0;
	coarseOffset		= 0;
	correctionNeeded	= true;
	attempts		= 0;
	theReader. setRunning (true);	// useful after a restart
//
//	to get some idea of the signal strength
	try {
	   const int tempSize = 128;
	   std::vector<Complex> temp (tempSize);
	   for (int i = 0; i < T_F / (5 * tempSize); i ++) {
	      theReader. get_samples (temp, 0, tempSize, 0, true);
	   }

	   while (true) {
	      if (!inSync) {
	         totalFrames ++;
	         totalSamples	= 0;
	         frameCount	= 0;
	         sampleCount	= 0;

	         set_synced (false);
	         theTIIDetector. reset ();
	         switch (myTimeSyncer. sync (T_null, T_F)) {
	            case TIMESYNC_ESTABLISHED:
	               inSync	= true;
	               set_synced (true);
	               break;			// yes, we are ready

	            case NO_DIP_FOUND:
	               if (++ attempts >= 8) {
	                  emit (no_signal_found());
	                  attempts = 0;
	               }	
	               continue;

	            default:			// does not happen
	            case NO_END_OF_DIP_FOUND:
	               continue;
	         }

	         theReader. get_samples (ofdmBuffer, 0,
	                        T_u, coarseOffset + fineOffset, false);
	         startIndex = myCorrelator. findIndex (ofdmBuffer,
	                                               correlationOrder,
	                                               threshold);

	         if (startIndex < 0) { // no sync, try again
	            if (!correctionNeeded) {
	               set_sync_lost();
	            }
	            badFrames ++;
	            set_synced (false);
	            inSync	= false;
	            continue;
	         }
	         sampleCount	= startIndex;
	      }
	      else {	// we are in sync and continue with a next frame
	         totalFrames ++;
	         frameCount ++;
	         bool null_shower	= false;
	         totalSamples	+= sampleCount;
	         if (frameCount >= 10) {
	            int diff	= (totalSamples - frameCount * T_F);
	            diff	= (int)((float)INPUT_RATE / (frameCount * T_F) * diff);
	            show_clock_error (diff);
	            totalSamples = 0;
	            frameCount	= 0;
	            null_shower = true;
	            for (int i = 0; i < T_u / 4; i ++)
	               tester [i] = ofdmBuffer [T_null - T_u / 4 + i];
	         }

	         theReader. get_samples (ofdmBuffer, 0,
	                               T_u, coarseOffset + fineOffset, false);

	         startIndex = myCorrelator. findIndex (ofdmBuffer,
	                                               correlationOrder,
	                                               2.5 * threshold);
	         if (null_shower) {
	            for (int i = 0; i < T_u / 4; i ++)
	               tester [T_u / 4 + i] = ofdmBuffer [i];
	            nullBuffer_p -> putDataIntoBuffer (tester. data (),
	                                                       T_u / 2);
	            show_null (T_u / 2, startIndex);
	         }
	         if (startIndex < 0) { // no sync, try again
	            if (!correctionNeeded) {
	               set_sync_lost();
	            }
	            badFrames	++;
	            inSync	= false;
	            set_synced (false);
	            continue;
	         }
	         sampleCount = startIndex;
	      }

	      goodFrames ++;
	      double cLevel	= 0;
	      cCount	= 0;
	      memmove (ofdmBuffer. data (),
	               &((ofdmBuffer. data()) [startIndex]),
	                  (T_u - startIndex) * sizeof (Complex));
	      int ofdmBufferIndex	= T_u - startIndex;

//Block_0:
/**
  *	Block 0 is special in that it is used for fine time synchronization,
  *	for coarse frequency synchronization
  *	and its content is used as a reference for decoding the
  *	first datablock.
  *	We read the missing samples in the ofdm buffer
  */
	      theReader. get_samples (ofdmBuffer,
	                              ofdmBufferIndex,
	                              T_u - ofdmBufferIndex,
	                              coarseOffset + fineOffset, true);
	      static int abc = 0;
	      if (radioInterface_p -> channelOn ()) {
	         if (++abc > 10) { 
	            std::vector<Complex> result;
	            myEstimator. estimate (ofdmBuffer, result);
	            if (channelBuffer_p != nullptr) {
	               channelBuffer_p -> putDataIntoBuffer (result. data (),
	                                                   result. size ());
	               emit show_channel (result. size ());
	            }
	            abc = 0;
	         }
	      }
	      sampleCount	+= T_u;
	      bool frame_with_TII = 
	                   (p -> dabMode == 1) &&
	                     ((theFicHandler. get_CIFcount () & 0x7) == 0);
	      (void) theOfdmDecoder. processBlock_0 (ofdmBuffer,
	                                             frame_with_TII);
#ifdef	__MSC_THREAD__
	      if (!scanMode)
	         theMscHandler.  processBlock_0 (ofdmBuffer. data());
#endif

//	Here we look only at the block_0 when we need a coarse
//	frequency synchronization.
	      if (!correctionNeeded && !theFicHandler. syncReached ())
	         correctionNeeded	= true;
	      if (correctionNeeded) {
	         int correction	=
	            myFreqSyncer. estimate_CarrierOffset (ofdmBuffer);
	         if (correction != 100) {
	            coarseOffset	+= 0.4 * correction * carrierDiff;
	            if (abs (coarseOffset) > Khz (35))
	               coarseOffset = 0;
	         }
	      }
/**
  *	after block 0, we will just read in the other
  *	(params -> L - 1) blocks
  */
//Data_blocks:
/**
  *	The first ones are the FIC blocks these are handled within
  *	the thread executing this "task", the other blocks
  *	are passed on to be handled in the mscHandler, running
  *	in a different thread.
  *	We immediately start with building up an average of
  *	the phase difference between the samples in the cyclic prefix
  *	and the	corresponding samples in the datapart.
  */
	      cCount	= 0;
	      cLevel	= 0;
	      Complex FreqCorr	= Complex (0, 0);
	      for (int ofdmSymbolCount = 1;
	           ofdmSymbolCount < nrBlocks; ofdmSymbolCount ++) {
	         theReader. get_samples (ofdmBuffer, 0,
	                                 T_s, coarseOffset + fineOffset, true);
	         sampleCount += T_s;
	         for (int i = (int)T_u; i < (int)T_s; i ++) {
	            FreqCorr +=
	                      ofdmBuffer [i] * conj (ofdmBuffer [i - T_u]);
	            cLevel += abs (ofdmBuffer [i]) + abs (ofdmBuffer [i - T_u]);
	         }
	         cCount += 2 * T_g;
//
//
//	If "eti_on" we process all data here
	         if (eti_on) {
	            theOfdmDecoder.
	                   decode (ofdmBuffer, ofdmSymbolCount, ibits);
	            if (ofdmSymbolCount <= 3) 
	               theFicHandler.
	                    process_ficBlock (ibits, ofdmSymbolCount);
	            else 
	               theMscHandler. process_mscBlock (ibits, ofdmSymbolCount);
	            theEtiGenerator. processBlock (ibits, ofdmSymbolCount);
	            continue;
	         }
//
//	Normal Processing, no eti 
//	we distinguish vetween processing everything in this thread, or
//	delegate processing of the data blocks in the MSC thread
//	Of course, if scanning is ON, then we do not process
//	the payload at all
	         if (ofdmSymbolCount <= 3) {
	            theOfdmDecoder.
                           decode (ofdmBuffer, ofdmSymbolCount, ibits);
	            theFicHandler.   
                            process_ficBlock (ibits, ofdmSymbolCount);
	         }
	         if (scanMode)
	            continue;
#ifdef	__MSC_THREAD__
	         theMscHandler. process_Msc (ofdmBuffer, 0,  ofdmSymbolCount);
#else
	         if (ofdmSymbolCount >= 4) {
	            theOfdmDecoder.
	                    decode (ofdmBuffer, ofdmSymbolCount, ibits);
	            theMscHandler.
	                    process_mscBlock (ibits, ofdmSymbolCount);
	         }
#endif
	      }
/**
  *	OK,  here we are at the end of the frame
  *	Assume everything went well and skip T_null samples
  */
	      theReader. get_samples (ofdmBuffer, 0,
	                         T_null, coarseOffset + fineOffset, false);
	      sampleCount += T_null;
//
//	The snr is computed, where we take as "noise" the signal strength
//	of the NULL period (the one without TII data)

	      if (frame_with_TII) {
	         theTIIDetector. addBuffer (ofdmBuffer);
	         if (++tii_counter >= tii_delay) {
	            tiiBuffer_p -> putDataIntoBuffer (ofdmBuffer. data(),
	                                                          T_u);
	            show_tii_spectrum ();
	            QVector<tiiData> resVec =
	                           theTIIDetector. processNULL (tiiThreshold,
	                                                 tiiCollisions_active,
	                                                 tiiFilter_active);
	            show_tiiData (resVec, 0);
	            tii_counter = 0;
//	            theTIIDetector. reset();
	         }
	      }
	      else {	// compute SNR
	         float sum	= 0;
	         for (int i = 0; i < T_null; i ++)
	            sum += abs (ofdmBuffer [i]);
	         sum /= T_null;
	         float snrV	=
	              20 * log10 ((cLevel / cCount + 0.005) / (sum + 0.005));
	         snr = 0.9 * snr + 0.1 * snrV;
	         if (this -> snrBuffer_p != nullptr) 
	            snrBuffer_p -> putDataIntoBuffer (&snr, 1);
	         snrCount ++;
	         if (snrCount >= 3) {
	            snrCount = 0;
	            show_snr (snr);
	         }
	      }
/**
  *	The first sample to be found for the next frame should be T_g
  *	samples ahead. Before going for the next frame, we
  *	we just check the fineCorrector
  */
//NewOffset:
//     we integrate the newly found frequency error with the
//     existing frequency error.

	      int oldCoarseOffset	= coarseOffset;
	      fineOffset += 0.1 * arg (FreqCorr) / (2 * M_PI) * carrierDiff;
	      if (fineOffset > carrierDiff / 2) {
	         coarseOffset += carrierDiff;
	         fineOffset -= carrierDiff;
	      }
	      else
	      if (fineOffset < -carrierDiff / 2) {
	         coarseOffset -= carrierDiff;
	         fineOffset += carrierDiff;
	      }
	      show_Corrector (coarseOffset, fineOffset);
	      if ((oldCoarseOffset != coarseOffset) &&
	          (theFicHandler. get_ficQuality () < 40))
	              correctionNeeded = true;;
	   
//ReadyForNewFrame:
///	and off we go, up to the next frame
	   }
	}
	catch (int e) {
//	   fprintf (stderr, "ofdmHandler is stopping\n");
	   ;
	}
}
//
//
void	ofdmHandler::set_scanMode	(bool b) {
	scanMode	= b;
	attempts	= 0;
}

void	ofdmHandler::get_frameQuality	(int	*totalFrames,
	                                 int	*goodFrames,
	                                 int	*badFrames) {
	*totalFrames		= this	-> totalFrames;
	*goodFrames		= this	-> goodFrames;
	*badFrames		= this	-> badFrames;
	this	-> totalFrames	= 0;
	this	-> goodFrames	= 0;
	this	-> badFrames	= 0;
}
//
//	just convenience functions
//	ficHandler abstracts channel data

QString	ofdmHandler::find_service	(uint32_t SId, int SCIds) {
	return theFicHandler. find_service (SId, SCIds);
}

void	ofdmHandler::get_parameters	(const QString &s,
	                                 uint32_t *p_SId, int*p_SCIds) {
	theFicHandler. get_parameters (s, p_SId, p_SCIds);
}

std::vector<serviceId>	ofdmHandler::get_services	(int n) {
	return theFicHandler. get_services (n);
}

int	ofdmHandler::get_nrComps	(uint32_t SId) {
	return theFicHandler. get_nrComps (SId);
}

void	ofdmHandler::data_for_audioservice	(const QString &s,
	                                         audiodata &d) {
	theFicHandler. data_for_audioservice (s, d);
}

void	ofdmHandler::data_for_packetservice	(const QString &s,
	                                         packetdata &pd,
	                                         int16_t compnr) {
	theFicHandler. data_for_packetservice (s, pd, compnr);
}

uint8_t	ofdmHandler::get_ecc 		() {
	return theFicHandler. get_ecc();
}

int32_t ofdmHandler::get_ensembleId	() {
	return theFicHandler. get_ensembleId();
}

QString ofdmHandler::get_ensembleName	() {
	return theFicHandler. get_ensembleName();
}

void	ofdmHandler::set_epgData	(int SId, int32_t theTime,
	                                 const QString &s,
	                                 const QString &d) {
	theFicHandler. set_epgData (SId, theTime, s, d);
}

bool	ofdmHandler::has_timeTable	(uint32_t SId) {
	return theFicHandler. has_timeTable (SId);
}

std::vector<epgElement>	ofdmHandler::find_epgData	(uint32_t SId) {
	return theFicHandler. find_epgData (SId);
}

QStringList ofdmHandler::basicPrint	() {
	return theFicHandler. basicPrint ();
}

int	ofdmHandler::scanWidth		() {
	return theFicHandler. scanWidth ();
}
//
//	for the mscHandler:
void	ofdmHandler::reset_services	() {
	if (!scanMode)
	   theMscHandler. reset_Channel ();
}

void	ofdmHandler::stop_service (descriptorType *d, int flag) {
	fprintf (stderr, "function obsolete\n");
	if (!scanMode)
	   theMscHandler. stop_service (d -> subchId, flag);
}

void	ofdmHandler::stop_service (int subChId, int flag) {
	if (!scanMode)
	   theMscHandler. stop_service (subChId, flag);
}

bool    ofdmHandler::set_audioChannel (audiodata &d,
	                               RingBuffer<std::complex<int16_t>> *b,
	                               FILE *dump, int flag) {
	if (!scanMode)
	   return theMscHandler. set_Channel (d, b,
	                         (RingBuffer<uint8_t> *)nullptr, dump, flag);
	else
	   return false;
}

bool    ofdmHandler::set_dataChannel (packetdata &d,
	                               RingBuffer<uint8_t> *b, int flag) {
	if (!scanMode)
	   return theMscHandler. set_Channel (d,
	                     (RingBuffer<std::complex<int16_t>> *)nullptr, b,
	                      nullptr, flag);
	else
	   return false;
}

void	ofdmHandler::start_dumping	(const QString &f, int freq) {
	theReader. start_dumping (f, freq);
}

void	ofdmHandler::stop_dumping() {
	theReader. stop_dumping();
}

void	ofdmHandler::start_ficDump	(FILE *f) {
	theFicHandler. start_ficDump (f);
}

void	ofdmHandler::stop_ficDump	() {
	theFicHandler. stop_ficDump ();
}

uint32_t ofdmHandler::julianDate	()  {
	return theFicHandler. julianDate ();
}

bool	ofdmHandler::start_etiGenerator	(const QString &s) {
	eti_on = theEtiGenerator. start_etiGenerator (s);
	return eti_on;
}

void	ofdmHandler::stop_etiGenerator		() {
	theEtiGenerator. stop_etiGenerator ();
	eti_on		= false;
}

void	ofdmHandler::reset_etiGenerator	() {
	theEtiGenerator. reset ();
}

void	ofdmHandler::handle_iqSelector	() {
	theOfdmDecoder. handle_iqSelector ();
}

void	ofdmHandler::handle_decoderSelector	(int d) {
	theOfdmDecoder. handle_decoderSelector (d);
}

void	ofdmHandler::set_correlationOrder	(bool b) {
	correlationOrder = b;
}

void	ofdmHandler::set_dxMode		(bool b) {
	dxMode	= b;
}

