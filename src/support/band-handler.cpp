#
/*
 *    Copyright (C) 2013, 2014, 2015, 2016, 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the Qt-DAB program
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
//
//	The issue:
//	suppose that someone wants to add some -non official -
//	frequencies, e.g. amateur bands.
//	we therefore use the "frequencies_1" and "frequencies_2"
//	tables for initializing the "bandIII" and "LBand" tables
//	and allow the BandIII table to be overwritten by a
//	supplied table.
//
#include	"band-handler.h"
#include	"dab-constants.h"
#include	<stdio.h>

static
struct {
	const char *key;
	int	fKHz;
} frequencies_1 [] = {
{"5A",	174928},
{"5B",	176640},
{"5C",	178352},
{"5D",	180064},
{"6A",	181936},
{"6B",	183648},
{"6C",	185360},
{"6D",	187072},
{"7A",	188928},
{"7B",	190640},
{"7C",	192352},
{"7D",	194064},
{"8A",	195936},
{"8B",	197648},
{"8C",	199360},
{"8D",	201072},
{"9A",	202928},
{"9B",	204640},
{"9C",	206352},
{"9D",	208064},
{"10A",	209936},
{"10B", 211648},
{"10C", 213360},
{"10D", 215072},
{"11A", 216928},
{"11B",	218640},
{"11C",	220352},
{"11D",	222064},
{"12A",	223936},
{"12B",	225648},
{"12C",	227360},
{"12D",	229072},
{"13A",	230784},
{"13B",	232496},
{"13C",	234208},
{"13D",	235776},
{"13E",	237488},
{"13F",	239200},
{nullptr, 0}
};

static
struct  {
	const char *key;
	int	fKHz;
} frequencies_2 [] = {
{"LA", 1452960},
{"LB", 1454672},
{"LC", 1456384},
{"LD", 1458096},
{"LE", 1459808},
{"LF", 1461520},
{"LG", 1463232},
{"LH", 1464944},
{"LI", 1466656},
{"LJ", 1468368},
{"LK", 1470080},
{"LL", 1471792},
{"LM", 1473504},
{"LN", 1475216},
{"LO", 1476928},
{"LP", 1478640},
{nullptr, 0}
};

typedef struct {
	QString key;
	int fKHz;
}dab_frequencies;

dab_frequencies bandIII_frequencies [100];
dab_frequencies Lband_frequencies   [100];

	bandHandler::bandHandler (const QString &a_band) {
int filler;
	for (filler = 0; frequencies_2 [filler]. fKHz != 0; filler ++) {
	   Lband_frequencies [filler]. key =
	                        QString (frequencies_2 [filler]. key);
	   Lband_frequencies [filler]. fKHz =
	                        frequencies_2 [filler]. fKHz;
	}
	Lband_frequencies [filler]. fKHz = 0;
//
	FILE *f	= nullptr;
#ifndef	__MINGW32__
	if (a_band != "")
	   f = fopen (a_band. toLatin1 (). data (), "r");
#endif
	if (f == nullptr) {
	   for (filler = 0; frequencies_1 [filler]. fKHz != 0; filler ++) {
	      bandIII_frequencies [filler]. key =
	                               QString (frequencies_1 [filler]. key);
	      bandIII_frequencies [filler]. fKHz =
	                               frequencies_1 [filler]. fKHz;
	   }
	   bandIII_frequencies [filler]. fKHz = 0;
	   return;
	}

#ifndef	__MINGW32__
//	OK we have a file with - hopefully - some input
	int	cnt	= 0;
	size_t	amount	= 128;
	filler		= 0;
	char *line	= (char *) malloc (256);
	while ((cnt < 100) && (amount > 0)) {
	   amount = getline (&line, &amount, f);
	   fprintf (stderr, "%s (%d)\n", line, amount);
	   if ((int)amount < 0) {
	      break;
	   }
	   line [amount] = 0;
	   char channelName [128];
	   int freq;
	   cnt ++;
	   int res = sscanf (line, "%s %d", channelName, &freq);
	   if (res != 2)
	      continue;
	   bandIII_frequencies [filler]. fKHz	= freq;
	   bandIII_frequencies [filler]. key	= QString (channelName);
	   filler ++;
	}

	bandIII_frequencies [filler]. fKHz	= 0;
	free (line);
	fclose (f);
#endif
}

	bandHandler::~bandHandler() {}

void	bandHandler::setupChannels (QComboBox *s, uint8_t band) {
dab_frequencies *t;
int16_t	i;
int16_t	c	= s -> count();

	theBand	= band;
//	clear the fields in the comboBox
	for (i = 0; i < c; i ++) 
	   s	-> removeItem (c - (i + 1));

	if (band == BAND_III)
	   t = bandIII_frequencies;
	else
	   t = Lband_frequencies;

	for (i = 0; t [i]. key != nullptr; i ++) 
	   s -> insertItem (i, t [i]. key, QVariant (i));
}

//	find the frequency for a given channel in a given band
int32_t	bandHandler::Frequency (QString Channel) {
int32_t	tunedFrequency		= 0;
dab_frequencies	*finger;
int	i;

	if (theBand == BAND_III)
	   finger = bandIII_frequencies;
	else
	   finger = Lband_frequencies;

	for (i = 0; finger [i]. key != nullptr; i ++) {
	   if (finger [i]. key == Channel) {
	      tunedFrequency	= KHz (finger [i]. fKHz);
	      break;
	   }
	}

	if (tunedFrequency == 0)
	   tunedFrequency = KHz (finger [0]. fKHz);

	return tunedFrequency;
}

