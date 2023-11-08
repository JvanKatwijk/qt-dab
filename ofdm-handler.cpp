#
/*
 *    Copyright (C) 2014 .. 2022
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
 *    along with Qt-DAB if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"ofdm-handler.h"

#include	<utility>
#include	"fic-handler.h"
#include	"msc-handler.h"
#include	"radio.h"
#include	"process-params.h"
#include	"dab-params.h"
#include	"timesyncer.h"
#include	"freqsyncer.h"
#include	"ringbuffer.h"
#ifdef	__ESTIMATOR_
#include	"estimator.h"
#endif
#include	"correlator.h"

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
	                                 QSettings	*dabSettings):
	                                    radioInterface_p (mr),
	                                    device_p (inputDevice),
	                                    params (p -> dabMode),
	                                    settings_p (dabSettings),
	                                    theReader (mr,
	                                           inputDevice,
	                                           p -> spectrumBuffer),
	                                    theFicHandler (mr, p -> dabMode),
	                                    theEtiGenerator (p -> dabMode,
	                                                  &theFicHandler),
	                                    theTIIDetector (p -> dabMode,
	                                                  p -> tii_depth),
	                                    theOfdmDecoder (mr,
	                                                 p -> dabMode,
	                                                 inputDevice -> bitDepth(),
	                                                 p -> stdDevBuffer,
	                                                 p -> iqBuffer),
	                                    theMscHandler (mr, p -> dabMode,
	                                                p -> frameBuffer) {

	this	-> p			= p;
	this	-> threshold		= p -> threshold;
	this	-> tiiBuffer_p		= p -> tiiBuffer;
	this	-> nullBuffer_p		= p -> nullBuffer;
	this	-> snrBuffer_p		= p -> snrBuffer;
#ifdef	__ESTIMATOR_
	this	-> channelBuffer_p	= p -> channelBuffer;
#endif
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

	connect (this, SIGNAL (set_synced (bool)),
	         radioInterface_p, SLOT (set_synced (bool)));
	connect (this, SIGNAL (set_sync_lost (void)),
	         radioInterface_p, SLOT (set_sync_lost (void)));
	connect (this, SIGNAL (show_tii (int, int)),
	         radioInterface_p, SLOT (show_tii (int, int)));
	connect (this, SIGNAL (show_tii_spectrum ()),
	         radioInterface_p, SLOT (show_tii_spectrum ()));
	connect (this, SIGNAL (show_snr (float)),
	         mr, SLOT (show_snr (float)));
	connect (this, SIGNAL (show_clock_error (int)),
	         mr, SLOT (show_clock_error (int)));
	connect (this, SIGNAL (show_null (int)),
	         mr, SLOT (show_null (int)));
#ifdef	__ESTIMATOR_
	connect (this, SIGNAL (show_channel (int)),
	         mr, SLOT (show_channel (int)));
#endif
	connect (this, SIGNAL (show_Corrector (int, float)),
	         mr, SLOT (show_Corrector (int, float)));
	theTIIDetector. reset();
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

void	ofdmHandler::set_tiiDetectorMode	(bool b) {
	theTIIDetector. setMode (b);
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
freqSyncer	myFreqSyncer (radioInterface_p, p);
#ifdef	__ESTIMATOR_
estimator	myEstimator  (radioInterface_p, p);
#endif
correlator	myCorrelator (radioInterface_p, p);
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
	         startIndex = myCorrelator. findIndex (ofdmBuffer, threshold);
	         if (startIndex < 0) { // no sync, try again
	            if (!correctionNeeded) {
	               set_sync_lost();
	            }
	            badFrames ++;
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
	         if (null_shower) {
	            for (int i = 0; i < T_u / 4; i ++)
	               tester [T_u / 4 + i] = ofdmBuffer [i];
	            nullBuffer_p -> putDataIntoBuffer (tester. data (),
	                                                       T_u / 2);
	            show_null (T_u / 2);
	         }
	         startIndex = myCorrelator. findIndex (ofdmBuffer,
	                                              3 * threshold);
	         if (startIndex < 0) { // no sync, try again
	            if (!correctionNeeded) {
	               set_sync_lost();
	            }
	            badFrames	++;
	            inSync	= false;
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
	      set_synced (true);
	      theReader. get_samples (ofdmBuffer,
	                            ofdmBufferIndex,
	                            T_u - ofdmBufferIndex,
	                            coarseOffset + fineOffset, true);
#ifdef	__ESTIMATOR_
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
#endif
	      sampleCount	+= T_u;
	      theOfdmDecoder. processBlock_0 (ofdmBuffer);
#ifdef	__MSC_THREAD__
	      if (!scanMode)
	         theMscHandler.  processBlock_0 (ofdmBuffer. data());
#endif

//	Here we look only at the block_0 when we need a coarse
//	frequency synchronization.
	      correctionNeeded	= !theFicHandler. syncReached();
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
//	lots of cases
//	we always process all blocks

//
//	symbols 1 .. 3 are always processed using the ofdm decoder
//	and if eti is selected then we do them all
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
	      if (!isEvenFrame (theFicHandler. get_CIFcount(), &params)) {
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
/*
 *	odd frames carry - if any = the TII data
 */
	      if (params. get_dabMode () == 1) {
	         if (isEvenFrame (theFicHandler. get_CIFcount(), &params)) {
	            theTIIDetector. addBuffer (ofdmBuffer);
	            if (++tii_counter >= tii_delay) {
	               tiiBuffer_p -> putDataIntoBuffer (ofdmBuffer. data(),
	                                                          T_u);
	               show_tii_spectrum ();
	               uint16_t res = theTIIDetector. processNULL ();
	               if (res != 0) {
	                  uint8_t mainId	= res >> 8;
	                  uint8_t subId	= res & 0xFF;
	                  show_tii (mainId, subId);
	               }
	               tii_counter = 0;
	               theTIIDetector. reset();
	            }
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
//
	      fineOffset += 0.05 * arg (FreqCorr) / (2 * M_PI) * carrierDiff;
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

int	ofdmHandler::get_subCh_id	(const QString &s,
	                                          uint32_t SId) {
	return theFicHandler. get_subCh_id (s, SId);
}

bool	ofdmHandler::is_audioservice	(const QString &s) {
audiodata ad;
	theFicHandler. data_for_audioservice (s, ad);
	return ad. defined;
}

bool	ofdmHandler::is_packetservice	(const QString &s) {
packetdata pd;
	theFicHandler. data_for_packetservice (s, &pd, 0);
	return pd. defined;
}

void	ofdmHandler::data_for_audioservice	(const QString &s,
	                                         audiodata &d) {
	theFicHandler. data_for_audioservice (s, d);
}

void	ofdmHandler::data_for_packetservice	(const QString &s,
	                                         packetdata *pd,
	                                         int16_t compnr) {
	theFicHandler. data_for_packetservice (s, pd, compnr);
}

uint8_t	ofdmHandler::get_ecc 		() {
	return theFicHandler. get_ecc();
}

uint16_t ofdmHandler::get_countryName	() {
	return theFicHandler. get_countryName ();
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

void	ofdmHandler::start_dumping	(SNDFILE *f) {
	theReader. start_dumping (f);
}

void	ofdmHandler::stop_dumping() {
	theReader. stop_dumping();
}

bool	ofdmHandler::isEvenFrame (int16_t cf, dabParams *p) {
	switch (p -> get_dabMode()) {
	   default:
	   case 1:
	      return (cf & 07) >= 4;
	   case 2:
	   case 3:
	      return (cf & 02);
	   case 4:
	      return (cf & 03) >= 2;
	}
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
	if (theEtiGenerator. start_etiGenerator (s))
	   eti_on	= true;
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

void	ofdmHandler::handle_dcRemovalSelector (bool b) {
	theReader. set_dcRemoval (b);
}
