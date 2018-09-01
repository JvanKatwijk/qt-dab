
#include	"dab_tables.h"
#include	"data-descriptor.h"


	dataDescriptor::dataDescriptor (packetdata *pd):
	                                       myFrame (NULL) {
        setupUi (&myFrame);
        myFrame. show ();
	serviceName  -> setText (pd -> serviceName);
	subChannelId -> setText (QString::number (pd -> subchId));
	startAddress -> setText (QString::number (pd -> startAddr));
	Length       -> setText (QString::number (pd -> length));
	bitrate      -> setText (QString::number (pd -> bitRate));
	packetAddress-> setText (QString::number (pd -> packetAddress));
	QString protL	= getProtectionLevel (pd -> shortForm,
	                                      pd -> protLevel);
	protectionLevel   -> setText (protL);
	appType	     -> setText (getDSCTy (pd -> DSCTy));
	FECscheme    -> setText (getFECscheme (pd -> FEC_scheme));
}

	dataDescriptor::~dataDescriptor (void) {
}

