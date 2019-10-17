
#ifndef	__RTLSDR_WIDGET__
#define	__RTLSDR_WIDGET__

#include	<QWidget>
#include	<QSpinBox>
#include	<QLabel>
#include	<QComboBox>
#include	<QPushButton>

#include	<QVBoxLayout>
#include	<QHBoxLayout>
#include	<QFormLayout>

class	rtlsdrWidget {
public:
QSpinBox	*ppm_correction;
QLabel		*ppmCorrectionLabel;
QComboBox	*combo_gain;
QComboBox	*combo_autogain;
QPushButton	*dumpButton;
QLabel		*versionLabel;
QLabel		*product_display;
QLabel		*deviceName;

	rtlsdrWidget	() {}
void	setupUi		(QWidget *qw) {

	ppm_correction	= new QSpinBox ();
	ppmCorrectionLabel	= new QLabel ("ppm");
	combo_gain	= new QComboBox ();
	combo_autogain	= new QComboBox ();
	dumpButton	= new QPushButton ("write raw");
	versionLabel	= new QLabel ("dabstick");
	product_display	= new QLabel ();
	deviceName	= new QLabel ();

	combo_autogain	-> addItem ("autogain_off");
	combo_autogain	-> addItem ("autogain_on");

	QFormLayout *line1	= new QFormLayout ();
//	QHBoxLayout *line1	= new QHBoxLayout ();
	line1		-> addWidget (ppmCorrectionLabel);
	line1		-> addWidget (ppm_correction);

	QVBoxLayout *base	= new QVBoxLayout ();
	base		-> addWidget (versionLabel);
	base		-> addWidget (combo_autogain);
	base		-> addWidget (combo_gain);
	base		-> addWidget (dumpButton);
	base		-> addItem   (line1);
	base		-> addWidget (product_display);
	base		-> addWidget (deviceName);

	qw	-> setLayout (base);
}

};

#endif
