#
/*
 *    Copyright (C) 2013 .. 2024
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
 *
 ************************************************************************
 *	may 15 2015. A real improvement on the code
 *	is the addition from Stefan Poeschel to create a
 *	header for the aac that matches, really a big help!!!!
 *
 *	(2019:)Furthermore, the code in the "build_aacFile" function is
 *	his as well. Chapeau!
 ************************************************************************
 */
#include	"mp4processor.h"
#include	"radio.h"

#include	"crc-handlers.h"
#include	<cstring>
#include	"charsets.h"
#include	"pad-handler.h"
#include	"bitWriter.h"

//
/**
  *	\class mp4Processor is the main handler for the aac frames
  *	the class proper processes input and extracts the aac frames
  *	that are processed by the "faadDecoder" class
  */
	mp4Processor::mp4Processor (RadioInterface	*mr,
	                            int16_t		bitRate,
	                            RingBuffer<complex16> *b,
	                            RingBuffer<uint8_t> *frameBuffer,
	                            FILE		*dump,
	                            bool		backgroundFlag):
	                                my_padhandler (mr, backgroundFlag),
 	                                my_rsDecoder (8, 0435, 0, 1, 10),
	                                aacDecoder (mr, b) {

	myRadioInterface	= mr;
	this	-> bitRate	= bitRate;	// input rate
	this	-> frameBuffer	= frameBuffer;
	this	-> dump		= dump;
	connect (this, &mp4Processor::show_frameErrors,
	         mr, &RadioInterface::show_frameErrors);
	connect (this, &mp4Processor::show_rsErrors,
	         mr, &RadioInterface::show_rsErrors);
	connect (this, &mp4Processor::show_aacErrors,
	         mr, &RadioInterface::show_aacErrors);
	connect (this, &mp4Processor::isStereo,
	         mr, &RadioInterface::setStereo);
	connect (this, &mp4Processor::newFrame,
	         mr, &RadioInterface::newFrame);
	connect (this, &mp4Processor::show_rsCorrections,
	         mr, &RadioInterface::show_rsCorrections);

	superFramesize		= 110 * (bitRate / 8);
	RSDims			= bitRate / 8;
	frameBytes. resize (RSDims * 120);	// input
	outVector . resize (RSDims * 110);
	blockFillIndex		= 0;
	blocksInBuffer		= 0;
	frameCount		= 0;
	frameErrors		= 0;
	aacErrors		= 0;
	crcErrors		= 0;
	aacFrames		= 0;
	successFrames		= 0;
	rsErrors		= 0;
	totalCorrections	= 0;
	goodFrames		= 0;
}

	mp4Processor::~mp4Processor () {
}
/**
  *	\brief addtoFrame
  *
  *	a DAB+ superframe consists of 5 consecutive DAB frames 
  *	we add vector for vector to the superframe. Once we have
  *	5 lengths of "old" frames, we check
  *	Note that the packing in the entry vector is still one bit
  *	per Byte, nbits is the number of Bits (i.e. containing bytes)
  *	the function adds nbits bits, packed in bytes, to the frame
  */
