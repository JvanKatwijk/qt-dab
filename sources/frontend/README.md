
The directory "front end" is the front end handler. The driver is the
in the file "ofdm-handler", it takes the raw samples, does the
synchronization and the transformation from samples to soft bits
and passes the fft output - converted to soft bits - to
the backend

The so-called "fib decoder"  is now split into a "database" part
that contains the FIG data and the interpreting functions, and
the fib-decoder. The fib decoder now has no (direct) access anymore
to the database structures. Much cleaner code now.

