#
/*
 *    Copyright (C) 2020
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of dumpViewer
 *
 *    dumpViewer is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dumpViewer is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dumpViewer; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	Main program
 */
#include	<QApplication>
#include	<stdio.h>
#include	"dump-viewer.h"

int	main (int argc, char **argv) {
dumpViewer	*theViewer;
FILE	*theFile;

	if (argc < 2) {
	   fprintf (stderr, "Usage: dumpViewer fileName\n");
	   exit (1);
	}

	theFile	= fopen (argv [1], "r+b");
	if (theFile == nullptr) {
	   fprintf (stderr, "we could not open %s\n", argv [1]);
	   exit (1);
	}

	QApplication a (argc, argv);
	theViewer 	= new dumpViewer (theFile);
	theViewer	-> show ();
	a. exec ();
/*
 *	done:
 */
	fflush (stdout);
	fflush (stderr);
	qDebug ("It is done\n");
}

