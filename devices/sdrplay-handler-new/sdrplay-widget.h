

#include	<QObject>
#include	<QApplication>
#include	<QLabel>
#include	<QLCDNumber>
#include	<QSpinBox>
#include	<QCheckBox>
#include	<QWidget>
#include	<QComboBox>
#include	<QHBoxLayout>
#include	<QVBoxLayout>

class	sdrplayWidget {
public:
QLabel		*deviceLabel;
QLCDNumber	*api_version;
QLabel		*serialNumber;
QSpinBox	*ppmControl;
QSpinBox	*GRdBSelector;
QLabel		*ppmControlLabel;
QLabel		*GRdBSelectorLabel;
QSpinBox	*lnaGainSetting;
QLCDNumber	*lnaGRdBDisplay;
QLabel		*lnaStateSelectorLabel;
QComboBox	*tunerSelector;
QComboBox	*antennaSelector;
QCheckBox	*agcControl;
QCheckBox	*debugControl;

	sdrplayWidget ();
	~sdrplayWidget	();
void	setupUI	(QWidget *);
};

