#
/*
 *    Copyright (C) 2016 .. 2023
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
 */
//
//	Common definitions and includes for
//	the DAB decoder

#pragma once

#include	<QString>
#include	<QStringList>
#include	<cmath>
#include	<cstdint>
#include	<cstdlib>
#include	<cstdio>
#include	<complex>
#include	<vector>
#include	<limits>
#include	<cstring>
#include	<unistd.h>

#ifndef	__FREEBSD__
//#include	<malloc.h>
#endif

#ifdef	__ARCH_X86__
#define	AVX_SUPPORT	0100
#define	SSE_SUPPORT	0200
#endif


#if defined (__MINGW32__) || defined (__WIN32)
//#include	"iostream.h"
#include	"winsock2.h"
#include	"windows.h"
#else
#ifndef	__FREEBSD__
//#include	"alloca.h"
#endif
#include	"dlfcn.h"
typedef	void	*HINSTANCE;
#endif

#define	MAX_VITERBI	127
#define	NR_TAPS		512

#ifdef	__WITH_DOUBLES__
typedef	double	DABFLOAT;
#else
typedef	float	DABFLOAT;
#endif
typedef	std::complex<DABFLOAT> Complex;
typedef std::complex<int16_t> complex16;

#define	DEFAULT_SWITCHVALUE	6

#ifndef	M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif

#ifndef	M_PI_2
#define	M_PI_2		(M_PI / 2.0f)
#endif
constexpr float	RAD_PER_DEGREE = (float)(M_PI / 180.0);

enum AudioFlags : uint32_t {
    AFL_NONE     = 0x0,
    AFL_SBR_USED = 0x1,
    AFL_PS_USED  = 0x2
};

typedef struct  {
	uint8_t		mainId;
	uint8_t		subId;
	float		strength;
	float		phase;
	bool		norm;
	int		index;
	bool		collision;
	uint16_t	pattern;
} tiiData;

typedef struct {
	int	mainId;
	int	subId;
	float	phase;
	bool	norm;
	float	strength;
	QString Name;
} corrElement;

#define	SINGLE_SCAN		0
#define	SCAN_SINGLE		0
#define	SCAN_TO_DATA		1
#define	SCAN_CONTINUOUS		2

using namespace std;
//
#define	Hz(x)		(x)
#define	Khz(x)		(x * 1000)
#define	KHz(x)		(x * 1000)
#define	kHz(x)		(x * 1000)
#define	Mhz(x)		(Khz (x) * 1000)
#define	MHz(x)		(KHz (x) * 1000)
#define	mHz(x)		(kHz (x) * 1000)

#define	CURRENT_VERSION	"6.9"

//#define		DAB		0100
//#define		DAB_PLUS	0101

#define		AUDIO_SERVICE	0
#define		PACKET_SERVICE	3

#define		SAMPLERATE	2048000
#define		BANDWIDTH	1536000

#define		M_PER_SAMPLE	 (299792458 / SAMPLERATE)

#define		MAP_RESET	0
#define		MAP_FRAME	1
#define		MAP_MAX_TRANS	2
#define		MAP_NORM_TRANS	4

#define		SHOW_SINGLE	16
#define		SHOW_ALL	32

#define		BAND_III	0100
#define		L_BAND		0101
#define		A_BAND		0102

#define		FORE_GROUND	0000
#define		BACK_GROUND	0100

#define		DAB_PLUS	077
#define		DAB_OLD		0

//	order by id order by name
#define ID_BASED        1
#define	SUBCH_BASED	2
#define	ALPHA_BASED	3

typedef struct {
	int     theTime;
	QString theText;
	QString	theDescr;
} epgElement;

class serviceId {
public:
	bool		defined;
	bool		selected;
	QString		name;
	uint32_t	SId;
	uint16_t	subChId;
	int		nrComps;
	bool		isAudio;
	int16_t		programType;
        QString		channel;        // just for presets
	int16_t		FEC_scheme;

};
//
class	descriptorType {
public:
	uint8_t	type;
	bool	defined;
	QString	serviceName;
	QString	shortName;
	int32_t	SId;
	int	SCIds;
	int16_t subchId;
	int16_t	startAddr;
	bool	shortForm;
	int16_t	protLevel;
	int16_t	length;
	int16_t	bitRate;
	QString	channel;	// just for presets
public:
		descriptorType() {
	defined		= false;
	serviceName	= "";
	}
virtual		~descriptorType() {}
};

//	for service handling we define
class packetdata: public descriptorType {
public:
	int16_t DSCTy;
	int16_t	FEC_scheme;
	int16_t	DGflag;
	int16_t	appType;
	int16_t	compnr;
	int16_t	packetAddress;
	packetdata() {
	   type	= PACKET_SERVICE;
	}
};

class audiodata: public descriptorType {
public:
	int16_t	ASCTy;
	int16_t	language;
	int16_t	programType;
	int16_t	compnr;
	std::vector<int> fmFrequencies;
	audiodata() {
	   type	= AUDIO_SERVICE;
	}
};

typedef struct {
	bool    in_use;
	int16_t id;
	int16_t start_cu;
	uint8_t uepFlag;
	int16_t protlev;
	int16_t size;
	int16_t bitrate;
	int16_t ASCTy; 
} channel_data;

class	contentType {
public:
	uint8_t		TMid;		// TMId value
	uint8_t		isActive;
	QString		serviceName;
	uint32_t	SId;
	uint16_t	SCIds;
//	the following values are subChannel values
	uint16_t	subChId;
	uint16_t	startAddress;
	uint16_t	length;
	QString		protLevel;
	QString		codeRate;
	uint16_t	bitRate;
	uint8_t		FEC_scheme;
	uint16_t	language;
//
	uint16_t	programType;
	std::vector<int> fmFrequencies;
	uint16_t	ASCTy_DSCTy;
	uint16_t	packetAddress;
	uint16_t	appType;
	contentType () {
	   isActive = 0;
	   FEC_scheme	= 0;
	   packetAddress	= 0;
	}
	~contentType () {}
};
	


//	40 up shows good results
#define		DIFF_LENGTH	60

static inline
float	get_db	(float x) {
	return 20 * log10 ((0.1 + x) / 256);
}

static inline
bool	isIndeterminate (float x) {
	return x != x;
}

static inline
bool	isInfinite (float x) {
	return x == numeric_limits<float>::infinity();
}

#define dynVec(t, v)	(t *)(alloca (v * sizeof (t)))
#define	MINIMUM(x, y)	((x) < (y) ? x : y)
#define	MAXIMUM(x, y)	((x) > (y) ? x : y)

static inline
float	jan_abs (Complex z) {
float	re	= real (z);
float	im	= imag (z);
	if (re < 0) re = -re;
	if (im < 0) im = -im;
	if (re > im) 
	   return re + 0.5 * im;
	else
	   return im + 0.5 * re;
}

static inline
DABFLOAT constrain (DABFLOAT V, DABFLOAT Low, DABFLOAT high) {
	if (V < Low)
	   return Low;
	if (V > high)
	   return high;
	return V;
}

static inline
float	square	(float a) {
	return a * a;
}

static inline 
float	compute_avg	(float oldVal, float newVal, float Alpha) {
	return (1 - Alpha) * oldVal + Alpha * newVal;
}

