#
/*
 *    Copyright (C) 2017 .. 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the dab-2
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2 if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	"dab-processor.h"
#include	"fic-handler.h"
#include	"msc-handler.h"
#include	"radio.h"
#include	"dab-params.h"
/**
  *	\brief dabProcessor
  *	The dabProcessor class is the driver of the processing
  *	of the samplestream.
  *	It is the main interface to the qt-dab program,
  *	local are classes ofdmDecoder, ficHandler and mschandler.
  */
#define BUFSIZE 64
#define BUFMASK (64 - 1)
#define	N	5


static	float	avg_dipValue	= 0;
static	float	avg_signalValue	= 0;
static  inline
int16_t valueFor (int16_t b) {
int16_t res     = 1;
        while (--b > 0)
           res <<= 1;
        return res;
}

	dabProcessor::dabProcessor	(RadioInterface	*mr,
	                                 processParams *p):
	                                 params (p -> dabMode),
	                                 my_ficHandler (mr, p ->  dabMode),
	                                 my_mscHandler (mr, p -> dabMode,
	                                                p -> frameBuffer),
	                                 phaseSynchronizer (mr, p),
	                                 my_TII_Detector (dabMode,
	                                                  p -> tii_depth),
	                                 my_ofdmDecoder (mr,
	                                                 p -> dabMode,
	                                                 12,
	                                                 p -> iqBuffer) {
int32_t	i;

	this	-> myRadioInterface	= mr;
	this	-> theParams		= p;
	tii_counter			= 0;
        this    -> spectrumBuffer       = p -> spectrumBuffer;
	this	-> tiiBuffer		= p -> tiiBuffer;
	this	-> dabMode		= p -> dabMode;
	this	-> T_null		= params. get_T_null ();
	this	-> T_s			= params. get_T_s ();
	this	-> T_u			= params. get_T_u ();
	this	-> T_g			= T_s - T_u;
	this	-> T_F			= params. get_T_F ();
	this	-> nrBlocks		= params. get_L ();
	this	-> carriers		= params. get_carriers ();
	this	-> carrierDiff		= params. get_carrierDiff ();
	processorMode			= START;
	nullCount			= 0;
	ofdmBuffer. resize (2 * T_s);
	ofdmBufferIndex			= 0;
	avgSignalValue			= 0;
	avgLocalValue			= 0;
	counter				= 0;
	sampleCounter			= 0;
	nrFrames			= 0;
	dataBuffer. resize (BUFSIZE);
	memset (dataBuffer. data (), 0, BUFSIZE * sizeof (float));
	bufferP				= 0;
	fineOffset			= 0;	
	coarseOffset			= 0;	
	totalOffset			= 0;
	correctionNeeded		= true;

	ibits. resize (2 * carriers);

//	for the spectrum display we need:
	bufferSize              = 32768;
        localBuffer. resize (bufferSize);
        localCounter            = 0;


	connect (this, SIGNAL (show_snr (float)),
	         myRadioInterface, SLOT (show_snr (float)));
	connect (this, SIGNAL (show_clockErr (int)),
	         mr, SLOT (show_clockError (int)));
	connect (this, SIGNAL (setSynced (bool)),
	         myRadioInterface, SLOT (setSynced (bool)));
	connect (this, SIGNAL (set_freqOffset (int)),
	         myRadioInterface, SLOT (set_CorrectorDisplay (int)));
        connect (this, SIGNAL (show_Spectrum (int)),
                 mr, SLOT (showSpectrum (int)));
	connect (this, SIGNAL (show_tii (int, int)),
                 myRadioInterface, SLOT (show_tii (int, int)));
	connect (this, SIGNAL (No_Signal_Found (void)),
	         myRadioInterface, SLOT (No_Signal_Found (void)));
	my_TII_Detector. reset ();
}

	dabProcessor::~dabProcessor	(void) {
}
//
//	Since in this implementation, the callback of the device
//	is the driving force, i.e. pumping symbols into the system,
//	the basic interpretation is using an explicit state-based
//	approach

