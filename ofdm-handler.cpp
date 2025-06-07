#
/*
 *    Copyright (C) 2014 .. 2025
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

#include	"tii-detector-1.h"
#include	"tii-detector-2.h"

#define	TII_OLD	0100
#define	TII_NEW	0101

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
	                                 logger		*theLogger,
	                                 uint8_t	cpuSupport):
	                                    radioInterface_p (mr),
	                                    params (p -> dabMode),
	                                    settings_p (dabSettings),
	                                    theReader (mr,
	                                              inputDevice,
	                                              p -> spectrumBuffer),
	                                    theFicHandler (mr, p -> dabMode,
	                                                      cpuSupport),
	                                    theEtiGenerator (p -> dabMode,
	                                                  &theFicHandler,
	                                                  cpuSupport),
	                                    theOfdmDecoder (mr,
	                                                 p -> dabMode,
	                                                 inputDevice -> bitDepth(),
	                                                 p -> stdDevBuffer,
	                                                 p -> iqBuffer),
	                                    theMscHandler (mr, p -> dabMode,
	                                                p -> frameBuffer,
	                                                theLogger,
	                                                cpuSupport) {

	this	-> p			= p;
	this	-> theLogger		= theLogger;
	this	-> cpuSupport		= cpuSupport;
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

	this	-> freq_speedUp		= false;
	this	-> tii_delay		= p -> tii_delay;
	this	-> tii_counter		= 0;
	this	-> correlationOrder	= 
	         value_i (dabSettings, CONFIG_HANDLER,
	                             S_CORRELATION_ORDER, 0) != 0;
	this	-> dxMode		=
	         value_i (dabSettings, CONFIG_HANDLER, S_DX_MODE, 0) != 0;
	this	-> decoder		= value_i (dabSettings, CONFIG_HANDLER, 
	                                           "decoders", DECODER_1); 
	this	-> selected_TII		= value_i (dabSettings, CONFIG_HANDLER,		 	                                   "tii-detector", 1) == 0 ?
	                                          TII_OLD : TII_NEW;

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
	theOfdmDecoder. handle_decoderSelector (decoder);
}

	ofdmHandler::~ofdmHandler () {
	   if (isRunning ()) {
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
	fineOffset			= 0;	
	coarseOffset			= 0;	
	attempts			= 0;

	goodFrames			= 0;
	badFrames			= 0;
	totalFrames			= 0;
	theOfdmDecoder. reset	();
	theFicHandler.  restart	();
	if (!scanMode)
	   theMscHandler. resetChannel ();
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
timeSyncer	myTimeSyncer (&theReader);
phaseTable	theTable (p -> dabMode);
TII_Detector_B	theTIIDetector_OLD (p -> dabMode, &theTable, settings_p);
TII_Detector_A	theTIIDetector_NEW (p -> dabMode, &theTable);
freqSyncer	myFreqSyncer (radioInterface_p, p, &theTable, freq_speedUp);
estimator	myEstimator  (radioInterface_p, p, &theTable);
correlator	myCorrelator (radioInterface_p, p, &theTable);
int32_t		startIndex	= -1;
std::vector<int16_t> ibits;
int	frameCount	= 0;
int	sampleCount	= 0;
int	totalSamples	= 0;
int	cCount		= 0;
float	snr		= 0;
bool	inSync		= false;
int	tryCounter	= 0;
Complex tester	[T_u / 2];
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
	   for (int i = 0; i < T_F / tempSize; i ++) {
	      theReader. getSamples (temp, 0, tempSize, 0, true);
	   }

	   while (true) {
	      if (!inSync) {
	         totalFrames ++;
	         totalSamples	= 0;
	         frameCount	= 0;
	         sampleCount	= 0;
	         set_synced (false);
	         if (selected_TII == TII_NEW)
	            theTIIDetector_NEW. reset ();
	         else
	            theTIIDetector_OLD. reset ();
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

	         theReader. getSamples (ofdmBuffer, 0,
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
	            memcpy (tester, &(ofdmBuffer [T_null - T_u / 3]),
	                                 T_u / 4 * sizeof (Complex));
	         }

	         theReader. getSamples (ofdmBuffer, 0,
	                               T_u, coarseOffset + fineOffset, false);
	         startIndex = myCorrelator. findIndex (ofdmBuffer,
	                                               correlationOrder,
	                                               2.5 * threshold);
	         if (null_shower) {
	            memcpy (&tester [T_u / 4], ofdmBuffer. data (),
	                               T_u / 4 * sizeof (Complex));
	            nullBuffer_p -> putDataIntoBuffer (tester, T_u / 2);
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
	      theReader. getSamples (ofdmBuffer,
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
	      correctionNeeded = !theFicHandler. syncReached ();
	      if (correctionNeeded && (tryCounter == 0)) {
	         int correction	=
	            myFreqSyncer. estimate_CarrierOffset (ofdmBuffer);
	         if (correction != 100) {
	            if (abs (coarseOffset) > Khz (35))
	               coarseOffset = 0;
	            else {
//	               coarseOffset	+= 0.4 * correction * carrierDiff;
	               coarseOffset	+=  correction * carrierDiff;
	               tryCounter	= 5;
	            }
	         }
	      }
	      else
	      if (!correctionNeeded)
	         tryCounter = 5;
	      else
	      if (tryCounter > 0)
	         tryCounter --;
	      
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
	         theReader. getSamples (ofdmBuffer, 0,
	                                 T_s, coarseOffset + fineOffset, true);
	         sampleCount += T_s;
	         for (int i = (int)T_u; i < (int)T_s; i ++) {
	            FreqCorr +=
	                      ofdmBuffer [i] * conj (ofdmBuffer [i - T_u]);
	            cLevel += jan_abs (ofdmBuffer [i]) +
	                                  jan_abs (ofdmBuffer [i - T_u]);
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
	                    processFICBlock (ibits, ofdmSymbolCount);
	            else 
	               theMscHandler. processMscBlock (ibits, ofdmSymbolCount);
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
                            processFICBlock (ibits, ofdmSymbolCount);
	         }
	         if (scanMode)
	            continue;
#ifdef	__MSC_THREAD__
	         theMscHandler. processMsc (ofdmBuffer, 0,  ofdmSymbolCount);
#else
	         if (ofdmSymbolCount >= 4) {
	            theOfdmDecoder.
	                    decode (ofdmBuffer, ofdmSymbolCount, ibits);
	            theMscHandler.
	                    processMscBlock (ibits, ofdmSymbolCount);
	         }
#endif
	      }
/**
  *	OK,  here we are at the end of the frame
  *	Assume everything went well and skip T_null samples
  */
	      theReader. getSamples (ofdmBuffer, 0,
	                         T_null, coarseOffset + fineOffset, false);
	      sampleCount += T_null;
