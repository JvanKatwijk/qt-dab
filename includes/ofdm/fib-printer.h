
#pragma once
#include	"ensemble.h"
#include	"fib-config.h"

#include	<QString>
#include	<QStringList>

class	fibPrinter {
fibConfig	*currentConfig;
ensemble	*theEnsemble;
public:
	fibPrinter (fibConfig *currentConfig, ensemble *theEnsemble);
	~fibPrinter ();
	
QStringList	basicPrint		();
int		scanWidth		();
private:
QString		serviceName		(int index);
QString		serviceIdOf		(int index);
QString		subChannelOf 		(int index);
QString		startAddressOf 		(int index);
QString		lengthOf 		(int index);
QString		protLevelOf		(int index);
QString		codeRateOf 		(int index);
QString		bitRateOf		(int index);
QString		dabType 		(int index);
QString		languageOf 		(int index);
QString		programTypeOf		(int index);
QString		fmFreqOf		(int index);
QString		appTypeOf		(int index);
QString		FEC_scheme		(int index);
QString		packetAddress		(int index);
QString		DSCTy			(int index);
QString		audioHeader		();
QString		audioData		(int index);
QString		packetHeader		();
QString		packetData		(int index);
};