int	dabProcessor::addSymbol	(std::complex<float> *buffer, int count) {
int	retValue	= GO_ON;		// default

	for (int i = 0; i < count; i ++) {
	   std::complex<float> symbol = buffer [i];
	   avgSignalValue	= 0.9999 * avgSignalValue +
	                                   0.0001 * jan_abs (symbol);
	   dataBuffer [bufferP] = jan_abs (symbol);
	   avgLocalValue	+= jan_abs (symbol) -
	                      dataBuffer [(bufferP - 50) & BUFMASK];
	   bufferP		= (bufferP + 1) & BUFMASK;
	   sampleCounter ++;

	   if (localCounter < bufferSize)
	      localBuffer [localCounter ++] = symbol;
	   sampleCount ++;

	   if (++sampleCount > INPUT_RATE / N) {
	      sampleCount	= 0;
	      spectrumBuffer -> putDataIntoBuffer (localBuffer. data (),
	                                           localCounter);
	      show_Spectrum	(localCounter);
	      localCounter = 0;
	   }

	   switch (processorMode) {
	      default:
	      case START:
	         avgSignalValue		= 0;
	         avgLocalValue		= 0;
	         counter		= 0;
	         dipValue		= 0;
	         dipCnt			= 0;
	         fineOffset		= 0;
	         correctionNeeded	= true;
	         coarseOffset		= 0;
	         totalOffset		= 0;
	         attempts		= 0;
	         memset (dataBuffer. data (), 0, BUFSIZE * sizeof (float));
	         bufferP		= 0;
	         processorMode		= INIT;
	         break;

	      case INIT:
	         if (++counter >= 2 * T_F) {
	            processorMode	= LOOKING_FOR_DIP;
//	         retValue	= INITIAL_STRENGTH;	
	            setSynced	(false);
	            counter	= 0;
	         }
	         break;
//
//	After initialization, we start looking for a dip,
//	After recognizing a frame, we pass this and continue
//	at end of dip
	      case LOOKING_FOR_DIP:
	         counter	++;
	         if (avgLocalValue / 50 < avgSignalValue * 0.45) {
	            processorMode	= DIP_FOUND;
	            dipValue		= 0;
	            dipCnt		= 0;
	         }
	         else	
	         if (counter > T_F) {
	            counter	= 0;
	            attempts ++;
	            if (attempts > 5) {
	               emit No_Signal_Found ();
	               processorMode	= START;
	            }
	            else {
	               counter		= 0;
	               processorMode	= INIT;
	            }
	         }
	         break;
	         
	      case DIP_FOUND:
	         dipValue		+= jan_abs (symbol);
	         dipCnt		++;
	         if (avgLocalValue / BUFSIZE > avgSignalValue * 0.8) {
	            dipValue		/= dipCnt;
	            avg_dipValue	= 0.9 * avg_dipValue + 0.1 * dipValue;
	            avg_signalValue	= 0.9 * avg_signalValue + 0.1 * avgSignalValue;
	            retValue		= DEVICE_UPDATE;
	            dipValue		= 0;
	            processorMode  	= END_OF_DIP;
	            ofdmBufferIndex	= 0;
	            totalSamples	= 0;
	            frameCount		= 0;
	         }
	         else 
	         if (dipCnt > T_null + 100) {	// no luck here
	            dipCnt		= 0;
	            attempts ++;
	            if (attempts > 5) {
	               emit No_Signal_Found ();
	               processorMode       = START;
	            }
	            else {
	               counter		= 0;
	               processorMode       = INIT;
	            }
	         }
	         break;

	      case END_OF_DIP:
	         ofdmBuffer [ofdmBufferIndex ++] = symbol;
	         if (ofdmBufferIndex >= T_u) {
	            int startIndex =
	                  phaseSynchronizer. findIndex (ofdmBuffer, 3);
	            if (startIndex < 0) {		// no sync
	               if (attempts > 5) {
	                  emit No_Signal_Found ();
                          processorMode       = START;
	                  break;
                       }
	               else {
	                  processorMode = LOOKING_FOR_DIP;
	                  break;
	               }
	            }
	            attempts	= 0;	// we made it!!!
//	            sampleCounter = sampleCounterCounter - T_u + startIndex;
	            sampleCounter = T_u - startIndex;
	            memmove (ofdmBuffer. data (),
	                     &((ofdmBuffer. data ()) [startIndex]),
                           (T_u - startIndex) * sizeof (std::complex<float>));
	            ofdmBufferIndex  = T_u - startIndex;
	            processorMode	= GO_FOR_BLOCK_0;
	            setSynced (true);
	         }
	         break;

	      case TO_NEXT_FRAME:
	         ofdmBuffer [ofdmBufferIndex ++] = symbol;
	         if (ofdmBufferIndex >= T_u) {
	            int startIndex = phaseSynchronizer. findIndex (ofdmBuffer, 10);
	            if (startIndex < 0) {		// no sync
	               if (attempts > 5) {
	                  emit No_Signal_Found ();
                          processorMode       = START;
	                  break;
	               }
	               else {
	                  processorMode = LOOKING_FOR_DIP;
	                  break;
	               }
	            }

	            attempts	= 0;	// we made it!!!
	            sampleCounter	= sampleCounter - T_u + startIndex;
	            totalSamples	+= sampleCounter;
	            frameCount ++;
	            if (frameCount >= 10) {
	               show_clockErr (totalSamples - frameCount * 196608);
                       totalSamples = 0;
                       frameCount = 0;
                    }
	           
	            sampleCounter	= T_u - startIndex;
	            memmove (ofdmBuffer. data (),
	                     &((ofdmBuffer. data ()) [startIndex]),
                           (T_u - startIndex) * sizeof (std::complex<float>));
	            ofdmBufferIndex  = T_u - startIndex;
	            processorMode	= GO_FOR_BLOCK_0;
	         }
	         break;

	      case GO_FOR_BLOCK_0:
	         ofdmBuffer [ofdmBufferIndex] = symbol;
	         if (++ofdmBufferIndex < T_u)
	            break;

	         my_ofdmDecoder. processBlock_0 (ofdmBuffer);
	         my_mscHandler.  processBlock_0 (ofdmBuffer. data ());
//      Here we look only at the block_0 when we need a coarse
//      frequency synchronization.
	         correctionNeeded     = !my_ficHandler. syncReached ();
	         if (correctionNeeded) {
	            int correction    =
                        phaseSynchronizer. estimate_CarrierOffset (ofdmBuffer);
	            if (correction != 100) {
	               coarseOffset   = correction * carrierDiff;
	               totalOffset	+= coarseOffset;
	               if (abs (totalOffset) > Khz (25)) {
	                  totalOffset	= 0;
	                  coarseOffset	= 0;
	               }
	            }
	         }
	         else
	            coarseOffset	= 0;
//
//	and prepare for reading the data blocks
	         FreqCorr		= std::complex<float> (0, 0);
	         ofdmSymbolCount	= 1;
	         ofdmBufferIndex	= 0;
	         processorMode	= BLOCK_READING;
	         break;

	      case BLOCK_READING:
	         ofdmBuffer [ofdmBufferIndex ++] = symbol;
	         if (ofdmBufferIndex < T_s) 
	            break;

	         for (int i = (int)T_u; i < (int)T_s; i ++)
	            FreqCorr += ofdmBuffer [i] * conj (ofdmBuffer [i - T_u]);
	         if (ofdmSymbolCount < 4) {
	            my_ofdmDecoder. decode (ofdmBuffer,
	                                    ofdmSymbolCount, ibits. data ());
	            my_ficHandler. process_ficBlock (ibits, ofdmSymbolCount);
	         }

	         my_mscHandler. process_Msc  (&((ofdmBuffer. data ()) [T_g]),
	                                      ofdmSymbolCount);
	         ofdmBufferIndex	= 0;
	         if (++ofdmSymbolCount >= nrBlocks) {
	            processorMode	= END_OF_FRAME;
	         }
	         break;

	      case END_OF_FRAME:
	         fineOffset = arg (FreqCorr) / M_PI * carrierDiff / 2;

	         if (fineOffset > carrierDiff / 2) {
	            coarseOffset += carrierDiff;
	            fineOffset -= carrierDiff;
	         }
	         else
	         if (fineOffset < -carrierDiff / 2) {
	            coarseOffset -= carrierDiff;
	            fineOffset += carrierDiff;
	         }
//
//	Once here, we are - without even looking - sure
//	that we are in a dip period
	         processorMode	= PREPARE_FOR_SKIP_NULL_PERIOD;
	         break;
//
//	here, we skip the next null period
	      case PREPARE_FOR_SKIP_NULL_PERIOD:
	         nullCount		= 0;
	         dipValue		= jan_abs (symbol);
	         ofdmBuffer [nullCount ++] = symbol;
	         processorMode	= SKIP_NULL_PERIOD;
	         break;

	      case SKIP_NULL_PERIOD:
	         ofdmBuffer [nullCount] = symbol;
	         dipValue		+= jan_abs (symbol);
	         nullCount ++;
	         if (nullCount >= T_null - 1) {
	            processorMode = TO_NEXT_FRAME;
	            dipValue	/= T_null;
	            avg_dipValue	= 0.9 * avg_dipValue + 0.1 * dipValue;
	            avg_signalValue	= 0.9 * avg_signalValue + 0.1 * avgSignalValue;
	            if (++nrFrames >= 10) {
	               set_Values (coarseOffset + fineOffset,
	                           avg_dipValue, avg_signalValue);
	               coarseOffset	= 0;
	               fineOffset	= 0;
	               nrFrames 	= 0;
	               float snr = 20 * log10 (avg_signalValue / avg_dipValue);
	               show_snr (snr);
	            }
	            retValue	= DEVICE_UPDATE;
	            dipValue	= 0;
	            handle_tii_detection (ofdmBuffer);
	         }
	         break;
	   }
	}
	return retValue;
}

