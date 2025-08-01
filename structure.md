
		THIS IS WORK IN PROGRESS

	   The structure of the source tree for Qt-DAB
		J van Katwijk, Lazy Chair Computing
-------------------------------------------------------------------------
------------------------------------------------------------------------

The basic operation of Qt-DAB - or any DAB decoder - is rather simple,
a stream of IQ samples - rate 2048000 samples/second - comes in, is interpreted, a user chooses a service, and a selection of the incoming samples is made and transformed to an audio stream with a samplerate of 48000 samples/second.

While that is certainly true, there are some details to take care of,
Qt-DAB has a GUI, with lots of choices to make for the user, input device
selection, channel selection, scanning, service selection, running services simultaneously etc etc.

That is why the program is somewhat larger, however, the basic operation
still applies, and essentially determines the structure of the software.

This structure is reflected in the structure of the sourcetree:

 * a group of sources implements functionality for handling the radio input devices - tuning to a frequency, gain setting, reading in samples and making IQ samples with a rate of 2048000 S.s available,  i.e. "Input Handling";

 * a second group of sources implements functions for the interpretation of the samples, transforming them into (soft) bits and building up a description of the payload of the datastream, i.e. the "Frontend";

 * a third group of sources implements the functions for the interpretation and conversion of (user) selected portions of the bitstream to audio and/or data, i.e. the "Backend";

 * a fourth group of sources implements the control of it all and implements the GUI elements, i.e. the "GUI control".

The Input Handling
----------------------------------------------------------------------------

The directory "sources/devices" contains the sources for the various device handlers. Device handlers interface to device libraries for external radio devices (or process files), and deliver a sample stream of 2048000 IQ samples/second as input.
A device driver shows a (usually small) window with controls for e.g. gain and output to a file.

 * a class "deviceHandler" is the "mother" of all device handlers. It defines theinterface that should be implemented by device handlers, and it implements some common elements (i.e. a frame for the GUI element and the visibility of this frame) and some defaults. Device handlers inherit from the class "deviceHandler".

 * a class "xml_fileWriter" is used by the various device handlers to write the raw input into an ".uff" type file.

 * a class "deviceChooser" implements an interface for the GUI controller to easily select (and instatiate) a device.

 * a class "device_exceptions" for mapping common device errors into exceptions with clear messages.

 * a subdirectory "fileReaders" contains sources for readers from files.

 * for each device that can be configured a subdirectory with a class derived from "deviceHandler" exists.

Some devices (AIRspy and Pluto) are not capable of outputting samples
with the required samplerate, in which case the device driver has to do dome  samplerate conversion.

The Frontend
----------------------------------------------------------------------------

The directory "sources/frontend" contains the sources implementing the
front end functionality.  As mentioned, the front end implements the functionality for inputting a stream of samples from a device handler, doing the required synchronizations on that stream, converting samples to (soft) bits, building up a database with the descriptions of the services.

One should realize that the input data consists of DAB frames, where a DAB frame contains three parts:
  * a so-called NULL part followed by a data block with predefined contents usedfor synchronization;
  * a few datablocks containing  data for the description of the content, i.e. the services;
  * data blocks with the actual content, the data for services.

The first step is synchronization, i.e. ensuring that the front end takes
the right samples for converting them into bits, including correction for frequency errors of the tuning of the signal, the functionality is implemented by:

 * The class "sampleReader" implements reading the input and ensuring samples are available and applies - in software - a frequency correction if needed;

 * The class "timeSyncer" implements the time synchronisation, i.e. it reports - when asked for - whether or not the transition between NULL symbol and first data element of a DAB frame seems to be detected;

 * The class "correlator" then implements the fine tuning here, it determines
the exactposition of the first sample of the first datablock of a DAB frame;

 * The class "freqSyncer" then implements the search for the
required correction of the tuned frequency. (Note that the actual correction of the frequency of the incoming sample stream is done in the "sampleReader" class);

The second step is implemented by 

 * the class "ofdmDecoder" that implements the conversion from the samples in the subsequent datablocks of the DAB frame to a sequence of soft bits, soft bits are encoded in the range -127 .. 127.

 * and a class "ficHandler" takes its input - soft bits - from the ofdmHandler, applies deconvolution on the input and prepares segments with hard bits for
processing by the fibDecoder.

 * and a class "fibDecoder" - together with the class "ensemble" and the class "fibConfig" - implements the actual processing of the FIC data (i.e. the "FIG's").  A database (the class "fibConfig" and the class "ensemble") is filled,
the GUI controller is informed ("signalled") about the ensemble and the services  that are described in the database, and the class provides functions  to inspect and extract attribute values of the services.

 * The two classes "tii-detector-xx"  implement the extraction of TII data from the NULL periods starting DAB frames. They report ("signal") their findings (i.e. basically mainId and subId) to the GUI controller.

The execution of the functionality is controlled by

 * the class "ofdmHandler", which is the controller here, it is aware of the state of the synchronization, takes action when needed, and sends the soft bits that return from the functions in the "ofdmDecoder" from the
