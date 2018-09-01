
#ifndef	__DATA_DESCRIPTOR__
#define	__DATA_DESCRIPTOR__
#include        <QObject>
#include        <QFrame>
#include        <QSettings>
#include        <atomic>
#include        "dab-constants.h"
#include	"service-descriptor.h"
#include        "ui_data-description.h"
#include	"text-mapper.h"

class	dataDescriptor : public serviceDescriptor, public Ui_dataDescription {
public:
		dataDescriptor		(packetdata *ad);
		~dataDescriptor	(void);
private:
	QFrame	myFrame;
	textMapper      the_textMapper;
};

#endif

