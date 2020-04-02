#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
 *    Qt-Dab is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation version 2 of the License.
 *
 *    Qt-Dab is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-Dab if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//
//	This file will be included in the sdrplay-controller.cpp file
//
//	The file contains the definitions of the interface data between
//	the calling and the callee thread
#ifndef	__SDRPLAY_COMMANDS__
#define	__SDRPLAY_COMMANDS__
#define RESTART_REQUEST         0100
#define STOP_REQUEST            0101
#define SETFREQUENCY_REQUEST	0102
#define	GETFREQUENCY_REQUEST	0103
#define AGC_REQUEST             0104
#define GRDB_REQUEST            0105
#define PPM_REQUEST             0106
#define LNA_REQUEST             0107
#define ANTENNASELECT_REQUEST   0110
#define GAINVALUE_REQUEST	0111

#include	<QSemaphore>

class generalCommand {
public:
	int	cmd;
	bool	result;
	QSemaphore waiter;
	generalCommand (int command):
	                waiter (0){
	   this -> cmd = command;
	}
	generalCommand () {}
};

class	restartRequest: public generalCommand {
public:
	int	frequency;
	bool	result;
	restartRequest (int newFrequency):
	   generalCommand (RESTART_REQUEST) {
	   this	-> frequency = newFrequency;
	}

	~restartRequest	() {}
};

class	stopRequest: public generalCommand {
public:
	stopRequest ():
	   generalCommand (STOP_REQUEST) {
	}

	~stopRequest	() {}
};

class set_frequencyRequest: public generalCommand {
public:
	int frequency;
	set_frequencyRequest (int newFreq):
	   generalCommand (SETFREQUENCY_REQUEST) {
	   this	-> frequency = newFreq;
	}

	~set_frequencyRequest	() {}
};

class get_frequencyRequest: public generalCommand {
public:
	int frequency;
	get_frequencyRequest () :
	   generalCommand (GETFREQUENCY_REQUEST) {
	}

	~get_frequencyRequest	() {}
};

class agcRequest: public generalCommand {
public:
	int	setPoint;
	bool	agcMode;
	agcRequest (bool onoff, int setPoint) :
	                generalCommand (AGC_REQUEST) {
	   this	-> setPoint	= setPoint;
	   this	-> agcMode	= onoff;
	}
	~agcRequest	() {}
};

class	GRdBRequest: public generalCommand {
public:
	int	GRdBValue;
	GRdBRequest (int GRdBValue):
	               generalCommand (GRDB_REQUEST) {
	   this	-> GRdBValue	= GRdBValue;
	}
	~GRdBRequest	() {}
};

class	ppmRequest: public generalCommand {
public:
	int	ppmValue;
	ppmRequest (int ppmValue):
	           generalCommand (PPM_REQUEST) {
	   this	-> ppmValue	= ppmValue;
	}
	~ppmRequest	() {}
};

class	lnaRequest: public generalCommand {
public:
	int	lnaState;
	lnaRequest (int lnaState):
	           generalCommand (LNA_REQUEST) {
	   this	-> lnaState	= lnaState;
	}
	~lnaRequest	() {}
};

class	gainvalueRequest: public generalCommand {
public: 
        int	gainValue;
        gainvalueRequest ():
                    generalCommand (GAINVALUE_REQUEST) {
        }
        ~gainvalueRequest     () {}
};

class	antennaRequest: public generalCommand {
public:
	int	antenna;
	antennaRequest (int antenna) :
	           generalCommand (ANTENNASELECT_REQUEST) {
	   this	-> antenna	= antenna;
	}
	~antennaRequest	() {}
};

#endif

