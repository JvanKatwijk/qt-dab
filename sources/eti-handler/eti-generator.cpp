#
/*
 * ## Copyright
 *
 * dabtools is written by Dave Chapman <dave@dchapman.com> 
 *
 * Large parts of the code are copied verbatim (or with trivial
 * modifications) from David Crawley's OpenDAB and hence retain his
 * copyright.
 *
 *	Parts of this software are copied verbatim (or with trivial
 *	Modifications) from David Chapman's dabtools and hence retain
 *	his copyright. In particular, the crc, descramble and init_eti
 *	functions are - apart from naming - a verbatim copy. Thanks
 *	for the nice work
 *
 *    Copyright (C) 2016, 2023
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the eti library
 *    eti library is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    eti library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with eti library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 	eti generator
 */

#include	<iostream>
#include	"dab-constants.h"
#include	"eti-generator.h"
#include	"eep-protection.h"
#include	"uep-protection.h"
#include	<memory>

#include	"crc-handlers.h"

int16_t cif_In    [55296];
int16_t	cifVector [16][55296];
uint8_t	fibVector [16][96];
bool	fibValid  [16];

#define	CUSize	(4 * 16)
//
//	For each subchannel we create a
//	deconvoluter and a descramble table up front
protection *protTable [64]	= {nullptr};
uint8_t	*descrambler [64]	= {nullptr};

int16_t	temp [55296];
const int16_t interleaveMap[] = {0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};
uint8_t	theVector [6144];

//
//	fibvector contains the processed fics, i.e ready for addition
//	to the eti frame.
//	Since there are 4 fibs, one for each CIF, the index to be used for
//	filling - and processing - indicates at the end of the frame
//	again the last fib.
//	In between there are more fib fields filled than CIFS
//
//	Since the odpprocessor (together with the decoder) takes quite an
//	amount of cycles, the eti-generation is done in a different thread
//	Note CIF counts from 0 .. 3
//
		etiGenerator::etiGenerator	(uint8_t   dabMode,
	                                        ficHandler *my_ficHandler,
	                                        uint8_t		cpuSupport):
	                                            params (dabMode) {
	this	-> my_ficHandler	= my_ficHandler;
	this	-> cpuSupport	= cpuSupport;
	index_Out		= 0;
	BitsperBlock		= 2 * params. get_carriers ();
	numberofblocksperCIF	= 18;	// mode I
	amount			= 0;
	CIFCount_hi		= -1;
	CIFCount_lo		= -1;
	etiFile			= nullptr;
	Minor			= 0;
}

		etiGenerator::~etiGenerator	() {
	reset	();
	if (etiFile != nullptr)
	   fclose (etiFile);
}
//
//	we probably need "reset" when handling a change in configuration
//
void	etiGenerator::reset	() {
	for (int i = 0; i < 64; i ++) {
           if (descrambler [i] != nullptr)
              delete descrambler [i];
           if (protTable [i] != nullptr)
              delete protTable [i];

           protTable [i]        = nullptr;
           descrambler [i]      = nullptr;
        }
	index_Out		= 0;
	BitsperBlock		= 2 * params. get_carriers ();
	numberofblocksperCIF	= 18;	// mode I
	amount			= 0;
	CIFCount_hi		= -1;
	CIFCount_lo		= -1;
	Minor			= 0;
	running			= false;
}

