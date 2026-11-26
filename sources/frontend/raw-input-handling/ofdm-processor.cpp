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
#include	"ofdm-processor.h"

#include	<utility>
#include	"radio.h"
#include	"dab-constants.h"
#include	"process-params.h"
#include	"dab-params.h"
#include	"timesyncer.h"
#include	"freqsyncer.h"
#include	"ringbuffer.h"
#include	"correlator.h"
#include	"estimator.h"
#include	"logger.h"
#include	"settingNames.h"
#include	"settings-handler.h"

#include	"tii-detector-1.h"

/**
  *	\brief ofdmProcessor
  *	The ofdmProcessor class is the driver of the processing
  *	of the samplestream.
  *	It is the main interface to the "radio" program, that
  *	controls the GUI and the various processing elements
  */

	ofdmProcessor::ofdmProcessor	(RadioInterface	*mr,
	                                 deviceHandler	*inputDevice,
	                                 processParams	*p,
	                                 QSettings	*dabSettings,
	                                 logger		*theLogger,
	                                 uint8_t	cpuSupport):
	                                    radioInterface_p (mr),
	                                    settings_p (dabSettings),
	                                    theReader (mr,
	                                              inputDevice,
	                                              p -> spectrumBuffer),
	                                    theFicHandler (mr, cpuSupport),
	                                    theEtiGenerator (&theFicHandler,
	                                                     cpuSupport),
	                                    theOfdmDecoder (mr,
	                                                 inputDevice -> bitDepth(),
	                                                 p -> stdDevBuffer,
	                                                 p -> carrierBuffer,
	                                                 p -> iqBuffer),
	                                    theMscHandler (mr, p -> frameBuffer,
	                                                   theLogger,
	                                                   cpuSupport),
	                                    fft (get_T_u (), false) {
	this	-> p			= p;
	this	-> theLogger		= theLogger;
	this	-> cpuSupport		= cpuSupport;
	this	-> thresHold		= p -> threshold;
	this	-> tiiBuffer_p		= p -> tiiBuffer;
	this	-> nullBuffer_p		= p -> nullBuffer;
	this	-> snrBuffer_p		= p -> snrBuffer;
	this	-> channelBuffer_p	= p -> channelBuffer;
	this	-> carrierBuffer	= p -> carrierBuffer;
	this	-> T_null		= get_T_null ();
	this	-> T_s			= get_T_s ();
	this	-> T_u			= get_T_u ();
	this	-> T_g			= T_s - T_u;
	this	-> T_F			= get_T_F ();
	this	-> nrBlocks		= get_L ();
	this	-> carriers		= get_carriers ();
	this	-> carrierDiff		= get_carrierDiff ();

	this	-> null_avg		= 0.2;
	viewCarrier_mode		= 255;
	starter				= true;
	this	-> tiiDelay		= p -> tii_delay;
	this	-> tiiCounter		= 0;
	this	-> dxMode		=
	 		                  value_i (dabSettings,
	                                           CONFIG_HANDLER,
	                                           S_DX_MODE, 0) != 0;
	this	-> decoder		= value_i (dabSettings, CONFIG_HANDLER, 
	                                           "decoders", DECODER_1); 
	QString		 correlationText	=
	         value_s (dabSettings, CONFIG_HANDLER,
	                             S_CORRELATION_ORDER, "first found");
	setCorrelationOrder (correlationText);

	this	-> etiOn		= false;
	ofdmBuffer. resize (3 * T_s);
	fineOffset			= 0;	
	coarseOffset			= 0;	
	correctionNeeded		= true;
	attempts			= 0;

	goodFrames			= 0;
	badFrames			= 0;
	totalFrames			= 0;
	scanMode			= false;

	connect (this, &ofdmProcessor::setSynced,
	         radioInterface_p, &RadioInterface::set_synced);
	connect (this, &ofdmProcessor::setSyncLost,
	         radioInterface_p, &RadioInterface::set_sync_lost);
	connect (this, &ofdmProcessor::showTIIData,
	         radioInterface_p, &RadioInterface::show_tiiData);
	connect (this, &ofdmProcessor::showTIIspectrum,
	         radioInterface_p, &RadioInterface::show_tii_spectrum);
	connect (this, static_cast<void (ofdmProcessor::*)(float)>(&ofdmProcessor::showSnr),
	         mr, &RadioInterface::show_snr);
	connect (this, &ofdmProcessor::showClockError,
	         mr,  &RadioInterface::show_clock_error);
	connect (this, &ofdmProcessor::showNull,
	         mr, &RadioInterface::show_null);
	connect (this, &ofdmProcessor::showChannel,
	         mr, &RadioInterface::show_channel);
	connect (this, &ofdmProcessor::showCorrector,
	         mr,  &RadioInterface::show_Corrector);
	connect (this, &ofdmProcessor::show_carriers,
	         radioInterface_p, &RadioInterface::show_carriers);
	tiiThreshold = value_i (settings_p, CONFIG_HANDLER,
                                             TII_THRESHOLD, 6);
	tiiCollision	= value_i (settings_p, CONFIG_HANDLER,
	                                      "tiiCollision", -1);
	theOfdmDecoder. handle_decoderSelector (decoder);

	this	-> snr		= 10;	// until we know better
}

	ofdmProcessor::~ofdmProcessor () {
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

void	ofdmProcessor::setTIIThreshold	(int16_t threshold) {
	tiiThreshold = threshold;
}

void	ofdmProcessor::setTIICollisions	(int subId) {
	tiiCollision = subId;
}

void	ofdmProcessor::start () {
	fineOffset			= 0;	
	coarseOffset			= 0;	
	attempts			= 0;
	rateError			= 0;
	goodFrames			= 0;
	badFrames			= 0;
	totalFrames			= 0;
	theOfdmDecoder. reset	();
	theFicHandler.  restart	();
	if (!scanMode)
	   theMscHandler. resetChannel ();
	QThread::start ();
}

void	ofdmProcessor::stop	() {
	theReader. setRunning (false);
	while (isRunning ())
//	   wait ();
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
void	ofdmProcessor::run	() {
timeSyncer	myTimeSyncer	(&theReader);
TII_Detector	theTIIDetector	(&theTable);
freqSyncer	myFreqSyncer	(radioInterface_p, &theTable);
correlator	myCorrelator	(radioInterface_p, p, &theTable);
int32_t		startIndex	= -1;
std::vector<int16_t> softbits;
int	frameCount	= 0;
int	sampleCount	= 0;
int	totalSamples	= 0;
int	cCount		= 0;
bool	inSync		= false;
int	tryCounter	= 0;
Complex *tester	= dynVec (Complex, T_u / 2);

	this	-> snr		= 10; 	// until really computed
	this	-> null_avg	= 0.2;
	softbits. resize (2 * get_carriers());
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
	         setSynced (false);
	         theTIIDetector. reset ();
	         switch (myTimeSyncer. sync (T_null, T_F)) {
	            case TIMESYNC_ESTABLISHED:
	               inSync	= true;
	               setSynced (true);
	               break;			// yes, we are ready

	            case NO_DIP_FOUND:
	               if (++ attempts >= 8) {
	                  emit (noSignalFound());
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
	                                               thresHold);

	         if (starter || (startIndex < 0)) { // no sync, try again
	            if (!correctionNeeded) {
	               setSyncLost ();	
	            }
	            starter	= false;
	            badFrames ++;
	            setSynced (false);
	            inSync	= false;
	            continue;
	         }
	         sampleCount	= startIndex;
	      }
	      else {	// we are in sync and continue with a next frame
	         totalFrames ++;
	         frameCount ++;
	         bool nullShower	= false;
	         totalSamples	+= sampleCount;
	         if (frameCount >= 10) {
	            rateError = SAMPLERATE * 
	                         (totalSamples / ((float)frameCount * T_F) - 1);
	            showClockError ((int)rateError);
	            totalSamples = 0;
	            frameCount	= 0;
	            nullShower	= true;
	            memcpy (tester, &(ofdmBuffer [T_null - T_u / 3]),
	                                 T_u / 4 * sizeof (Complex));
	         }

	         theReader. getSamples (ofdmBuffer, 0,
	                               T_u, coarseOffset + fineOffset,  false);
	         startIndex = myCorrelator. findIndex (ofdmBuffer,
	                                               correlationOrder,
	                                               2.5 * thresHold);
//
	         if (nullShower) {
	            memcpy (&tester [T_u / 4], ofdmBuffer. data (),
	                               T_u / 4 * sizeof (Complex));
	            nullBuffer_p -> putDataIntoBuffer (tester, T_u / 2);
	            showNull (T_u / 2, startIndex);
	         }

	         if (startIndex < 0) { // no sync, try again
	            if (!correctionNeeded) {
	               setSyncLost();
	            }
	            badFrames	++;
	            inSync	= false;
	            setSynced (false);
	            continue;
	         }
	         sampleCount = startIndex;
	      }
	      
	      goodFrames ++;
	      double cLevel	= 0;
	      cCount	= 0;

//	The size of the ofdm Buffer is large enough to 
//	read All data of the first block in
	      theReader. getSamples (ofdmBuffer,
	                             T_u,
	                             startIndex,
	                             coarseOffset + fineOffset, true);
	      
	      if (radioInterface_p -> channelOn ()) {
	         static int abc = 0;
	         if (++abc >= 6) {
	            generate_CI (ofdmBuffer, startIndex);
	            abc = 0;
	         }
	      }
//
//	Then we move the data of the first block to the start of the buffer:
	      memmove (ofdmBuffer. data (),
	               &((ofdmBuffer. data()) [startIndex]),
	                  T_u * sizeof (Complex));
//Block_0:
/**
  *	Block 0 is special in that it is used for fine time synchronization,
  *	for coarse frequency synchronization
  *	and its content is used as a reference for decoding the
  *	first datablock.
  *	We read the missing samples in the ofdm buffer
  */
	      sampleCount	+= T_u;
	      (void) theOfdmDecoder. processBlock_0 (ofdmBuffer);

//	Here we look only at the block_0 when we need a coarse
//	frequency synchronization.
	      correctionNeeded = !theFicHandler. syncReached ();
	      if (correctionNeeded && (tryCounter == 0)) {
	         int correction	=
	            myFreqSyncer. estimateCarrierOffset (ofdmBuffer);
	         if (correction != 100) {
	            if (abs (coarseOffset) > Khz (35))
	               coarseOffset = 0;
	            else {
//	               coarseOffset	+= 0.4 * correction * carrierDiff;
	               coarseOffset	+=  correction * carrierDiff;
//	               tryCounter	= 5;
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
  *	possibly in a different thread.
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
	                                 T_s, coarseOffset + fineOffset,  true);
//	         fprintf (stderr, "Block %d\n", ofdmSymbolCount);
	         sampleCount += T_s;
	         for (int i = (int)T_u; i < (int)T_s; i ++) {
	            FreqCorr +=
	                      ofdmBuffer [i] * conj (ofdmBuffer [i - T_u]);
	            cLevel += jan_abs (ofdmBuffer [i]) +
	                                  jan_abs (ofdmBuffer [i - T_u]);
	         }
	         cCount += 2 * T_g;
//
//	If "eti_on" we process all data here
	         if (etiOn) {
	            theOfdmDecoder.
	                   decode (ofdmBuffer, ofdmSymbolCount,
	                                            softbits, snr, rateError);
	            if (ofdmSymbolCount <= 3) 
	               theFicHandler.
	                    processFICBlock (softbits, ofdmSymbolCount);
	            else 
	               theMscHandler. processMscBlock (softbits,
	                                             ofdmSymbolCount);
	            theEtiGenerator. processBlock (softbits, ofdmSymbolCount);
	            continue;
	         }
//
//	Normal Processing, no eti 
	         if (ofdmSymbolCount <= 3) {
	            theOfdmDecoder.
                           decode (ofdmBuffer, ofdmSymbolCount,
	                                            softbits, snr, rateError);
	            theFicHandler.   
                            processFICBlock (softbits, ofdmSymbolCount);
	         }
	         if (scanMode)
	            continue;

	         if (ofdmSymbolCount >= 4) {
	            theOfdmDecoder.
	                    decode (ofdmBuffer, ofdmSymbolCount,
	                                             softbits, snr, rateError);
	            theMscHandler.
	                    processMscBlock (softbits, ofdmSymbolCount);
	         }
	      }
/**
  *	OK,  here we are at the end of the frame
  *	Assume everything went well and "skip" T_null samples
  */
	      theReader. getSamples (ofdmBuffer, 0,
	                         T_null, coarseOffset + fineOffset, false);
	      sampleCount += T_null;

//	The snr is computed, where we take as "noise" the signal strength
//	of the NULL period (the one without TII data)
//	" The TII signal shall fill the null symbol of each transmission
//	frame comprising the CIFs of CIF count 0, 1, 2, 3"
//	modulo 8 (transmission mode I).
//	We have the CIF count of the previous frame
	      int16_t CIF_hi, CIF_lo;
	      theFicHandler. getCIFcount (CIF_hi, CIF_lo); 
	      if ((CIF_lo & 0x07) >= 4) {
	         static int t2 = 0;
	         if (viewCarrier_mode == NULL_CARRIERS_TII) {
	            if (++ t2 >= 4) {
	               DABFLOAT *cBuf = dynVec (DABFLOAT, carriers);
	               NullToCarriers (ofdmBuffer, cBuf);
	               carrierBuffer -> putDataIntoBuffer (cBuf, carriers);
	               emit show_carriers (viewCarrier_mode, carriers);
	               t2 = 0;
	            }
	         }
	         theTIIDetector. addBuffer (ofdmBuffer);
	         tiiBuffer_p -> putDataIntoBuffer (ofdmBuffer. data (),
	                                                           T_null);
	         showTIIspectrum ();
	         if (++tiiCounter >= tiiDelay) {
	            tiiCounter = 0;
	            QVector<tiiData> resVec =
	                   theTIIDetector. processNULL (tiiThreshold,
	                                                 tiiCollision);
	            showTIIData (resVec, 0);
	         }
	      }
	      else {	// compute SNR
	         if (viewCarrier_mode == NULL_CARRIERS_NO_TII) {
	            static int t3 = 0;
	            if (++t3 >= 4) {
	               DABFLOAT *cBuf = dynVec (DABFLOAT, carriers);
	               NullToCarriers (ofdmBuffer, cBuf);
	               carrierBuffer -> putDataIntoBuffer (cBuf, carriers);
	               emit show_carriers (viewCarrier_mode, carriers);
	               t3 = 0;
	            }
	         }

	         float sum	= 0;
	         for (int i = 100; i < T_null - 100; i ++)
	            sum += jan_abs (ofdmBuffer [i]);
	         sum /= T_null - 200;
	         null_avg	= 0.9 * null_avg + 0.1 * sum;

//	         float sLevel	= theReader. getSLevel ();
	         float snrV	=
//	              20 * log10 ((sLevel + 0.005) / (null_avg));
	              10 * log10 ((cLevel / cCount + 0.005) / (null_avg));
//	         this -> snr = 0.8 * this ->  snr + 0.2 * snrV;
	         showSnr (snr);
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
	      showCorrector (coarseOffset, fineOffset);
	      if ((oldCoarseOffset != coarseOffset) &&
	          (theFicHandler. getFICQuality () < 40))
	              correctionNeeded = true;;
	   
//ReadyForNewFrame:
///	and off we go, up to the next frame
	   }
	}
	catch (int e) {
//	   fprintf (stderr, "ofdmProcessor is stopping\n");
	   ;
	}
}
//
//
void	ofdmProcessor::setScanMode	(bool b) {
	scanMode	= b;
	attempts	= 0;
}

void	ofdmProcessor::getFrameQuality	(int	*totalFrames,
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

bool	ofdmProcessor::isPrimaryService		(uint32_t SId, uint8_t SCIds) {
	return theFicHandler. isPrimaryService (SId, SCIds);
}

bool	ofdmProcessor::isAudioService		(uint32_t SId, uint8_t SCIds) {
	return theFicHandler. isAudioService (SId, SCIds);
}

bool	ofdmProcessor::isPacketService		(uint32_t SId, uint8_t SCIds) {
	return theFicHandler. isPacketService (SId, SCIds);
}

bool	ofdmProcessor::is_SPI			(uint32_t SId) {
	return theFicHandler. is_SPI (SId);
}

std::vector<uint8_t>
        ofdmProcessor::get_secondaryServices	(uint32_t SId) {
	return theFicHandler. get_secondaryServices (SId);
}

int	ofdmProcessor::FIG07_value		() {
	return theFicHandler. FIG07_value ();
}

void	ofdmProcessor::getFreqs			(uint32_t SId,
	                                           std::vector<uint32_t> &fl) {
	theFicHandler. getFreqs (SId, fl);
}

void	ofdmProcessor::getServiceName		(QString &serviceName,
	                                         QString &shortName,
	                                         uint32_t SId, uint8_t SCIds) {
	theFicHandler. getServiceName (serviceName, shortName, SId, SCIds);
}

void	ofdmProcessor::mapNameToId		(const QString &name,
                                                    uint32_t &SId,
	                                                  uint8_t &SCIds) {
	theFicHandler. mapNameToId (name, SId, SCIds);
}

void	ofdmProcessor::audioData			(uint32_t SId, uint8_t SCIds,
	                                                audiodata &d) {
	theFicHandler. audioData (SId, SCIds, d);
}

void	ofdmProcessor::packetData			(uint32_t SId, uint8_t SCIds,
	                                                packetdata &pd) {
	theFicHandler. packetData (SId, SCIds, pd);
}

QList<contentType> ofdmProcessor::contentPrint	() {
	QList<contentType> res = theFicHandler. contentPrint ();
	for (auto &ct : res) 
	   ct. isRunning = serviceRuns (ct. SId, ct. SCIds);
	return res;
}

int	ofdmProcessor::freeSpace		() {
	return theFicHandler. freeSpace ();
}

//	controlling functions
//
void	ofdmProcessor::stopService (uint32_t SId, uint8_t SCIds) {
	if (!scanMode)
	   theMscHandler. stopBackend (SId, SCIds);
}

bool    ofdmProcessor::setAudioChannel (audiodata &d,
	                               RingBuffer<std::complex<int16_t>> *b,
	                               FILE *dump, int flag) {
	if (!scanMode)
	   return theMscHandler. startBackend (d, b,
	                         (RingBuffer<uint8_t> *)nullptr, dump, flag);
	else
	   return false;
}

bool    ofdmProcessor::setDataChannel (packetdata &d,
	                               RingBuffer<uint8_t> *b, int flag) {
	if (!scanMode)
	   return theMscHandler. startBackend (d,
	                     (RingBuffer<std::complex<int16_t>> *)nullptr, b,
	                      nullptr, flag);
	else
	   return false;
}

//
void	ofdmProcessor::startDumping	(const QString &f, int freq,
	                                 int bitDepth, const QString creator) {
	theReader. startDumping (f, freq, bitDepth, creator);
}

void	ofdmProcessor::stopDumping() {
	theReader. stopDumping();
}

bool	ofdmProcessor::isDumping		() {
	return theReader. isDumping ();
}

void	ofdmProcessor::startFicDump	(const QString &f) {
	theFicHandler. startFICDump (f);
}

bool	ofdmProcessor::ficDumping_on	() {
	return theFicHandler. ficDumping_on ();
}

void	ofdmProcessor::stopFicDump	() {
	theFicHandler. stopFICDump ();
}

uint32_t ofdmProcessor::julianDate	()  {
	return theFicHandler. julianDate ();
}

bool	ofdmProcessor::startEtiGenerator	(const QString &s) {
	etiOn = theEtiGenerator. start_etiGenerator (s);
	return etiOn;
}

void	ofdmProcessor::stopEtiGenerator		() {
	etiOn		= false;
	theEtiGenerator. stop_etiGenerator ();
}

void	ofdmProcessor::resetEtiGenerator	() {
	theEtiGenerator. reset ();
}

void	ofdmProcessor::handleIQSelector	() {
	theOfdmDecoder. handle_iqSelector ();
}

void	ofdmProcessor::handleDecoderSelector	(int d) {
	theOfdmDecoder. handle_decoderSelector (d);
}

void	ofdmProcessor::setCorrelationOrder	(const QString & order) {
	if (order == ISI_REDUCE)
	   correlationOrder	= ISI_ONE;
	else
	if (order == MAX_CORR)
	   correlationOrder	= MAX_ONE;
	else
	   correlationOrder	= FIRST_ONE;
}

void	ofdmProcessor::setDXMode		(bool b) {
	dxMode	= b;
}

void	ofdmProcessor::setSpeedUp	(bool b) {
	freqSpeedUp	= b;
	store (settings_p, CONFIG_HANDLER, "SPEED_UP", b ? 1 : 0);
}

void	ofdmProcessor::setFreqCorrelator	(uint8_t corr) {
	store (settings_p, CONFIG_HANDLER, "SELECT_CORR", corr);
}

bool	ofdmProcessor::serviceRuns	(uint32_t SId, uint8_t SCIds) {
	return theMscHandler. serviceRuns (SId, SCIds);
}

void	ofdmProcessor::generate_CI (const std::vector<Complex> &rawBuffer,
	                          int startIndex) {
estimator myEstimator (&theTable);
std::vector<Complex> CI_Vector (NR_TAPS);

	myEstimator. estimate (rawBuffer, CI_Vector);
	channelBuffer_p -> putDataIntoBuffer (CI_Vector. data (),
	                                                  CI_Vector. size ());
	emit showChannel (CI_Vector. size ());
}

void	ofdmProcessor::set_dcRemoval	(bool b) {
	theReader. set_dcRemoval (b);
}

void	ofdmProcessor::set_iqBalance	(bool b) {
	theReader. set_iqBalance (b);
}

void	ofdmProcessor::set_dataTracer	(bool b) {
	theMscHandler. set_dataTracer (b);
}

std::vector<basicService>  ofdmProcessor::getServices	() {
	return theFicHandler. getServices ();
}

void	ofdmProcessor::viewCarriers	(uint8_t carrierMode) {
	viewCarrier_mode	= carrierMode;
	theOfdmDecoder. viewCarriers	(carrierMode);
}

void	ofdmProcessor::NullToCarriers (const std::vector<Complex> &in,
	                             DABFLOAT *out) {
std::vector<Complex>buffer (T_u);
	for (int i = 0; i < T_u; i ++)
	   buffer [i] = in [i];
	fft. fft (buffer);
	for (int i = 0; i < carriers; i ++)
	   out [i] = abs (buffer [(T_u - carriers / 2 + i) % T_u]);
}

