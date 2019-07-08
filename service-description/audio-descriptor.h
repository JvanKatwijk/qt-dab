
#ifndef	__AUDIO_DESCRIPTOR__
#define	__AUDIO_DESCRIPTOR__
#include        <QObject>
#include        <QFrame>
#include        <QSettings>
#include        <atomic>
#include        "dab-constants.h"
#include	"service-descriptor.h"
#include        "ui_audio-description.h"
#include	"text-mapper.h"

class	audioDescriptor : public serviceDescriptor, public Ui_audioDescription {
public:
		audioDescriptor		(audiodata *ad);
		~audioDescriptor();
private:
	QFrame	myFrame;
	textMapper      the_textMapper;
};

#endif

	
