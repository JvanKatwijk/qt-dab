
Sources of the Qt-DAB program are organized in directories

  * the directory "main" contains the C++ main program, the GUI handling "radio.cpp" program and some oter classes related to the GUI.
The GUI class handles the selection and connection of a devices,
and it instructs fromt end and backend what data is needed for selected
services.

  * the directory "frontend", contains all classed for handling the input samples and converting these to sequences of soft bits (softbits encoded in the range -127 .. 127

  * the output of the front end handler is passed on the "backend", the latter contains the functionality of mapping the soft bits to the audio and/pr data of the selected services. The directory itself has subdirectories for handling audio (i.e. mp2 or mp4) or data (mot, ip, journaline, epg)

  * "output" is the directory with the functionality of converting the PCM samples to audible audio. It allows a choice between using portaudio, Qt5 audio
or Qt6 audio.

  * "protection" contains a few classes handling the different protection schemes

  * "qt-devices" contains all code - in different subdirectories - for controlling the devices that can be part of the configuration

  * "support" contains - as the name suggests - the various support functions

  * "server-thread" contains some code for the tcp-server (does not need to be configured)

  * "eti-handler" contains a special class, one used to generate eti output.


