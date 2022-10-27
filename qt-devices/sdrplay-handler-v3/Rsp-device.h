
#ifndef	__RSP_DEVICE_H
#define	__RSP_DEVICE_H

#include	<QObject>
#include	<stdint.h>
#include	<stdio.h>
#include	<sdrplay_api.h>

class	sdrplayHandler_v3;

class Rsp_device : public QObject {
Q_OBJECT
protected:
	sdrplay_api_DeviceT *chosenDevice;
	int	sampleRate;
	int	freq;
	bool	agcMode;
	int	lnaState;
	int	GRdB;
	sdrplay_api_RxChannelParamsT	*chParams;
	sdrplay_api_DeviceParamsT	*deviceParams;
	sdrplayHandler_v3	*parent;
	int	lna_upperBound;
	QString	deviceModel;
	bool	antennaSelect;
	int	nrBits;
	bool	biasT;
public:
		Rsp_device 	(sdrplayHandler_v3 *parent,
	                         sdrplay_api_DeviceT *chosenDevice,
	                         int sampleRate,
	                         int startFrequency,
	                         bool agcMode,
	                         int lnaState,
	                         int GRdB, bool biasT);
	virtual	~Rsp_device	();
virtual int	lnaStates	(int frequency);

virtual	bool	restart		(int freq);
virtual	bool	set_agc		(int setPoint, bool on);
virtual	bool	set_lna		(int lnaState);
virtual	bool	set_GRdB	(int GRdBValue);
virtual	bool	set_ppm		(int ppm);
virtual	bool	set_antenna	(int antenna);
virtual	bool	set_amPort 	(int amPort);
virtual	bool	set_biasT 	(bool biasT);
signals:
	void	set_lnabounds_signal	(int, int);
	void	set_deviceName_signal	(const QString &);
	void	set_antennaSelect_signal (int);
	void	set_nrBits_signal	(int);
	void	show_lnaGain		(int);
};
#endif


