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
#pragma once

#define DUO_RESTART	        0100
#define DUO_STOP		0101
#define DUO_SETFREQUENCY	0102
#define DUO_SETAGC_A		0103
#define DUO_SETAGC_B		1103
#define DUO_SETGRDB_A		0104
#define DUO_SETGRDB_B		1104
#define DUO_SETPPM_A		0105
#define DUO_SETPPM_B		1105
#define DUO_SETLNA_A		0106
#define DUO_SETLNA_B		1106
#define	DUO_SETNOTCH_A		0110
#define	DUO_SETNOTCH_B		1110
#define	DUO_SETBIAS_T		0111

#include	<QSemaphore>
#include	<stdio.h>

class duoCommand {
public:
	int	cmd;
	bool	result;
	QSemaphore waiter;
	duoCommand (int command):
	                waiter (0){
	   this -> cmd = command;
	}
	duoCommand () {}
};

class	duo_restart: public duoCommand {
public:
	int	freq;
	duo_restart (int newFrequency):
	   duoCommand (DUO_RESTART) {
	   this	-> freq = newFrequency;
	}

	~duo_restart	() {}
};

class	duo_stop: public duoCommand {
public:
	duo_stop ():
	   duoCommand (DUO_STOP) {
	}

	~duo_stop	() {}
};

class duo_Agc_A: public duoCommand {
public:
	int	setPoint;
	bool	agcMode;
	duo_Agc_A (bool onoff, int setPoint) :
	                duoCommand (DUO_SETAGC_A) {
	   this	-> setPoint	= setPoint;
	   this	-> agcMode	= onoff;
	}
	~duo_Agc_A	() {}
};

class duo_Agc_B: public duoCommand {
public:
	int	setPoint;
	bool	agcMode;
	duo_Agc_B (bool onoff, int setPoint) :
	                duoCommand (DUO_SETAGC_B) {
	   this	-> setPoint	= setPoint;
	   this	-> agcMode	= onoff;
	}
	~duo_Agc_B	() {}
};

class	duo_GRdB_A: public duoCommand {
public:
	int	GRdBValue;
	duo_GRdB_A (int GRdBValue):
	               duoCommand (DUO_SETGRDB_A) {
	   this	-> GRdBValue	= GRdBValue;
	}
	~duo_GRdB_A	() {}
};

class	duo_GRdB_B: public duoCommand {
public:
	int	GRdBValue;
	duo_GRdB_B (int GRdBValue):
	               duoCommand (DUO_SETGRDB_B) {
	   this	-> GRdBValue	= GRdBValue;
	}
	~duo_GRdB_B	() {}
};

class	duo_lna_A: public duoCommand {
public:
	int	lnaState;
	duo_lna_A (int lnaState):
	           duoCommand (DUO_SETLNA_A) {
	   this	-> lnaState	= lnaState;
	}
	~duo_lna_A	() {}
};

class	duo_lna_B: public duoCommand {
public:
	int	lnaState;
	duo_lna_B (int lnaState):
	           duoCommand (DUO_SETLNA_B) {
	   this	-> lnaState	= lnaState;
	}
	~duo_lna_B	() {}
};

class	duo_ppm_A: public duoCommand {
public:
	int	ppmValue;
	duo_ppm_A (int ppmValue):
	           duoCommand (DUO_SETPPM_A) {
	   this	-> ppmValue	= ppmValue;
	}
	~duo_ppm_A	() {}
};

class	duo_ppm_B: public duoCommand {
public:
	int	ppmValue;
	duo_ppm_B (int ppmValue):
	           duoCommand (DUO_SETPPM_B) {
	   this	-> ppmValue	= ppmValue;
	}
	~duo_ppm_B	() {}
};

class	duo_notch_A: public duoCommand {
public:
	bool	checked;
	duo_notch_A (bool notch_value):
	            duoCommand (DUO_SETNOTCH_A) {
	   this -> checked = notch_value;
	}
	~duo_notch_A	() {}
};

class	duo_notch_B: public duoCommand {
public:
	bool	checked;
	duo_notch_B (bool notch_value):
	            duoCommand (DUO_SETNOTCH_B) {
	   this -> checked = notch_value;
	}
	~duo_notch_B	() {}
};

class	duo_biasT: public duoCommand {
public:
	bool	checked;
	duo_biasT (bool biasT_value):
	            duoCommand (DUO_SETBIAS_T) {
	   this -> checked = biasT_value;
	}
	~duo_biasT	() {}
};
