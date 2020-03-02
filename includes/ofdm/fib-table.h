#
/*
 *    Copyright (C) 2018, 2019, 2020
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
 * 	fib decoder. Functionality is shared between fic handler, i.e. the
 *	one preparing the FIC blocks for processing, and the mainthread
 *	from which calls are coming on selecting a program
 *
 *
 *	Definition of the "configuration" as maintained during reception of
 *	a channel
 *
 *	file is to be included in the source of fib-decoder
 */
// Tabelle ETSI EN 300 401 Page 50
// Table is copied from the work of Michael Hoehn
   const int ProtLevel[64][3]   = {{16,5,32},	// Index 0
	                           {21,4,32},
	                           {24,3,32},
	                           {29,2,32},
	                           {35,1,32},	// Index 4
	                           {24,5,48},
	                           {29,4,48},
	                           {35,3,48},
	                           {42,2,48},
	                           {52,1,48},	// Index 9
	                           {29,5,56},
	                           {35,4,56},
	                           {42,3,56},
	                           {52,2,56},
	                           {32,5,64},	// Index 14
	                           {42,4,64},
	                           {48,3,64},
	                           {58,2,64},
	                           {70,1,64},
	                           {40,5,80},	// Index 19
	                           {52,4,80},
	                           {58,3,80},
	                           {70,2,80},
	                           {84,1,80},
	                           {48,5,96},	// Index 24
	                           {58,4,96},
	                           {70,3,96},
	                           {84,2,96},
	                           {104,1,96},
	                           {58,5,112},	// Index 29
	                           {70,4,112},
	                           {84,3,112},
	                           {104,2,112},
	                           {64,5,128},
	                           {84,4,128},	// Index 34
	                           {96,3,128},
	                           {116,2,128},
	                           {140,1,128},
	                           {80,5,160},
	                           {104,4,160},	// Index 39
	                           {116,3,160},
	                           {140,2,160},
	                           {168,1,160},
	                           {96,5,192},
	                           {116,4,192},	// Index 44
	                           {140,3,192},
	                           {168,2,192},
	                           {208,1,192},
	                           {116,5,224},
	                           {140,4,224},	// Index 49
	                           {168,3,224},
	                           {208,2,224},
	                           {232,1,224},
	                           {128,5,256},
	                           {168,4,256},	// Index 54
	                           {192,3,256},
	                           {232,2,256},
	                           {280,1,256},
	                           {160,5,320},
	                           {208,4,320},	// index 59
	                           {280,2,320},
	                           {192,5,384},
	                           {280,3,384},
	                           {416,1,384}};

