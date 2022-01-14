
#include	<stdio.h>
#include	<stdint.h>
#include	<stdlib.h>
#include	<QSettings>
#include	<QDir>
#include	<QString>


int	main (int argc, char ** argv) {
	if (argc < 2)
	   return 0;

QString	fileName	= QDir::homePath ();
	fileName. append ("/");
	fileName. append (".qt-dab.ini");

FILE *f		= fopen (argv [1], "r");

	if (f == nullptr)
	   return 0;
QSettings	*dabSettings;

char coord [256];
int fp	= 0;

	dabSettings	= new QSettings (fileName, QSettings::IniFormat);
//"loc": "52.0417,4.4737",
//	we know the string has the format as given above,
//	first, we strip the quotes
	fgets (coord, 100, f);
	for (int i = 0; coord [i] != 0; i ++)
	 if (coord [i] != '\"')
	   coord [fp ++] = coord [i];
	coord [fp ++] = 0;
	char * r = coord;
	while (*r != ':') r ++;
	r ++;
	char *lat	= r;
	char *lon	= r;
	while (*lon != ',') lon ++;
	lon [-1 ] = 0;
	lon [0] = 0;
	lon ++;
	float la = atof (lat);
	float lo = atof (lon);
	fprintf (stderr, "%f, %f\n", la, lo);
	dabSettings	-> setValue ("latitude", QString::number (la));
	dabSettings	-> setValue ("longitude", QString::number (lo));
	dabSettings	-> sync ();
	delete dabSettings;
	return 1;
}



	
