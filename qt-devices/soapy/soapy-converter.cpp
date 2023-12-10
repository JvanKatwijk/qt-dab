

#include	"soapy-converter.h"

	soapyConverter::soapyConverter
	                        (RingBuffer<std::complex<float>> *outBuffer){
	this	-> outBuffer	= outBuffer;
	convIndex	= 0;
	convBufferSize	= 0;
}

	soapyConverter::~soapyConverter	() {}

void	soapyConverter::setup (int inputRahe, int targetRange) {
	this	-> inputRate	= inputRate;
	this	-> targetRate	= targetRate;
	if (inputRate == targetRate)
	   return;

	this	-> mapTable_int. resize (targetRate / 1000);
	this	-> mapTable_float. resize (targetRate / 1000);
	convBufferSize		= inputRate / 1000;
	convIndex	= 0;
	convBuffer. resize (convBufferSize + 1);
	for (int i = 0; i < targetRate / 1000; i ++) {
	   float inVal	= (float) inputRate / 1000;
	   mapTable_int [i] = (int)(floor (i * inVal / (targetRate / 1000)));
	   mapTable_float [i] = 
	           i * (inVal / (float)(targetRate / 1000)) - mapTable_int [i];
	}
}

void	soapyConverter::add	(std::complex<float> *inBuf, int nSamples) {
std::complex<float> temp [targetRate / 1000];

	if (inputRate == targetRate) {
	   outBuffer -> putDataIntoBuffer (inBuf, nSamples);
	   return;
	}
//
//	alas, a real conversion is required
	for (int i = 0; i < nSamples; i ++) {
	   convBuffer [convIndex ++] = inBuf [i];
	   if (convIndex > convBufferSize) {
	      for (int j = 0; j < 2048; j ++) {
	         int16_t  inpBase	= mapTable_int [j];
	         float    inpRatio	= mapTable_float [j];
	         temp [j]	= convBuffer [inpBase + 1] * inpRatio + 
	                          convBuffer [inpBase] * (1 - inpRatio);
	      }

	      outBuffer -> putDataIntoBuffer (temp, targetRate / 1000);
//
//	shift the sample at the end to the beginning, it is needed
//	as the starting sample for the next time
	      convBuffer [0] = convBuffer [convBufferSize];
	      convIndex = 1;
	   }
	}
}



