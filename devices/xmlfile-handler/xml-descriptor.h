

#ifndef	__XML_DESCRIPTOR__
#define	__XML_DESCRIPTOR__

#include <QtXml>

#include	<QString>
#include	<stdint.h>
#include	<stdio.h>

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

class xmlDescriptor {
public:
	QString		deviceName;
	QString		deviceModel;
	QString		recorderName;
	QString		recorderVersion;
	QString		recordingTime;
	int		sampleRate;
	int		nrChannels;
	int		bitsperChannel;
	QString		container;
	QString		byteOrder;
	QString		iqOrder;
	int		nrBlocks;
	std::vector<Blocks> blockList;
			xmlDescriptor	(FILE *, uint32_t *, bool *);
			~xmlDescriptor	();
	void		printDescriptor	();
	void		setSamplerate	(int sr);
	void		setChannels 	(int	nrChannels,
	             	                 int	bitsperChannel,
	                                 QString	ct,
	             	                 QString	byteOrder);
	void		addChannelOrder (int channelOrder,
	                                 QString Value);
	void		add_dataBlock (int currBlock,  int Count,
                                       int  blockNumber, QString Unit);
	void		add_freqtoBlock	(int blockno, int freq);
	void		add_modtoBlock (int blockno, QString modType);
};

#endif