void	mp4Processor::addtoFrame (const std::vector<uint8_t> &V) {
int16_t	nbits	= 24 * bitRate;
uint8_t	temp	= 0;

	for (int i = 0; i < nbits / 8; i ++) {	// in bytes
	   temp = 0;
	   for (int j = 0; j < 8; j ++)
	      temp = (temp << 1) | (V [i * 8 + j] & 01);
	   frameBytes [blockFillIndex * nbits / 8 + i] = temp;
	}
//
	blocksInBuffer ++;
	blockFillIndex = (blockFillIndex + 1) % 5;
//
/**
  *	we take the last five blocks to look at
  */
	if (blocksInBuffer >= 5) {
///	first, we show the "successrate"
	   if (++frameCount >= 25) {
	      frameCount = 0;
	      show_frameErrors (frameErrors);
	      frameErrors = 0;
	   }
/**
  *	starting for real: check the fire code
  *	if the firecode is OK, we handle the frame
  *	and adjust the buffer here for the next round
  *	if the firecode check fails, we shift one block
  */
	   handleRS (frameBytes, blockFillIndex * nbits / 8,
	             outVector, frameErrors, rsErrors);
	   if (frameErrors > 0) {	// cannot fix the potential frame 
	      blocksInBuffer = 4;
	      return;
	   }
	   
	   if (!fc. check (outVector. data ())) {
	      blocksInBuffer = 4;
	      return;
	   }

	   if (!processSuperframe (outVector)) {
	      frameErrors ++;
	      blocksInBuffer = 0;
	      return;
	   }

//	when here, both firecode and superframe are OK
//	since we processed a full cycle of 5 blocks, we just start a
//	new sequence, beginning with block blockFillIndex
	   blocksInBuffer	= 0;
	   totalCorrections += rsErrors;
	   if (++ goodFrames >= 100) {
	      show_rsCorrections (totalCorrections, crcErrors);
	      totalCorrections = 0;
	      goodFrames = 0;
	   }
	   if (++successFrames > 25) {
	      show_rsErrors (rsErrors);
	      successFrames	= 0;
	      rsErrors	= 0;
	   }
	}
}
/**
  *	\brief processSuperframe
  *
  *	First, we know the firecode checker gave green light
  *	We correct the errors using RS
  */

void	mp4Processor::handleRS (const std::vector<uint8_t> &frameBytes, 
	                        int16_t base, 
	                        std::vector<uint8_t> &outVector,
	                        int16_t &errorLines, int16_t &repairs) {
uint8_t		rsIn	[120];
uint8_t		rsOut	[110];
int16_t		ler;
	errorLines	= 0;
	repairs		= 0;
/**
  *	apply reed-solomon error repar
  *	OK, what we now have is a vector with RSDims * 120 uint8_t's
  *	the superframe, containing parity bytes for error repair
  *	take into account the interleaving that is applied.
  */
	for (int j = 0; j < RSDims; j ++) {
	   for (int k = 0; k < 120; k ++) 
	      rsIn [k] = frameBytes [(base + j + k * RSDims) % (RSDims * 120)];
	   ler = my_rsDecoder. dec (rsIn, rsOut, 135);
	   for (int k = 0; k < 110; k ++) 
	      outVector [j + k * RSDims] = rsOut [k];
	   if (ler < 0)
	      errorLines ++;
	   else
	      repairs += ler;
	}
}