data blocks 2..4 for processing the FIC, i.e. the "catalog", and the
softbits derived from the blocks 5 .. 76 to the backend.

The Backend
--------------------------------------------------------------------------

The directory "/sources/backend" contains the sources for the backend functionality.
DAB services are either "audio" or data oriented. For "audio", two modes are implemented, one for the "old" MP2 mode used for DAB services, and one for MP4, used for DAB+.
For "data", Qt-DAB supports a few common data modes, Slides through MOT, IP, EPG, TDC and journaline.

  * The class "mscHandler" is the interface class between the front end and the various backend "processors". It gets its input from the
class "ofdmHandler" (i.e. the soft bits resulting from the conversion of the
blocks 5 .. 76 of the DAB frames, as well as "instructions" from the
GUI controller about the services that should run. The functions in the class build cokmplete CIF vectors (i.e. vectors of 55296 soft bits) containing the data for ALL services in the ensemble.
Based on the instructions from the GUI controller, it instantiates (or kills) backend classes for the selected services, and it selects segments from the CIF vector and passes them on to backend "processors".

  * The class "Backend" is the generic backend controller, it implements the conversion from a segment of softbits that is passed on from the mscHandler, to output using the functionality the other classes in the directory;

  * The class "backend-deconvolver" does the deconvolution, after the deinterleaving was done. The deconvolution transforms the "soft bits" into hard bits.

  * The class "backendDriver" is the class that allocates and instantiates - based on the kind of service selected - a "processor" for further processing the data. 

The subdirectory "audio" contains sources for two classes rach implementing such an "audio" processor

 * a class mp2Processor, for handling the original DAB data, in MP2 format;

 * a class mp4Processor, for handling the DAB+ data.

Furthermore it contains

 * a class "firecode_cheker". A DAB+ frame is composed by 5 incoming data frames. The firecode_checker is used to identify the first of these frames such a DAB+ fraem can be built;

 * a class "bitWriter" used to write the AAC frames, resulting from the MP4 decoding into a file.

The subdirectory "data" contains the classes involved in the processing of the selected data.

  * a class "dataProcessor" which is the interface class between the sender
of the data segments (i.e. the backendDriver class) and the actual interpreter of the data. The class assembles from the incoming data the packets and
passes them on the the actual interpreter.

  * a class "ip_datahandler" implementing the collection of "ip" packets and sending them out as datagram;

  * a class "journaline-dataHandler" and a class "journalineScreen" that together implement the functionality of decoding and displaying journaline data;

  * a class "padHandler" that handles the "PAD",  Program Associated Data, varying from Dynamic Label to MOT data;

  * a **subdirectory** for MOT data, containing classes for handling a variety of MOT objects, i.e. Slides and EPG data;

     * a class "motHandler", the common interface from the providers of the MOT data to the "motDirectory" and "motObject" classes;

     * a class "motDirectory", that implements - using the functionality of the class "motObject: a caroussel with mot objects;

     * a class "motObject" that implements functions to collect the data for MOT objects; Resulting objects are passed on to the GUI controller.

  * a class "tdc_dataHandler", for implementing the extraction of TPEG type data, most of which is protected by a security key. Frames of the data can ne send to a separate server;

  * a **subdirectory** for EPG data, containing 

     * a class epgCompiler, translating the binary encoded EPG segments to xml

     * a class xmlExtractor, that helps interpreting the xml data

Note that handling the EPG data is done by functions in the GUI controller.
The binary encoded EPG segments are sent as MOT objects from the "motObject" class to the GUI controller.

The GUI controller
---------------------------------------------------------------------------

The directory "sources/main" contains the sources of the GUI controller.
Qt-DAB is GUI oriented, i.e. apart from some (obscure) command line parameters that can be passed to the program, all interaction is using a GUI. (Somewhere else the elements and the use of the GUI are detailed).

The Qt-DAB main program, traditionally called "main.cpp" "knows" a single
class that seems to do all the work

  * the class  RadioInterface controls the GUI. It ensures that on program startup a device is started, or if required, is selected first, after which processing is started.
 
  * the class "ensembleHandler" implements the management and display  of the services list.

  * the class "configHandler" "owns" the window with the same name and is
responsible for setting and registering the settings for the processes

  * the class "techData" "owns" the window with the same name and is responsible for displaying the parameters - and the spectrum - of the selected audio.

  * the class "displayWidget" in the directory /sources/main/forms-v7/new-display" implements the functionality of the spectrum display, it contains subclasses implementing the various scopes.


Other directories in the source tree
------------------------------------------------------------------------

 * eti-handling with the class "etiGenerator". The etiGenerator is able to generate from the input converted to an eti file;

 * protection with the class "protection", and the derived classes eepProtection and uepProtection, the toolkit for deconvolution in the backend;

 * server-thread with the class "tcpServer", used for sending tpg data to a client that is able to process that data;

 * support, with a fairly large number of classes delivering support functionality.
 


