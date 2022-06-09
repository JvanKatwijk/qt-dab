

#include	<stdio.h>
#include	<stdint.h>


int	main (int argc, char **argv) {
FILE	*fout	= stdout;
FILE	*fin	= stdin;
int	teller	= 0;
int	element;

	if (argc < 2) {
	   fprintf (stderr, "Usage converter infile outfile\n");
	   return 0;
	}

	fin	= fopen (argv [1], "r");
	fout	= fopen (argv [2], "w");

//
//	first a header for the out file
	fprintf (fout, "#\n#include <stdint.h>\n");
	fprintf (fout, "// generated map \n");
	fprintf (fout, "static uint8_t qt_map [] = {\n");
	while ((element = fgetc (fin)) >= 0) {
	   fprintf (fout, "%d, ", (uint8_t) element);
	   teller ++;
	   if (teller > 14) {
	      fprintf (fout, "\n\t");
	      teller = 0;
	   }
	}
	if (teller != 0) 
	   fprintf (fout, "\n\t 0};\n");
	else
	   fprintf (fout, "0}\n");
	fclose (fout);
}

	      
	   
	