bool	mp4Processor::processSuperframe (std::vector<uint8_t> &outVector){
uint8_t		num_aus;
int		tmp;
stream_parms    streamParameters;

//	bits 0 .. 15 is firecode
//	bit 16 is unused
	streamParameters. dacRate = (outVector [2] >> 6) & 01;	// bit 17
	streamParameters. sbrFlag = (outVector [2] >> 5) & 01;	// bit 18
	streamParameters. aacChannelMode
	                          = (outVector [2] >> 4) & 01;	// bit 19
	streamParameters. psFlag  = (outVector [2] >> 3) & 01;	// bit 20
	streamParameters. mpegSurround	= (outVector [2] & 07);	// bits 21 .. 23
//
//	added for the aac file writer
	streamParameters. CoreSrIndex	=
	              streamParameters. dacRate ?
	                            (streamParameters. sbrFlag ? 6 : 3) :
	                            (streamParameters. sbrFlag ? 8 : 5);
	streamParameters. CoreChConfig	=
	              streamParameters. aacChannelMode ? 2 : 1;

	streamParameters. ExtensionSrIndex =
	              streamParameters. dacRate ? 3 : 5;

	switch (2 * streamParameters. dacRate + streamParameters. sbrFlag) {
	  default:		// cannot happen
	   case 0:
	      num_aus = 4;
	      au_start [0] = 8;
	      au_start [1] = outVector [3] * 16 + (outVector [4] >> 4);
	      au_start [2] = (outVector [4] & 0xf) * 256 +
	                      outVector [5];
	      au_start [3] = outVector [6] * 16 +
	                     (outVector [7] >> 4);
	      au_start [4] = 110 *  (bitRate / 8);
	      break;
//
	   case 1:
	      num_aus = 2;
	      au_start [0] = 5;
	      au_start [1] = outVector [3] * 16 +
	                     (outVector [4] >> 4);
	      au_start [2] = 110 *  (bitRate / 8);
	      break;
//
	   case 2:
	      num_aus = 6;
	      au_start [0] = 11;
	      au_start [1] = outVector [3] * 16 + (outVector [4] >> 4);
	      au_start [2] = (outVector [4] & 0xf) * 256 + outVector [ 5];
	      au_start [3] = outVector [6] * 16 + (outVector [7] >> 4);
	      au_start [4] = (outVector [7] & 0xf) * 256 + outVector [8];
	      au_start [5] = outVector [9] * 16 + (outVector [10] >> 4);
	      au_start [6] = 110 *  (bitRate / 8);
	      break;
//
	   case 3:
	      num_aus = 3;
	      au_start [0] = 6;
	      au_start [1] = outVector [3] * 16 + (outVector [4] >> 4);
	      au_start [2] = (outVector [4] & 0xf) * 256 + outVector [5];
	      au_start [3] = 110 * (bitRate / 8);
	      break;
	}
/**
  *	OK, the result is N * 110 * 8 bits (still single bit per byte!!!)
  *	extract the AU's, and prepare a buffer,  with the sufficient
  *	lengthy for conversion to PCM samples
  */
	for (int i = 0; i < num_aus; i ++) {
	   int16_t	aac_frame_length;

///	sanity check 1
	   if (au_start [i + 1] < au_start [i]) {
//	      fprintf (stderr, "%d %d (%d)\n", au_start [i], au_start [i + 1], i);
//	should not happen, all errors were corrected
	      return false;
	   }

	   aac_frame_length = au_start [i + 1] - au_start [i] - 2;
//	just a sanity check
	   if ((aac_frame_length >=  960) || (aac_frame_length < 0)) {
//	      fprintf (stderr, "aac_frame_length = %d\n", aac_frame_length);
//	      return false;
	   }

//	but first the crc check
	   if (check_crc_bytes (&outVector [au_start [i]],
	                                aac_frame_length)) {
//
//	first prepare dumping
//
//	It is a litle tricky, but we deal with foreground and background
//	tasks. For the foreground task, the user might - or might not -
//	push the button to store the aac data,
//	for a background task - indicated by having the var "dump" not NULL,
//	the aac output is always written to a file
	      std::vector<uint8_t> fileBuffer;
	      int segmentSize =
	              build_aacFile (aac_frame_length,
	                             &streamParameters,
	                             &(outVector [au_start [i]]),
	                             fileBuffer);
	      if (dump == nullptr) {
	         frameBuffer -> putDataIntoBuffer (fileBuffer. data (),
	                                                  segmentSize);
	         newFrame (segmentSize);
	      }
	      else
	         fwrite (fileBuffer. data (), 1, segmentSize, dump);

//	first handle the pad data if any
	      if (dump == nullptr) {
	         if (((outVector [au_start [i + 0]] >> 5) & 07) == 4) {
	            int16_t count = outVector [au_start [i] + 1];
	            auto  *buffer = dynVec (uint8_t, count);
//	            uint8_t  *buffer = (uint8_t *) alloca (count * sizeof (uint8_t));
	            memcpy (buffer, &outVector [au_start [i] + 2], count);
	            uint8_t L0	= buffer [count - 1];
	            uint8_t L1	= buffer [count - 2];
	            my_padhandler. processPAD (buffer, count - 3, L1, L0);
	         }
	         else  {// no PAD data, so no dynamic label;
	         }
//
//	then handle the audio
#ifdef	__WITH_FDK_AAC__
	         tmp = aacDecoder. MP42PCM (&streamParameters, 
	                                      fileBuffer. data (), 
	                                      segmentSize);
#else
	         uint8_t theAudioUnit [2 * 960 + 10];	// sure, large enough

	         memcpy (theAudioUnit,
	                    &outVector [au_start [i]], aac_frame_length);
	         memset (&theAudioUnit [aac_frame_length], 0, 10);

	         tmp = aacDecoder. MP42PCM (&streamParameters,
	                                      theAudioUnit,
	                                      aac_frame_length);
#endif
	         emit isStereo ((streamParameters. aacChannelMode == 1) ||
	                        (streamParameters. psFlag == 1));

	         if (tmp <= 0) 
	            aacErrors ++;
	         if (++aacFrames > 25) {
	            show_aacErrors (aacErrors);
	            aacErrors	= 0;
	            aacFrames	= 0;
	         }
	      }
	   }
	   else {
	      crcErrors ++;
	   }
//
//	what would happen if the errors were in the 10 parity bytes
//	rather than in the 110 payload bytes?
	}
	return true;
}