void	etiGenerator::newFrame	() {
}
//
//
//	we ensure that when starting, we start with a 
//	block 1
void	etiGenerator::processBlock	(std::vector <int16_t> &ibits,
	                                                      int blkno) {

	if (!running && (etiFile != nullptr) && (blkno == 1))
	   running = true;

	if (!running)
	   return;

	if (blkno < 4)
	   return;

	if (blkno == 4) {	// import fibBits
	   bool ficValid [4];
	   my_ficHandler -> getFIBBits (fibBits, ficValid);
	   for (int i = 0; i < 4; i ++) {
	      fibValid [index_Out + i] = ficValid [i];
	      for (int j = 0; j < 96; j ++) {
	         fibVector [(index_Out + i) & 017][j] = 0;
	         for (int k = 0; k < 8; k ++) {
	            fibVector [(index_Out + i) & 017][j] <<= 1;
	            fibVector [(index_Out + i) & 017][j] |=
                              (fibBits [i * 768 + 8 * j + k] & 01);
	         }
	      }
	   }
	   Minor	= 0;
	   my_ficHandler -> getCIFcount  (CIFCount_hi, CIFCount_lo);
	}
//	
//	adding the MSC blocks. Blocks 5 .. 76 are "transformed"
//	into the "soft" bits arrays
	int CIF_index	= (blkno - 4) % numberofblocksperCIF;
	memcpy (&cif_In [CIF_index * BitsperBlock],
	                   ibits. data (), BitsperBlock * sizeof (int16_t));
	if (CIF_index == numberofblocksperCIF - 1) {
	   for (int i = 0; i < 3072 * 18; i++) {
	      int index = interleaveMap [i & 017];
	      temp [i] = cifVector [(index_Out + index) & 017] [i];
	      cifVector [index_Out & 0xF] [i] = cif_In [i];
	   }
//	we have to wait until the interleave matrix is filled
	   if (amount < 15) {
	      amount ++;
	      index_Out	= (index_Out + 1) & 017;
//	Minor is introduced to inform the init_eti function
//	anout the CIF number in the dab frame, it runs from 0 .. 3
	      Minor		= 0;
	      return;		// wait until next time
	   }
//
//	Otherwise, it becomes serious
	   if ((CIFCount_hi < 0) || (CIFCount_lo < 0))
	      return;
//
//	3 steps, init the vector, add the fib and add the CIF content
	   int offset	= init_eti (theVector, CIFCount_hi,
	                                               CIFCount_lo, Minor);
	   int base	= offset;
	   memcpy (&theVector [offset], fibVector [index_Out], 96);
	   offset += 96;
//
//
//	oef, here we go for handling the CIF
	   offset	= process_CIF (temp, theVector, offset);
//
//	EOF - CRC
//	The "data bytes" are stored in the range base .. offset
	   uint16_t crc = calc_crc (&(theVector [base]), offset - base);
	   theVector [offset ++] = (crc & 0xFF00) >> 8;
	   theVector [offset ++] = crc & 0xFF;
//
//	EOF - RFU
	   theVector [offset ++] = 0xFF;
	   theVector [offset ++] = 0xFF;
//
//	TIST	- 0xFFFFFFFF means time stamp not used
	   theVector [offset ++] = 0xFF;
	   theVector [offset ++] = 0xFF;
	   theVector [offset ++] = 0xFF;
	   theVector [offset ++] = 0xFF;
//
//	Padding
	   memset (&theVector [offset], 0x55, 6144 - offset);
	   if (etiFile != nullptr)
	      fwrite (theVector, 1, 6144, etiFile);
//	at the end, go for a new eti vector
	   index_Out	= (index_Out + 1) & 017;
	   Minor ++;
	}
}

//	Copied  from dabtools:
int32_t	etiGenerator::init_eti (uint8_t* eti,
	                        int16_t CIFCount_hi,
	                        int16_t CIFCount_lo,
	                        int16_t minor) {
int	fillPointer = 0;
channel_data data;

	CIFCount_lo += minor;
	if (CIFCount_lo >= 250) {
	   CIFCount_lo = CIFCount_lo % 250;
	   CIFCount_hi ++;
	}
	if (CIFCount_hi >= 20)
	   CIFCount_hi = 20;

//	SYNC()
//	ERR
//	if (fibValid [index_Out + minor])
	   eti [fillPointer ++] = 0xFF;		// error level 0
//	else
//	   eti [fillPointer ++] = 0x0F;		// error level 2, fib errors
//	FSYNC
	if (CIFCount_lo & 1) {
	   eti [fillPointer ++] = 0xf8;
	   eti [fillPointer ++] = 0xc5;
	   eti [fillPointer ++] = 0x49;
	} else {
	   eti [fillPointer ++] = 0x07;
	   eti [fillPointer ++] = 0x3a;
	   eti [fillPointer ++] = 0xb6;
	}
//	LIDATA ()
//	FC()
	eti [fillPointer ++]	= CIFCount_lo; // FCT from CIFCount_lo
	int FICF	= 1;			// FIC present in MST
	int NST		= 0;			// number of streams
	int FL		= 0;			// Frame Length
	for (int j = 0; j < my_ficHandler -> nrChannels ();  j++) {
	   my_ficHandler -> getChannelInfo (&data, j);
	   if (data. in_use) {
	      NST++;
	      FL += (data. bitrate * 3) / 4;		// words remember
	   }
 	}
//
	FL	+= NST + 1 + 24; // STC + EOH + MST (FIC data, Mode 1!)
	eti [fillPointer ++] = (FICF << 7) | NST;
//
//	The FP is computed as remainder of the total CIFCount,
	uint8_t FP = ((CIFCount_hi * 250) + CIFCount_lo) % 8;
//
	int MID	= 0x01; // We only support Mode 1
	eti [fillPointer ++] = (FP << 5) | (MID << 3) | ((FL & 0x700) >> 8);
 	eti [fillPointer ++] = FL & 0xff;
//	Now for each of the streams in the FIC we add information
//	on how to get it
//	STC ()
	for (int j = 0; j < my_ficHandler -> nrChannels (); j ++) {
	   my_ficHandler -> getChannelInfo (&data, j);
	   if (data. in_use) {
	      int SCID	= data. id;
	      int  SAD	= data. start_cu;
	      int  TPL;
	      if (data. uepFlag) 
	         TPL = 0x10 | (data. protlev - 1);
	      else
	         TPL = 0x20 | data. protlev;
	      int STL = data. bitrate * 3 / 8;
	      eti [fillPointer ++] = (SCID << 2) | ((SAD & 0x300) >> 8);
	      eti [fillPointer ++] = SAD & 0xFF;
	      eti [fillPointer ++] = (TPL << 2) | ((STL & 0x300) >> 8);
	      eti [fillPointer ++] = STL & 0xFF;
	   }
	}
//	EOH ()
//	MNSC
	eti [fillPointer ++] = 0xFF;
	eti [fillPointer ++] = 0xFF;
//	HCRC
	int HCRC = calc_crc (&eti [4], fillPointer - 4);
//	HCRC =~ HCRC;
	eti [fillPointer ++] = (HCRC & 0xff00) >> 8;
	eti [fillPointer ++] = HCRC & 0xff;

	return fillPointer;
}

