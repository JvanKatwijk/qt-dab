
The directory "backend" contains - as the name suggests - functions to map the
"soft bits", passed onto audio and/or data.

The input here is processed by the file "msc-handler.cpp", that file
extracts the data for the selected service(s) and initiates required backends.
The file backend takes care of the de-interleaving, the file "backend-deconvolver" takes care of the deconvolution and data from a packet service is
then further processed by files in the "data" directory, and audio data in the 
directory "audio".



