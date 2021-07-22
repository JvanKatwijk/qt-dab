#ifndef	__LIME_WIDGET__
#define	__LIME_WIDGET__

#include	<QLabel>
#include	<QSpinBox>
#include	<QLCDNumber>
#include	<QComboBox>
#include	<QPushButton>
#include	<QHBoxLayout>
#include	<QVBoxLayout>
#include	<QCheckBox>

class	limeWidget {
public:
	QLabel		*titleLabel;
	QSpinBox	*gainSelector;
	QLCDNumber	*actualGain;
	QLabel		*gainLabel;
	QComboBox	*antennaList;
	QLabel		*antennaLabel;
	QLCDNumber	*underrunDisplay;
	QLabel		*underrunLabel;
	QLCDNumber	*overrunDisplay;
	QLabel		*overrunLabel;
	QPushButton	*dumpButton;
	QCheckBox	*filterSelector;
	QSpinBox	*filterDepth;

	limeWidget	() {}
	~limeWidget	() {}

void	setupUi		(QWidget *qw) {
	titleLabel	= new QLabel	("lime handler");
	gainSelector	= new QSpinBox	();
	gainLabel	= new QLabel	("gain");
	actualGain	= new QLCDNumber ();
	antennaList	= new QComboBox	();
	antennaLabel	= new QLabel	("antennas");
	underrunDisplay	= new QLCDNumber ();
	underrunLabel	= new QLabel	("underruns");
	overrunDisplay	= new QLCDNumber ();
	overrunLabel	= new QLabel	("overruns");
	dumpButton	= new QPushButton	();
	filterSelector	= new QCheckBox	("filter");
	filterDepth	= new QSpinBox	();
	filterDepth	-> setRange (5, 25);
	dumpButton	-> setText ("Dump");
	actualGain	->  setFrameShape (QFrame::NoFrame);
        actualGain	->  setSegmentStyle (QLCDNumber::Flat);
	underrunDisplay	->  setFrameShape (QFrame::NoFrame);
        underrunDisplay	->  setSegmentStyle (QLCDNumber::Flat);
	overrunDisplay	->  setFrameShape (QFrame::NoFrame);
        overrunDisplay	->  setSegmentStyle (QLCDNumber::Flat);

	QHBoxLayout *line2	= new QHBoxLayout ();
	line2		-> addWidget (gainSelector);
	line2		-> addWidget (gainLabel);
	line2		-> addWidget (actualGain);

	QHBoxLayout *line3	= new QHBoxLayout ();
	line3		-> addWidget (antennaList);
	line3		-> addSpacing (3);
	line3		-> addWidget (antennaLabel);

	QHBoxLayout *line4	= new QHBoxLayout ();
	line4		-> addWidget (underrunDisplay);
	line4		-> addWidget (underrunLabel);
	line4		-> addWidget (filterSelector);

	QHBoxLayout *line5	= new QHBoxLayout ();
	line5		-> addWidget (overrunDisplay);
	line5		-> addWidget (overrunLabel);
	line5		-> addWidget (filterDepth);

	QHBoxLayout *line1	= new QHBoxLayout ();
	line1		-> addWidget (titleLabel);
	line1		-> addWidget (dumpButton);
	
	QVBoxLayout *base	= new QVBoxLayout ();
	base		-> addItem	(line1);
	base		-> addItem	(line2);
	base		-> addItem	(line3);
	base		-> addItem	(line4);
	base		-> addItem	(line5);

	qw	-> setLayout (base);
}
};

#endif

