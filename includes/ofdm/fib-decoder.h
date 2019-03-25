
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
#ifndef	__FIB_DECODER__
#define	__FIB_DECODER__
#
//
#include	<stdint.h>
#include	<stdio.h>
#include	<QObject>
#include	"msc-handler.h"
#include	<QMutex>
	
class	RadioInterface;

class	ensembleDescriptor;
class	dataBase;

class	fibDecoder: public QObject {
Q_OBJECT
public:
		fibDecoder		(RadioInterface *);
		~fibDecoder		(void);

	void	clearEnsemble		(void);
	bool	syncReached		(void);
	void	dataforAudioService	(const QString &,
	                                      audiodata *, int16_t);
	void	dataforPacketService	(const QString &,
	                                      packetdata *, int16_t);
        uint8_t get_ecc                 (void);
	int32_t	get_ensembleId		(void);
	QString get_ensembleName	(void);
	int32_t	get_CIFcount		(void);
protected:
	void	newFrame		(void);
	void	process_FIB		(uint8_t *, uint16_t);
private:
	RadioInterface	*myRadioInterface;
	dataBase	*currentBase;
	dataBase	*nextBase;
	ensembleDescriptor	*ensemble;
	void		process_FIG0		(uint8_t *);
	void		process_FIG1		(uint8_t *);
	void		FIG0Extension0		(uint8_t *);
	void		FIG0Extension1		(uint8_t *);
	void		FIG0Extension2		(uint8_t *);
	void		FIG0Extension3		(uint8_t *);
	void		FIG0Extension4		(uint8_t *);
	void		FIG0Extension5		(uint8_t *);
	void		FIG0Extension6		(uint8_t *);
	void		FIG0Extension7		(uint8_t *);
	void		FIG0Extension8		(uint8_t *);
	void		FIG0Extension9		(uint8_t *);
	void		FIG0Extension10		(uint8_t *);
//	void		FIG0Extension11		(uint8_t *);
//	void		FIG0Extension12		(uint8_t *);
	void		FIG0Extension13		(uint8_t *);
	void		FIG0Extension14		(uint8_t *);
//	void		FIG0Extension15		(uint8_t *);
//	void		FIG0Extension16		(uint8_t *);
	void		FIG0Extension17		(uint8_t *);
	void		FIG0Extension18		(uint8_t *);
	void		FIG0Extension19		(uint8_t *);
	void		FIG0Extension20		(uint8_t *);
	void		FIG0Extension21		(uint8_t *);
//	void		FIG0Extension22		(uint8_t *);
//	void		FIG0Extension23		(uint8_t *);
	void		FIG0Extension24		(uint8_t *);
	void		FIG0Extension25		(uint8_t *);
	void		FIG0Extension26		(uint8_t *);

	int16_t		HandleFIG0Extension1	(uint8_t *,
	                                         int16_t,
	                                         uint8_t, uint8_t, uint8_t);
	int16_t		HandleFIG0Extension2	(uint8_t *,
	                                         int16_t,
	                                         uint8_t, uint8_t, uint8_t);
	int16_t		HandleFIG0Extension3	(uint8_t *,
	                                         int16_t,
	                                         uint8_t, uint8_t, uint8_t);
	int16_t		HandleFIG0Extension5	(uint8_t *, int16_t);
	int16_t		HandleFIG0Extension8	(uint8_t *,
	                                         int16_t,
	                                         uint8_t, uint8_t, uint8_t);
	int16_t		HandleFIG0Extension13	(uint8_t *,
	                                         int16_t,
	                                         uint8_t, uint8_t, uint8_t);
	int16_t		HandleFIG0Extension21	(uint8_t*, int16_t);

	void		FIG1Extension0		(uint8_t *);
	void		FIG1Extension1		(uint8_t *);
//	void		FIG1Extension2		(uint8_t *);
//	void		FIG1Extension3		(uint8_t *);
	void		FIG1Extension4		(uint8_t *);
	void		FIG1Extension5		(uint8_t *);
	void		FIG1Extension6		(uint8_t *);

	int		findServiceIndex	(uint32_t);
	int		setServiceIndex		(uint32_t);
	int		findServiceComponent	(dataBase *, int16_t);
	int		findServiceComponentinService	(dataBase *,
	                                                 uint32_t, int16_t);

        void            bind_audioService (dataBase *,
	                                   int8_t,
                                           uint32_t, int16_t,
                                           int16_t, int16_t, int16_t);
        void            bind_packetService (dataBase *,
	                                    int8_t,
                                            uint32_t, int16_t,
                                            int16_t, int16_t, int16_t);
	int		findService	(const QString &);
//	void		bind_SCIds_to_ServiceComponent (uint16_t, uint16_t);
//	void		bind_SCIds_to_SubChannel	(uint16_t, uint16_t);
//	void		bindServiceComponenttoService	(uint32_t, uint16_t, uint8_t);
//	void		bind_xpadComponent		(uint32_t, uin16_t, uint8_t);
	int32_t		dateTime [8];
	QMutex		fibLocker;
	int		CIFcount;
signals:
	void		addtoEnsemble	(const QString &);
	void		nameofEnsemble  (int, const QString &);
	void		setTime		(const QString &);
	void		changeinConfiguration (void);
};

#endif