void	dabProcessor:: reset	(void) {
	processorMode		= START;
	nullCount		= 0;
	correctionNeeded	= true;
	my_ficHandler.  reset ();
	my_mscHandler. reset_Channel ();
	my_mscHandler. reset_Buffers ();
}


void    dabProcessor::set_tiiDetectorMode       (bool b) {
        my_TII_Detector. setMode (b);
}

void	dabProcessor::stop	(void) {
	my_ficHandler.  reset ();
}

void	dabProcessor::start	() {
	processorMode		= START;
	nullCount		= 0;
	correctionNeeded	= true;
	transmitters. clear ();
	my_ficHandler.  reset ();
	my_mscHandler. reset_Channel ();
}

void	dabProcessor::coarseCorrectorOn (void) {
	correctionNeeded 	= true;
	coarseOffset		= 0;
}

void	dabProcessor::coarseCorrectorOff (void) {
	correctionNeeded	= false;
}

//	just a convenience functions

QString	dabProcessor::findService		(uint32_t SId, int SCIds) {
	return my_ficHandler. findService (SId, SCIds);
}

void	dabProcessor::getParameters		(const QString &s,
	                                         uint32_t *p_SId, int*p_SCIds) {
	my_ficHandler. getParameters (s, p_SId, p_SCIds);
}

