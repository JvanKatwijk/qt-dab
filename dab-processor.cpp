#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
#include	"dab-processor.h"

#include <utility>
#include	"fic-handler.h"
#include	"msc-handler.h"
#include	"radio.h"
#include	"dab-params.h"
#include	"timesyncer.h"
//
/**
  *	\brief dabProcessor
  *	The dabProcessor class is the driver of the processing
  *	of the samplestream.
  *	It is the main interface to the qt-dab program,
  *	local are classes ofdmDecoder, ficHandler and mschandler.
  */

static	int goodFrames	= 0;
	dabProcessor::dabProcessor	(RadioInterface	*mr,
	                                 virtualInput	*theRig,
	                                 uint8_t	dabMode,
	                                 int16_t	threshold,
	                                 int16_t	diff_length,
	                                 int16_t	tii_delay,
	                                 int16_t	tii_depth,
	                                 int16_t	echo_depth,
	                                 QString	picturesPath,
	                                 RingBuffer<float> *responseBuffer,
		                         RingBuffer<std::complex<float>> *
	                                                         spectrumBuffer,
	                                 RingBuffer<std::complex<float>> *
	                                                         iqBuffer,
	                                 RingBuffer<std::complex<float>> *
	                                                         tiiBuffer
	                                 ):
	                                 params (dabMode),
	                                 myReader (mr,
	                                           theRig,
	                                           spectrumBuffer
	                                 ),
	                                 my_ficHandler (mr, dabMode),
	                                 my_mscHandler (mr, dabMode,
	                                                std::move(picturesPath)),
	                                 phaseSynchronizer (mr,
	                                                    dabMode, 
	                                                    threshold,
	                                                    diff_length,
	                                                    echo_depth,
	                                                    responseBuffer),
	                                 my_TII_Detector (dabMode, tii_depth), 
	                                 my_ofdmDecoder (mr,
	                                                 dabMode,
	                                                 theRig -> bitDepth(),
	                                                 iqBuffer) {

	this	-> myRadioInterface	= mr;
	this	-> theRig		= theRig;
	this	-> tiiBuffer		= tiiBuffer;
	this	-> T_null		= params. get_T_null();
	this	-> T_s			= params. get_T_s();
	this	-> T_u			= params. get_T_u();
	this	-> T_g			= T_s - T_u;
	this	-> T_F			= params. get_T_F();
	this	-> nrBlocks		= params. get_L();
	this	-> carriers		= params. get_carriers();
	this	-> carrierDiff		= params. get_carrierDiff();

	this	-> tii_delay		= tii_delay;
	this	-> tii_counter		= 0;

	ofdmBuffer. resize (2 * T_s);
	fineOffset			= 0;	
	coarseOffset			= 0;	
	correctionNeeded		= true;
	attempts			= 0;
	scanMode			= false;
	connect (this, SIGNAL (showCoordinates (int)),
	         mr,   SLOT   (showCoordinates (int)));
	connect (this, SIGNAL (showSecondaries (int)),
	         mr,   SLOT   (showSecondaries (int)));
	connect (this, SIGNAL (setSynced (char)),
	         myRadioInterface, SLOT (setSynced (char)));
	connect (this, SIGNAL (No_Signal_Found (void)),
	         myRadioInterface, SLOT (No_Signal_Found(void)));
	connect (this, SIGNAL (setSyncLost (void)),
	         myRadioInterface, SLOT (setSyncLost (void)));
	connect (this, SIGNAL (show_Spectrum (int)),
	         myRadioInterface, SLOT (showSpectrum (int)));
	connect (this, SIGNAL (show_tii (int)),
	         myRadioInterface, SLOT (show_tii (int)));
	connect (this, SIGNAL (show_snr (int)),
	         mr, SLOT (show_snr (int)));
	my_TII_Detector. reset();
//	the thread will be started from somewhere else
}

    dabProcessor::~dabProcessor() {
    if (isRunning()) {
	   myReader. setRunning (false);
	                                // exception to be raised
	                        	// through the getSample(s) functions.
	   msleep (100);
       while (isRunning()) {
	      usleep (100);
	   }
	}
}

/***
   *	\brief run
   *	The main thread, reading samples,
   *	time synchronization and frequency synchronization
   *	Identifying blocks in the DAB frame
   *	and sending them to the ofdmDecoder who will transfer the results
   *	Finally, estimating the small freqency error
   */