//
//	The snr is computed, where we take as "noise" the signal strength
//	of the NULL period (the one without TII data)

	      if (frame_with_TII) {
	         if (selected_TII == TII_NEW)
	            theTIIDetector_NEW. addBuffer (ofdmBuffer);
	         else
	            theTIIDetector_OLD. addBuffer (ofdmBuffer);
	         if (++tii_counter >= tii_delay) {
	            tiiBuffer_p -> putDataIntoBuffer (ofdmBuffer. data(),
	                                                          T_u);
	            show_tii_spectrum ();
	            QVector<tiiData> resVec =
	                selected_TII == TII_NEW ?
	                       theTIIDetector_NEW. processNULL (tiiThreshold,
	                                                 tiiCollisions_active,
	                                                 tiiFilter_active):
	                       theTIIDetector_OLD. processNULL (tiiThreshold,
	                                                 tiiCollisions_active,
	                                                 tiiFilter_active);
	            show_tiiData (resVec, 0);
	            tii_counter = 0;
	         }
	      }
	      else {	// compute SNR
	         float sum	= 0;
	         for (int i = 0; i < T_null; i ++)
	            sum += jan_abs (ofdmBuffer [i]);
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
	         theOfdmDecoder. setPowerLevel (sum / T_u);
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
	          (theFicHandler. getFICQuality () < 40))
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