int	mp4Processor::build_aacFile (int16_t aac_frame_len,
	                             stream_parms *sp,
	                             uint8_t *data,
	                             std::vector<uint8_t> &fileBuffer) {
BitWriter	au_bw;

	au_bw. AddBits (0x2B7, 11);	// syncword
	au_bw. AddBits (    0, 13);	// audioMuxLengthBytes - written later
//	AudioMuxElement(1)

	au_bw. AddBits (    0, 1);	// useSameStreamMux
//	StreamMuxConfig()

	au_bw. AddBits (    0, 1);	// audioMuxVersion
	au_bw. AddBits (    1, 1);	// allStreamsSameTimeFraming
	au_bw. AddBits (    0, 6);	// numSubFrames
	au_bw. AddBits (    0, 4);	// numProgram
	au_bw. AddBits (    0, 3);	// numLayer

	if (sp  -> sbrFlag) {
	   au_bw. AddBits (0b00101, 5); // SBR
	   au_bw. AddBits (sp -> CoreSrIndex, 4); // samplingFrequencyIndex
	   au_bw. AddBits (sp -> CoreChConfig, 4); // channelConfiguration
	   au_bw. AddBits (sp -> ExtensionSrIndex, 4);	// extensionSamplingFrequencyIndex
	   au_bw. AddBits (0b00010, 5);		// AAC LC
	   au_bw. AddBits (0b100, 3);	// GASpecificConfig() with 960 transform
	} else {
	   au_bw. AddBits (0b00010, 5); // AAC LC
	   au_bw. AddBits (sp -> CoreSrIndex, 4); // samplingFrequencyIndex
	   au_bw. AddBits (sp -> CoreChConfig, 4); // channelConfiguration
	   au_bw. AddBits (0b100, 3);	// GASpecificConfig() with 960 transform
}

	au_bw. AddBits (0b000, 3);	// frameLengthType
	au_bw. AddBits (0xFF, 8);	// latmBufferFullness
	au_bw. AddBits (   0, 1);	// otherDataPresent
	au_bw. AddBits (   0, 1);	// crcCheckPresent

//	PayloadLengthInfo()
	for (size_t i = 0; i < (size_t)(aac_frame_len / 255); i++)
	   au_bw. AddBits (0xFF, 8);
	au_bw. AddBits (aac_frame_len % 255, 8);

	au_bw. AddBytes (data, aac_frame_len);
	au_bw. WriteAudioMuxLengthBytes ();
	fileBuffer	= au_bw. GetData ();
	return fileBuffer. size ();
}

