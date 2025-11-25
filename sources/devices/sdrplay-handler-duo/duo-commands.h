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
#define DUO_SETAGC		0103
#define DUO_SETGRDB		0104
#define DUO_SETPPM		0105
#define DUO_SETLNA		0106
#define	DUO_SETBIAS_T		0110
#define	DUO_SETNOTCH		0111
#define	DUO_SETTUNER		0112

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

class	restartRequest: public duoCommand {
public:
	int	freq;
	bool	result;
	restartRequest (int newFrequency):
	   duoCommand (SET_RESTART) {
	   this	-> freq = newFrequency;
	}

	~restartRequest	() {}
};

class	stopRequest: public duoCommand {
public:
	stopRequest ():
	   duoCommand (SET_STOP) {
	}

	~stopRequest	() {}
};

class set_frequencyRequest: public duoCommand {
public:
	int freq;
	set_frequencyRequest (int newFreq):
	   duoCommand (DUO_SETFREQUENCY) {
	   this	-> freq = newFreq;
	}

	~set_frequencyRequest	() {}
};

class agcRequest: public duoCommand {
public:
	int	setPoint;
	bool	agcMode;
	agcRequest (bool onoff, int setPoint) :
	                duoCommand (DUO_SETAGC) {
	   this	-> setPoint	= setPoint;
	   this	-> agcMode	= onoff;
	}
	~agcRequest	() {}
};

class	GRdBRequest: public duoCommand {
public:
	int	GRdBValue;
	GRdBRequest (int GRdBValue):
	               duoCommand (DUO_SETGRDB) {
	   this	-> GRdBValue	= GRdBValue;
	}
	~GRdBRequest	() {}
};

class	ppmRequest: public duoCommand {
public:
	int	ppmValue;
	ppmRequest (int ppmValue):
	           duoCommand (DUO_SETPPM) {
	   this	-> ppmValue	= ppmValue;
	}
	~ppmRequest	() {}
};

class	lnaRequest: public duoCommand {
public:
	int	lnaState;
	lnaRequest (int lnaState):
	           duoCommand (DUO_SETLNA) {
	   this	-> lnaState	= lnaState;
	}
	~lnaRequest	() {}
};

class	biasT_Request: public duoCommand {
public:
	bool	checked;
	biasT_Request (bool biasT_value):
	            duoCommand (DUO_SETBIAS_T) {
	   this -> checked = biasT_value;
	}
	~biasT_Request	() {}
};
	
class	notch_Request: public duoCommand {
public:
	bool	checked;
	notch_Request (bool notch_value):
	            duoCommand (DUO_SETNOTCH) {
	   this -> checked = notch_value;
	}
	~notch_Request	() {}
};

class	tunerRequest: public duoCommand {
public:
	int	tuner;
	tunerRequest (int tuner) :
	           duoCommand (DUO_SETTUNER) {
	   this	-> tuner	= tuner;
	fprintf (stderr, "Creating a message for set_tuner for tuner %d\n", tuner);
	}
	~tunerRequest	() {}
};