std::vector<serviceId>	dabProcessor::getServices	(int n) {
	return my_ficHandler. getServices (n);
}

bool	dabProcessor::is_audioService	(const QString &s) {
audiodata ad;
	my_ficHandler. dataforAudioService (s, &ad);
	return ad. defined;
}

bool	dabProcessor::is_packetService	(const QString &s) {
packetdata pd;
	my_ficHandler. dataforPacketService (s, &pd, 0);
	return pd. defined;
}

void	dabProcessor::dataforAudioService	(const QString &s,
	                                         audiodata *d) {
	my_ficHandler. dataforAudioService (s, d);
}

void	dabProcessor::dataforPacketService	(const QString &s,
	                                         packetdata *pd,
	                                         int16_t compnr) {
	my_ficHandler. dataforPacketService (s, pd, compnr);
}

void	dabProcessor::reset_Services () {
	my_mscHandler. reset_Channel ();
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

//void	dabProcessor::clearEnsemble() {
//	my_ficHandler. clearEnsemble();
//}

bool    dabProcessor::wasSecond (int16_t cf, dabParams *p) {
        switch (p -> get_dabMode ()) {
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

void	dabProcessor::handle_tii_detection
	                      (std::vector<std::complex<float>> b) {
	if (dabMode != 1)
	   return;
	if (wasSecond (my_ficHandler. get_CIFcount(), &params)) {
	   my_TII_Detector. addBuffer (b);
	   if (++tii_counter >= theParams -> tii_delay) {
	      uint16_t res = my_TII_Detector. processNULL ();
	      if (res != 0) {
	         uint8_t mainId   = res >> 8;
	         uint8_t subId    = res & 0xFF;
	         tiiBuffer -> putDataIntoBuffer (ofdmBuffer. data (), T_u);
	         show_tii (mainId, subId);
              }
	      tii_counter = 0;
	      my_TII_Detector. reset ();
	   }
	}
}

void	dabProcessor::set_scanMode	(bool b) {
	(void)b;
}
