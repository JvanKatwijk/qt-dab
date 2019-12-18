
#ifndef	__XML_READER__
#define	__XML_READER__

#include	<QThread>
#include	<QMessageBox>
#include	<stdio.h>
#include	"ringbuffer.h"
#include	<stdint.h>
#include	<complex>
#include	<vector>
#include	<atomic>

class	xml_fileReader;
class	xmlDescriptor;

class	xml_Reader:public QThread {
Q_OBJECT
public:
			xml_Reader (xml_fileReader	*mr,
	                            FILE		*f,
	                            xmlDescriptor	*fd,
	                            uint32_t		filePointer,
	                            RingBuffer<std::complex<float>> *b);
			~xml_Reader	();
	void		stopReader	();
	FILE		*file;
	xmlDescriptor	*fd;
	uint32_t	filePointer;
	RingBuffer<std::complex<float>> *sampleBuffer;
	xml_fileReader	*parent;
	int		nrElements;
	int		samplesToRead;
	std::atomic<bool> running;
	void		run ();
	int		compute_nrSamples 	(FILE *f, int blockNumber);
	int		readSamples		(FILE *f, int amount);
	float		readElement		(FILE *f);
//
//	for the conversion - if any
	int16_t         convBufferSize;
        int16_t         convIndex;
        std::vector <std::complex<float> >   convBuffer;
        int16_t         mapTable_int   [2048];
        float           mapTable_float [2048];

signals:
	void		setProgress		(int, int);
};

#endif
