

#ifndef	__XML_FILEWRITER__
#define	__XML_FILEWRITER__

#include <QtXml>

#include	<QString>
#include	<stdint.h>
#include	<stdio.h>
#include	<complex>

class Blocks	{
public:
			Blocks		() {}
			~Blocks		() {}
	int		blockNumber;
	int		nrElements;
	QString		typeofUnit;
	int		frequency;
	QString		modType;
};

class xml_fileWriter {
public:
		xml_fileWriter	(FILE *,
	                         int,
	                         QString,
	                         int,
	                         int,
	                         QString,
	                         QString,
	                         QString);
	                         
			~xml_fileWriter		();
	void		add			(std::complex<int16_t> *, int);
	void		computeHeader		();
private:
	int		nrBits;
	QString		container;
	int		sampleRate;
	int		frequency;
	QString		deviceName;
	QString		deviceModel;
	QString		recorderVersion;
	QString		create_xmltree		();
	FILE		*xmlFile;
	QString		byteOrder;
	int		nrElements;
};

#endif
