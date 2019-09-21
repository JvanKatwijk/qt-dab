
#include	"sdrplay-widget.h"

	sdrplayWidget::sdrplayWidget	() {}

void	sdrplayWidget::setupUI (QWidget *qw) {
	deviceLabel	= new QLabel ("sdrplay control");
	api_version	= new QLCDNumber ();
	serialNumber	= new QLabel ();
	ppmControl	= new QSpinBox ();
	GRdBSelector	= new QSpinBox ();
	ppmControlLabel	= new QLabel ("ppm control");
	GRdBSelectorLabel = new QLabel ("if gain reduction");
	lnaGainSetting	= new QSpinBox ();
	lnaGRdBDisplay	= new QLCDNumber ();
	lnaStateSelectorLabel = new QLabel ("lna state select");
	tunerSelector	= new QComboBox	();
	antennaSelector	= new QComboBox ();
	agcControl 	= new QCheckBox ("agc");
	debugControl 	= new QCheckBox ("debug");

	api_version	->  setFrameShape (QFrame::NoFrame);
	api_version	->  setSegmentStyle (QLCDNumber::Flat);
	lnaGRdBDisplay	->  setFrameShape (QFrame::NoFrame);
	lnaGRdBDisplay	->  setSegmentStyle (QLCDNumber::Flat);

	QHBoxLayout *topLine	= new QHBoxLayout ();
	topLine		-> addWidget (deviceLabel);
	topLine		-> addSpacing (5);
	topLine		-> addWidget (api_version);

	QHBoxLayout *secondLine	= new QHBoxLayout ();
	secondLine	-> addWidget (ppmControl);
	secondLine	-> addSpacing (5);
	secondLine	-> addWidget (GRdBSelector);

	QHBoxLayout *thirdLine	= new QHBoxLayout ();
	thirdLine	-> addWidget (ppmControlLabel);
	thirdLine	-> addSpacing (5);
	thirdLine	-> addWidget (GRdBSelectorLabel);

	QVBoxLayout *fourthRight = new QVBoxLayout ();
	fourthRight	-> addWidget (agcControl);
	fourthRight	-> addSpacing (5);
	fourthRight	-> addWidget (debugControl);

	QHBoxLayout *intern	= new QHBoxLayout ();
	intern		-> addWidget (lnaGainSetting);
	intern		-> addWidget (lnaGRdBDisplay);

	QVBoxLayout *fourthLeft	= new QVBoxLayout ();
	fourthLeft	-> addItem (intern);
	fourthLeft	-> addWidget (lnaStateSelectorLabel);

	QHBoxLayout *fourth	= new QHBoxLayout ();
	fourth		-> addItem (fourthLeft);
	fourth		-> addItem (fourthRight);

	QHBoxLayout *fifth	= new QHBoxLayout ();
	fifth		-> addWidget (tunerSelector);
	fifth		-> addSpacing (5);
	fifth		-> addWidget (antennaSelector);

	QVBoxLayout *base	= new QVBoxLayout ();
	base		-> addItem (topLine);
	base		-> addWidget (serialNumber);
	base		-> addItem (secondLine);

	base		-> addItem (thirdLine);
	base		-> addItem (fourth);
	base		-> addItem (fifth);

	qw	->  setLayout (base);
}

	sdrplayWidget::~sdrplayWidget () {
}
