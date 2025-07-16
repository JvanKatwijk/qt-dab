
The directory "ofdm" is the front end handler. The driver is the
in the file "ofdm-handler", it takes the raw samples, does the
synchronization and the transformation from samples to soft bits
and passes the fft output - converted to soft bits - to
the backend

