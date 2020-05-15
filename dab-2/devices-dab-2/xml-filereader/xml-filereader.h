#
/*
 *    Copyright (C) 2013 .. 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the dab-2 software
 *
 *    dab-2 is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    dab-2 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with dab-2; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef	__XML_FILEREADER__
#define	__XML_FILEREADER__

#include	<QThread>
#include	<QString>
#include	<QFrame>
#include	<atomic>
#include	"dab-constants.h"
#include	"device-handler.h"
#include	"ringbuffer.h"
#include	"ui_xmlfiles.h"

class	QSettings;
class	xmlDescriptor;
class	xml_Reader;
class	RadioInterface;
class	dabProcessor;
/*
 */
class	xml_fileReader: public deviceHandler, public Ui_xmlfile_widget {
Q_OBJECT
public:
				xml_fileReader	(RadioInterface	*,
	                                         dabProcessor	*,
	                                         QString &);
                		~xml_fileReader	();
	bool			restartReader	(int32_t);
	void			stopReader	(void);
	int			getVFOFrequency	();
	int16_t			bitDepth	();
	void			show		();
	void			hide		();
	bool			isHidden	();

private:
	void			handle_Value	(int, float, float);
	dabProcessor		*base;
	QFrame			*myFrame;
	std::atomic<bool>	running;
	QString			fileName;
	FILE			*theFile;
	uint32_t		filePointer;
	xmlDescriptor		*theDescriptor;
	xml_Reader		*theReader;
	int			totalOffset;
public slots:
	void			setProgress	(int, int);
	void			handle_continuousButton ();
};

#endif

