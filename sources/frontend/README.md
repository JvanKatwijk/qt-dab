
The directory "front end" is the front end handler. The driver is the
derived class from from the class in "frontend-handler".
Depending on the selected input device - eti file or raw samples - the
etiProcessor or the ofdmProcessor is called.
The processor takes the samples, and the further processing.
The etiProcessor interprets the eti data and - after consulting the fibDecoder
allows the main program to select services and passes on the data to the
service processor.
The ofdmProcessor has a harder job, it controls synchronization
and the transformation from samples to soft bits
and passes the fft output - converted to soft bits - to
the backend

The so-called "fib decoder"  is now split into a "database" part
that contains the FIG data and the interpreting functions, and
the fib-decoder. The fib decoder now has no (direct) access anymore
to the database structures. Much cleaner code now.