void	dabProcessor::run() {
int32_t startIndex;
int32_t		i;
std::complex<float>	FreqCorr;
timeSyncer	myTimeSyncer (&myReader);
int		attempts;

float		avgValue_nullPeriod	= 0;
float		avgValue_testPeriod	= 0;
int		testLength		= 200;

	fineOffset		= 0;
	correctionNeeded	= true;
	attempts		= 0;
	theRig  -> resetBuffer();
	coarseOffset		= 0;
	myReader. setRunning (true);	// useful after a restart
	my_mscHandler. start();
//
//	to get some idea of the signal strength
	try {
	   for (i = 0; i < T_F / 5; i ++) {
	      myReader. getSample (0);
	   }
//Initing:
notSynced:
	   setSynced (false);
	   my_TII_Detector. reset();
	   switch (myTimeSyncer. sync (T_null, T_F)) {
	      case TIMESYNC_ESTABLISHED:
	         break;			// yes, we are ready

	      case NO_DIP_FOUND:
	         if (scanMode && (++ attempts >= 5)) {
	        emit (No_Signal_Found());
	            attempts = 0;
	         }
	         goto notSynced;

	      default:			// does not happen
	      case NO_END_OF_DIP_FOUND:
	         goto notSynced;
	   }
	   myReader. getSamples (ofdmBuffer. data(),
	                        T_u, coarseOffset + fineOffset);
/**
  *	Looking for the first sample of the T_u part of the sync block.
  *	Note that we probably already had 30 to 40 samples of the T_g
  *	part
  */
	   startIndex = phaseSynchronizer. findIndex (ofdmBuffer, 3);
	   if (startIndex < 0) { // no sync, try again
	      if (!correctionNeeded) {
	         setSyncLost();
	      }
//	      fprintf (stderr, "x = %d after %d good frames\n",
//	                         startIndex, goodFrames);
//	      goodFrames	= 0;
	      goto notSynced;
	   }
	   goodFrames ++;
//	   fprintf (stderr, "startIndex = %d\n", startIndex);
	   goto SyncOnPhase;

//
//	Note: Optimization Kills
//	An old saying is that "optimization kills", and indeed, here
//	is another example.
//	Spoiled by excellent devices such as the SDRplay, I noticed that
//	at the end of recognizing a DAB frame, the only thing that has to 
//	be done is to skip T_null examples to get the start of the
//	next frame. The findIndex function then would identify the
//	precise start of the T_u part of the zero-th block.
//	However, it turns out that with devices that give more noisy
//	samples the findIndex function might give wrong results, leading
//	to loosing synchronization.
//	Therefore, a simple test is added to just verify that the "dip"
//	of the null period ends within a reasonable distance of the 
//	estimated begin, and if no such end of dip could be identified
//	within a reasonable distance, just resync.
//	we make it into a simple test, just compare the avg value of the
//	samples that are (should be) in the non-null period after the
//	null period with the avg value of the samples in the null period
//	
Check_endofNULL:

	   avgValue_testPeriod	= 0;
	   myReader. getSamples (ofdmBuffer. data(),
	                        T_u, coarseOffset + fineOffset);
//	   for (i = 100; i < 100 + testLength; i ++)
//	      avgValue_testPeriod += abs (ofdmBuffer [i]);
//	   avgValue_testPeriod	/= testLength;
//
//	It seems reasonable to expect that the avg value of a segment
//	of samples belonging to the T_g part of the first block of the
//	DAB frame is at least twice the avg value of the samples in he
//	null period
//	   if (avgValue_testPeriod < 1.7 * avgValue_nullPeriod) {
//	      fprintf (stderr, "failing %f after %d good frames\n",
//	                     avgValue_testPeriod / avgValue_nullPeriod,
//	                     goodFrames);
//	      goodFrames	= 0;
//	      goto notSynced;
//	   }
//	   fprintf (stderr, "%f \n", avgValue_testPeriod / avgValue_nullPeriod);

/**
  *	We now have to find the exact first sample of the non-null period.
  *	We use a correlation that will find the first sample after the
  *	cyclic prefix.
  */
	   startIndex = phaseSynchronizer. findIndex (ofdmBuffer, 10);
	   if (startIndex < 0) { // no sync, try again
	      if (!correctionNeeded) {
	         setSyncLost();
	      }
//	      fprintf (stderr, "x = %d after %d good frames\n",
//	                         startIndex, goodFrames);
//	      goodFrames	= 0;
	      goto notSynced;
	   }
//	   goodFrames ++;
//	   fprintf (stderr, "startIndex = %d\n", startIndex);

SyncOnPhase:

/**
  *	Once here, we are synchronized, we need to copy the data we
  *	used for synchronization for block 0
  */
	   memmove (ofdmBuffer. data(),
	        &((ofdmBuffer. data()) [startIndex]),
	                  (T_u - startIndex) * sizeof (std::complex<float>));
	   int ofdmBufferIndex	= T_u - startIndex;

//Block_0:
/**
  *	Block 0 is special in that it is used for fine time synchronization,
  *	for coarse frequency synchronization
  *	and its content is used as a reference for decoding the
  *	first datablock.
  *	We read the missing samples in the ofdm buffer
  */
	   setSynced (true);
	   myReader. getSamples (&((ofdmBuffer. data()) [ofdmBufferIndex]),
	                           T_u - ofdmBufferIndex,
	                           coarseOffset + fineOffset);
	   my_ofdmDecoder. processBlock_0 (ofdmBuffer);
	   my_mscHandler.  processBlock_0 (ofdmBuffer. data());

//	Here we look only at the block_0 when we need a coarse
//	frequency synchronization.
       correctionNeeded	= !my_ficHandler. syncReached();
	   if (correctionNeeded) {
	      int correction	=
	            phaseSynchronizer. estimate_CarrierOffset (ofdmBuffer);
	      if (correction != 100) {
	         coarseOffset	+= 0.4 * correction * carrierDiff;
	         if (abs (coarseOffset) > Khz (35))
	            coarseOffset = 0;
	      }
	   }
/**
  *	after block 0, we will just read in the other (params -> L - 1) blocks
  */
//Data_blocks:
/**
  *	The first ones are the FIC blocks these are handled within
  *	the thread executing this "task", the other blocks
  *	are passed on to be handled in the mscHandler, running
  *	in a different thread.
  *	 We immediately
  *	start with building up an average of the phase difference
  *	between the samples in the cyclic prefix and the
  *	corresponding samples in the datapart.
  */
	   std::vector<int16_t> ibits;
	   ibits. resize (2 * params. get_carriers());
	   FreqCorr	= std::complex<float> (0, 0);
	   for (int ofdmSymbolCount = 1;
	        ofdmSymbolCount < nrBlocks; ofdmSymbolCount ++) {
	      myReader. getSamples (ofdmBuffer. data(),
	                              T_s, coarseOffset + fineOffset);
	      for (i = (int)T_u; i < (int)T_s; i ++) 
	         FreqCorr += ofdmBuffer [i] * conj (ofdmBuffer [i - T_u]);

	      if (ofdmSymbolCount < 4) {
	         my_ofdmDecoder. decode (ofdmBuffer,
	                            ofdmSymbolCount, ibits. data());
	         my_ficHandler. process_ficBlock (ibits, ofdmSymbolCount);
	      }

	      my_mscHandler. process_Msc  (&((ofdmBuffer. data()) [T_g]),
	                                                    ofdmSymbolCount);
	   }
/**
  *	OK,  here we are at the end of the frame
  *	Assume everything went well and skip T_null samples
  */
	   myReader. getSamples (ofdmBuffer. data(),
	                         T_null, coarseOffset + fineOffset);
	   float sum	= 0;
	   for (i = 0; i < T_null; i ++)
	      sum += abs (ofdmBuffer [i]);
	   sum /= T_null;
	   avgValue_nullPeriod	= sum;
	   static	float snr	= 0;
	   snr = 0.9 * snr +
	     0.1 * 20 * log10 ((myReader. get_sLevel() + 0.005) / sum);
	   static int ccc	= 0;
	   if (++ccc > 10) {
	      ccc = 0;
	      show_snr ((int)snr);
	   }
/*
 *	The TII data is encoded in the null period of the
 *	odd frames 
 */
       if (params. get_dabMode() == 1) {
	  if (wasSecond (my_ficHandler. get_CIFcount(), &params)) {
	         my_TII_Detector. addBuffer (ofdmBuffer);
	         if (++tii_counter >= tii_delay) {
	            std::vector<int> secondaries;
	            secondaries =
	            my_TII_Detector. processNULL();
	            showSecondaries (-1);
	            if (secondaries. size() > 0) {
	               showCoordinates (secondaries. at (0));
	               for (i = 0; i < (int)(secondaries. size()); i ++)
	                  showSecondaries (secondaries. at (i));
	            }

	            show_tii (1);
	            tii_counter = 0;
	            my_TII_Detector. reset();
	         }
	        tiiBuffer -> putDataIntoBuffer (ofdmBuffer. data(), T_u);
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
//	It might happen that the computation of the "startIndex"
//	is - more or less - ambiguous, in which case the computation
//	of the frequency correction hopelessly fails

//	   if (!correctionNeeded && (abs (arg (FreqCorr)) > 1.8)) {
//	      fprintf (stderr, "resyncing %d %f after %d good frames\n",
//	                  startIndex, arg (FreqCorr), goodFrames);
//	      goodFrames = 0;
//	      goto notSynced;
//	   }

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

//ReadyForNewFrame:
///	and off we go, up to the next frame
	   goto Check_endofNULL;
	}
	catch (int e) {
	   fprintf (stderr, "dabProcessor is stopping\n");
	   ;
	}
	my_mscHandler.  stop();
	my_ficHandler.  stop();
}

void	dabProcessor:: reset() {
	myReader. setRunning (false);
	while (isRunning())
	   wait();
	usleep (10000);
	my_ficHandler.  reset();
	start();
}

void	dabProcessor::stop() {
	myReader. setRunning (false);
	while (isRunning())
	   wait();
	usleep (10000);
	my_ficHandler.  reset();
}

void	dabProcessor::coarseCorrectorOn() {
	correctionNeeded 	= true;
	coarseOffset	= 0;
}

void	dabProcessor::coarseCorrectorOff() {
	correctionNeeded	= false;
	theRig	-> setOffset (coarseOffset);
}

void	dabProcessor::set_scanMode	(bool b) {
	scanMode	= b;
	attempts	= 0;
}
//
//	just a convenience function
bool	dabProcessor::is_audioService	(QString &s) {
audiodata ad;
	my_ficHandler. dataforAudioService (s, &ad, 0);
	return ad. defined;
}

bool	dabProcessor::is_packetService	(QString &s) {
packetdata pd;
	my_ficHandler. dataforPacketService (s, &pd, 0);
	return pd. defined;
}

void	dabProcessor::dataforAudioService	(QString &s,
	                                          audiodata *d, int16_t c) {
	my_ficHandler. dataforAudioService (s, d, c);
}

void	dabProcessor::dataforPacketService	(QString &s,
	                                         packetdata *d, int16_t c) {
	my_ficHandler. dataforPacketService (s, d, c);
}


void	dabProcessor::reset_msc() {
	my_mscHandler. reset();
}

void    dabProcessor::set_audioChannel (audiodata *d,
	                                      RingBuffer<int16_t> *b) {
	my_mscHandler. set_Channel (d, b, (RingBuffer<uint8_t> *)nullptr);
}

void    dabProcessor::set_dataChannel (packetdata *d,
	                                      RingBuffer<uint8_t> *b) {
	my_mscHandler. set_Channel (d, (RingBuffer<int16_t> *)nullptr, b);
}


uint8_t	dabProcessor::get_ecc() {
	return my_ficHandler. get_ecc();
}

int32_t dabProcessor::get_ensembleId() {
	return my_ficHandler. get_ensembleId();
}

QString dabProcessor::get_ensembleName() {
	return my_ficHandler. get_ensembleName();
}

void	dabProcessor::clearEnsemble() {
	my_ficHandler. clearEnsemble();
}

void	dabProcessor::print_Overview () {
	my_ficHandler. print_Overview ();
}

void	dabProcessor::startDumping	(SNDFILE *f) {
	myReader. startDumping (f);
}

void	dabProcessor::stopDumping() {
	myReader. stopDumping();
}

bool	dabProcessor::wasSecond (int16_t cf, dabParams *p) {
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

