
#ifndef	__FILEREADER_WIDGET__
#define	__FILEREADER_WIDGET__
#include	<QLabel>
#include	<QProgressBar>
#include	<QLCDNumber>
#include	<QHBoxLayout>
#include	<QVBoxLayout>

class	filereaderWidget {
public:
	QLabel		*titleLabel;
	QLabel		*nameofFile;
	QProgressBar	*fileProgress;
	QLCDNumber	*currentTime;
	QLabel		*seconds;
	QLCDNumber	*totalTime;

	filereaderWidget () {}
	~filereaderWidget () {}

void	setupUi		(QWidget *qw) {
	titleLabel	= new QLabel ("Playing pre-recorded file");
	nameofFile	= new QLabel ();
	fileProgress	= new QProgressBar ();
	currentTime	= new QLCDNumber	();
	currentTime	->  setFrameShape (QFrame::NoFrame);
	currentTime	->  setSegmentStyle (QLCDNumber::Flat);

	seconds		= new QLabel ("seconds of");
	totalTime	= new QLCDNumber	();

	totalTime	->  setFrameShape (QFrame::NoFrame);
	totalTime	->  setSegmentStyle (QLCDNumber::Flat);

	QHBoxLayout *bottom	= new QHBoxLayout ();
	bottom		-> addWidget (currentTime);
	bottom		-> addWidget (seconds);
	bottom		-> addWidget (totalTime);

	QVBoxLayout *base	= new QVBoxLayout ();
	base		-> addWidget (titleLabel);
	base		-> addWidget (nameofFile);
	base		-> addWidget (fileProgress);
	base		-> addItem   (bottom);
	
	qw		-> setLayout (base);
}
};
#endif

