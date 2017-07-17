#
/*
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB.
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
 *    along with Qt-DAB; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

//
//	We use a straightforward class "eladLoader"  both as
//	loader and as container	for the dll functions.

#ifndef	__ELAD_LOADER
#define	__ELAD_LOADER
#include	"dab-constants.h"
#include	<stdint.h>
#include	<libusb-1.0/libusb.h>

//	The naming of functions for accessing shared libraries
//	differ between Linux and Windows
#define	GETPROCADDRESS	dlsym

// Dll function prototypes

typedef	void (*Pset_en_ext_io_ATT20)(libusb_device_handle *, int *);
typedef void (*Pset_en_ext_io_LP30)(libusb_device_handle *, int *);
typedef	int  (*OPEN_HW)(libusb_device_handle *, long);
typedef int  (*SET_HWLO)(libusb_device_handle *, long *);
typedef void (*START_FIFO)(libusb_device_handle *);
typedef void (*STOP_FIFO)(libusb_device_handle *);
typedef void (*STOP_HW)(void);
typedef	void (*CLOSE_HW)(void);
typedef int  (*FDMS1_HW_INIT)(libusb_device_handle *);

class	eladLoader {
public:

		eladLoader		(int32_t, int16_t *);
		~eladLoader		(void);
	bool	OK			(void);
libusb_device_handle	*getHandle	(void);
Pset_en_ext_io_ATT20 set_en_ext_io_ATT20;
Pset_en_ext_io_LP30 set_en_ext_io_LP30;
OPEN_HW			OpenHW;
SET_HWLO		SetHWLO;
START_FIFO		StartFIFO;
STOP_FIFO		StopFIFO;
STOP_HW			StopHW;
CLOSE_HW		CloseHW;
FDMS1_HW_INIT		Init;
private:
	libusb_device_handle *startUSB	(void);
	HINSTANCE	hwHandle;
	HINSTANCE	cwHandle;
	libusb_device_handle	*dev_handle;
	libusb_context		*context;
	bool		libraryLoaded;
	bool		usb_OK;
	bool		hardware_OK;
const	char		*cwLibname;
	char		*hwLibname;
	int32_t		theRate;
};

#endif


