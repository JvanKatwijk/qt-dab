

#ifndef	__XML_HANDLER__
#define	__XML_HANDLER__

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

class xmlHandler {
public:
		xmlHandler	(FILE *, int, int);
		~xmlHandler	();
	void	add		(std::complex<int16_t> *, int);
	void	computeHeader	(QString &, QString &);
private:
	QFrame		myFrame;
	int		denominator;
	int		frequency;
	QString		create_xmltree		(QString &, QString &);
	FILE		*xmlFile;
	QString		byteOrder;
	int		nrElements;
};

#endif