int	ofdmHandler::get_serviceComp	(const QString &s) {
	return theFicHandler. get_serviceComp (s);
}

int	ofdmHandler::get_serviceComp	(uint32_t SId, int compnr) {
	return theFicHandler. get_serviceComp (SId, compnr);
}

int	ofdmHandler::get_serviceComp_SCIds	(uint32_t SId, int SCIds) {
	return theFicHandler. get_serviceComp (SId, SCIds);
}

bool	ofdmHandler::isPrimary (const QString &s) {
	return theFicHandler. isPrimary (s);
}

uint16_t ofdmHandler::get_announcing	(uint16_t SId) {
	return theFicHandler. get_announcing (SId);
}

int	ofdmHandler::get_nrComps	(uint32_t SId) {
	return theFicHandler. get_nrComps (SId);
}

uint32_t ofdmHandler::get_SId			(int index) {
	return theFicHandler. get_SId (index);
}

uint8_t	ofdmHandler::serviceType		(int index) {
	return theFicHandler. serviceType (index);
}

void	ofdmHandler::audioData			(int index, audiodata &d) {
	theFicHandler. audioData (index, d);
}

void	ofdmHandler::packetData			(int index, packetdata &pd) {
	theFicHandler. packetData (index, pd);
}

uint8_t	ofdmHandler::get_ecc 		() {
	return theFicHandler. get_ecc();
}

std::vector<int>	ofdmHandler::getFrequency	(const QString &s) {
	return theFicHandler. getFrequency (s);
}

QStringList ofdmHandler::basicPrint	() {
QStringList res;
	return theFicHandler. basicPrint ();
	return res;;
}

int	ofdmHandler::scanWidth		() {
	return theFicHandler. scanWidth ();
	return 0;
}

int	ofdmHandler::freeSpace		() {
	return theFicHandler. freeSpace ();
}

//
void	ofdmHandler::stopService (int subChId, int flag) {
	if (!scanMode)
	   theMscHandler. stopService (subChId, flag);
}

bool    ofdmHandler::setAudioChannel (audiodata &d,
	                               RingBuffer<std::complex<int16_t>> *b,
	                               FILE *dump, int flag) {
	if (!scanMode)
	   return theMscHandler. setChannel (d, b,
	                         (RingBuffer<uint8_t> *)nullptr, dump, flag);
	else
	   return false;
}

bool    ofdmHandler::setDataChannel (packetdata &d,
	                               RingBuffer<uint8_t> *b, int flag) {
	if (!scanMode)
	   return theMscHandler. setChannel (d,
	                     (RingBuffer<std::complex<int16_t>> *)nullptr, b,
	                      nullptr, flag);
	else
	   return false;
}

void	ofdmHandler::start_dumping	(const QString &f, int freq,
	                                               int bitDepth) {
	theReader. startDumping (f, freq, bitDepth);
}

void	ofdmHandler::stop_dumping() {
	theReader. stopDumping();
}

void	ofdmHandler::start_ficDump	(FILE *f) {
	theFicHandler. startFICDump (f);
}

void	ofdmHandler::stop_ficDump	() {
	theFicHandler. stopFICDump ();
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

void	ofdmHandler::handleIQSelector	() {
	theOfdmDecoder. handle_iqSelector ();
}

void	ofdmHandler::handleDecoderSelector	(int d) {
	theOfdmDecoder. handle_decoderSelector (d);
}

void	ofdmHandler::setCorrelationOrder	(bool b) {
	correlationOrder = b;
}

void	ofdmHandler::setDXMode		(bool b) {
	dxMode	= b;
}

void	ofdmHandler::select_TII		(uint8_t a) {
	if (a == 0)
	   selected_TII = TII_OLD;
	else
	   selected_TII = TII_NEW;
}

void	ofdmHandler::set_speedUp	(bool b) {
	freq_speedUp	= b;
}