//	In process_CIF we iterate over the data in the CIF and map that
//	upon a segment in the eti vector
//
//	Since from the subchannel data we know the location in
//	the input vector, the output vector and the
//	parameters for deconvolution, we can do
//	the processing in parallel. So, for each subchannel
//	we just launch a task
class parameter {
public:
	int16_t	*input;
	bool	uepFlag;
	int	bitRate;
	int	protLevel;
	int	start_cu;
	int	size;
	uint8_t	*output;
};

int32_t	etiGenerator::process_CIF (int16_t *input,
	                           uint8_t *output, int32_t offset) {
uint8_t	shiftRegister [9];
std::vector<parameter *> theParameters;

	for (int i = 0; i < my_ficHandler -> nrChannels (); i ++) {
	   channel_data data;
	   my_ficHandler -> getChannelInfo (&data, i);
	   if (data. in_use) {
	      parameter *t	= new parameter;
	      t -> input	= input;
	      t -> uepFlag	= data. uepFlag;
	      t -> bitRate	= data. bitrate;
	      t -> protLevel	= data. protlev;
	      t -> start_cu	= data. start_cu;
	      t -> size		= data. size;
	      t -> output	= &output [offset];
	      offset 		+= data. bitrate * 24 / 8;

	      if (protTable [i] == nullptr) {
	         if (t -> uepFlag)
	            protTable [i] = new uep_protection (t -> bitRate,
	                                                t -> protLevel,
	                                                cpuSupport);
	         else
	            protTable [i] = new eep_protection (t -> bitRate,
	                                                t -> protLevel,
	                                                cpuSupport);
	         
	         memset (shiftRegister, 1, 9);
	         descrambler [i] = new uint8_t [24 * t -> bitRate];

	         for (int j = 0; j < 24 * t -> bitRate; j ++) {
	            uint8_t b = shiftRegister [8] ^ shiftRegister [4];
	            for (int k = 8; k > 0; k--)
	               shiftRegister [k] = shiftRegister [k - 1];
	            shiftRegister [0] = b;
	            descrambler [i] [j] = b;
	         }
              }
//	we need to save a reference to the parameters
//	since we have to delete the instance later on
	      process_subCh (i, t, protTable [i], descrambler [i]);
	   }
	}
	return offset;
}

void	etiGenerator::process_subCh (int nr, parameter *p,
	                             protection *prot,
	                             uint8_t *desc) {
	(void)nr;
	std::unique_ptr<uint8_t[]> outVector { new uint8_t[24 * p->bitRate] };
	if (!outVector) {
	   std::cerr << "process_subCh - alloc fail";
	   return;
	}

	memset (outVector.get(), 0, sizeof (uint8_t) * 24 * p -> bitRate);

	prot -> deconvolve (&p -> input [p -> start_cu * CUSize],
	                                 p -> size * CUSize,
	                                 outVector.get());
//
	for (int j = 0; j < 24 *p -> bitRate; j ++) {
	   outVector [j] ^= desc [j];
        }
//
//	and the storage:
	for (int j = 0; j < 24 * p -> bitRate / 8; j ++) {
	   int temp = 0;
	   for (int k = 0; k < 8; k ++)
	      temp = (temp << 1) | (outVector [j * 8 + k] & 01);
	   p -> output [j] = temp;
	}

}

void	etiGenerator::postProcess (uint8_t *theVector, int32_t offset){
	(void)theVector;
	(void)offset;
}

bool	etiGenerator::start_etiGenerator	(const QString &f) {
	reset ();
	etiFile	= fopen (f. toUtf8 (). data (), "wb");
	return etiFile != nullptr;
}

void	etiGenerator::stop_etiGenerator		() {
	if (etiFile != nullptr) {
	   fclose (etiFile);
	}
	etiFile	= nullptr;
	running	= false;
}

