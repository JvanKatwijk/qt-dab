#
/*
 *    Copyright (C) 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB
 *    Qt-DAB is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    Qt-DAB is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Qt-TAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#
#ifndef	__FIB_PROCESSOR__
#define	__FIB_PROCESSOR__
#
//
#include	<stdint.h>
#include	<stdio.h>
#include	<QObject>
#include	"msc-handler.h"
#include	"tii_table.h"
#include	<QMutex>

	struct dablabel {
	   QString	label;
	   bool		hasName;
	};

	typedef struct dablabel	dabLabel;

	typedef struct subchannelmap channelMap;

//	from FIG1/2
	struct serviceid {
	   bool		inUse;
	   uint32_t	serviceId;
	   dabLabel	serviceLabel;
	   int16_t	language;
	   int16_t	programType;
	   int16_t	pNum;
	};
	typedef	struct serviceid serviceId;

//      The service component describes the actual service
//      It really should be a union, the component data for
//	audio and data are quite different
        struct servicecomponents {
           bool         inUse;          // just administration
           int8_t       TMid;           // the transport mode
           serviceId    *service;       // belongs to the service
           int16_t      componentNr;    // component

           int16_t      ASCTy;          // used for audio
           int16_t      PS_flag;        // use for both audio and packet
           int16_t      subchannelId;   // used in both audio and packet
           uint16_t     SCId;           // used in packet
           uint8_t      CAflag;         // used in packet (or not at all)
           int16_t      DSCTy;          // used in packet
	   uint8_t	DGflag;		// used for TDC
           int16_t      packetAddress;  // used in packet
	   int16_t	appType;	// used in packet
	   bool		is_madePublic;	// used to make service visible
        };

        typedef struct servicecomponents serviceComponent;

	struct subchannelmap {
	   bool		inUse;
	   int32_t	SubChId;
	   int32_t	StartAddr;
	   int32_t	Length;
	   bool		shortForm;
	   int32_t	protLevel;
	   int32_t	BitRate;
	   int16_t	language;
	   int16_t	FEC_scheme;
	};

	struct {
	   QString ensembleName;
	   int32_t ensembleId;
	   uint8_t ecc_byte;
	   bool	   name_Present;
	   bool	   ecc_Present;
	} ensemble_Descriptor;

class	RadioInterface;

class	fib_processor: public QObject {
Q_OBJECT
public:
		fib_processor		(RadioInterface *);
		~fib_processor		(void);

	void	setupforNewFrame	(void);
	void	clearEnsemble		(void);
	bool	syncReached		(void);
	void	resetSync		(void);
	void	setSelectedService	(QString &);
	uint8_t	kindofService		(QString &);
	void	dataforAudioService	(QString &, audiodata *, int16_t);
	void	dataforAudioService	(QString &, audiodata *);
	void	dataforAudioService	(int16_t,   audiodata *);
	void	dataforDataService	(QString &, packetdata *, int16_t);
	void	dataforDataService	(QString &, packetdata *);
	void	dataforDataService	(int16_t,   packetdata *);
	std::complex<float> get_coordinates	(int16_t, int16_t, bool *);
	int16_t	mainId			(void);
	uint8_t	get_ecc			(void);
	int32_t	get_ensembleId		(void);
	QString get_ensembleName	(void);
	int32_t	get_CIFcount		(void);
protected:
	void	newFrame		(void);
	void	process_FIB		(uint8_t *, uint16_t);
private:
	RadioInterface	*myRadioInterface;
	serviceId	*findServiceId (int32_t);
	serviceComponent *find_packetComponent (int16_t);
	serviceComponent *find_serviceComponent (int32_t SId, int16_t SCId);
        void            bind_audioService (int8_t,
                                           uint32_t, int16_t,
                                           int16_t, int16_t, int16_t);
        void            bind_packetService (int8_t,
                                            uint32_t, int16_t,
                                            int16_t, int16_t, int16_t);
	int32_t		findServiceIdwithName (QString &s);
	int32_t		CIFcount;
	void		process_FIG0		(uint8_t *);
	void		process_FIG1		(uint8_t *);
	void		FIG0Extension0		(uint8_t *);
	void		FIG0Extension1		(uint8_t *);
	void		FIG0Extension2		(uint8_t *);
	void		FIG0Extension3		(uint8_t *);
	void            FIG0Extension4          (uint8_t *);
	void		FIG0Extension5		(uint8_t *);
	void		FIG0Extension8		(uint8_t *);
	void		FIG0Extension9		(uint8_t *);
	void		FIG0Extension10		(uint8_t *);
	void		FIG0Extension13		(uint8_t *);
	void		FIG0Extension14		(uint8_t *);
	void		FIG0Extension16		(uint8_t *);
	void		FIG0Extension17		(uint8_t *);
	void		FIG0Extension18		(uint8_t *);
	void		FIG0Extension19		(uint8_t *);
	void		FIG0Extension21		(uint8_t *);
	void		FIG0Extension22		(uint8_t *);

	int16_t		HandleFIG0Extension1	(uint8_t *,
	                                         int16_t, uint8_t);
	int16_t		HandleFIG0Extension2	(uint8_t *,
	                                         int16_t, uint8_t, uint8_t);
	int16_t		HandleFIG0Extension3	(uint8_t *, int16_t);
	int16_t		HandleFIG0Extension5	(uint8_t *, int16_t);
	int16_t		HandleFIG0Extension8	(uint8_t *,
	                                         int16_t, uint8_t);
	int16_t		HandleFIG0Extension13	(uint8_t *,
	                                         int16_t, uint8_t);
	int16_t		HandleFIG0Extension22	(uint8_t *, int16_t);
	int32_t		dateTime	[8];
	channelMap	subChannels	[64];
	serviceComponent	ServiceComps [64];
	serviceId	listofServices	[64];
	tii_table	coordinates;
	bool		dateFlag;
	bool		isSynced;
	QMutex		fibLocker;
signals:
	void		addtoEnsemble	(const QString &);
	void		nameofEnsemble  (int, const QString &);
	void		changeinConfiguration (void);
};

#endif

