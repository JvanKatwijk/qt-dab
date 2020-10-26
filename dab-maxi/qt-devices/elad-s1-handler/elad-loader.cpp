#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of Qt-DAB
 *
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

#include	"elad-loader.h"
//
//	Straight forward class to load the functions from the
//	elad library

	eladLoader::eladLoader	(int32_t theRate, int16_t *success) {
int	rc;
	libraryLoaded	= false;
	hardware_OK	= false;
	usb_OK		= false;
	*success	= -1;		// just the default

	fprintf (stderr, "going to load libusb\n");
	hwHandle	= dlopen ("libusb-1.0.so", RTLD_NOW | RTLD_GLOBAL);
	hwLibname	= new char [100];
	sprintf (hwLibname, "libfdms1_hw_init_%d.so.1.0", theRate);
//	             "/usr/local/lib64/libfdms1_hw_init_%d.so.1.0", theRate);
	  
	fprintf (stderr, "going to load %s\n", hwLibname);
	hwHandle	= dlopen (hwLibname, RTLD_NOW);
	if (hwHandle == NULL) {
	   fprintf (stderr, "error report %s\n", dlerror ());
	   libusb_close (dev_handle);
	   return;
	}
//	load the init function
	Init	= (FDMS1_HW_INIT) dlsym (hwHandle, (const char *)"fdms1_hw_init");
	if (Init == NULL) {
	   fprintf (stderr, "Could not load fdms1_hw_init\n");
	   dlclose (hwHandle);
	   return;
	}
//
//	now for the control library
	cwLibname	= "libfdms1_hw_ctrl.so.1.0";
//	cwLibname	= "/usr/local/lib64/libfdms1_hw_ctrl.so.1.0";
	fprintf (stderr, "going to load %s\n", cwLibname);
	cwHandle	= dlopen (cwLibname, RTLD_NOW);
	if (cwHandle == NULL) {
	   fprintf (stderr, "error report %s\n", dlerror ());
	   dlclose (hwHandle);
	   return;
	}

	libraryLoaded	= true;
//
//	Load the functions one by one
	set_en_ext_io_ATT20 = (Pset_en_ext_io_ATT20)
	                GETPROCADDRESS (cwHandle, "set_en_ext_io_ATT20");
	if (set_en_ext_io_ATT20 == NULL) {
	   fprintf (stderr, "Could not load set_en_ext_io_ATT20\n");
	   return;
	}

	set_en_ext_io_LP30 = (Pset_en_ext_io_ATT20)
	                GETPROCADDRESS (cwHandle, "set_en_ext_io_LP30");
	if (set_en_ext_io_LP30 == NULL) {
	   fprintf (stderr, "Could not load set_en_ext_io_ATT20\n");
	   return;
	}

	OpenHW		= (OPEN_HW) GETPROCADDRESS (cwHandle, "OpenHW");
	SetHWLO		= (SET_HWLO) GETPROCADDRESS (cwHandle, "SetHWLO");
	StartFIFO	= (START_FIFO) GETPROCADDRESS (cwHandle, "StartFIFO");
	StopFIFO	= (STOP_FIFO) GETPROCADDRESS (cwHandle, "StopFIFO");
	StopHW		= (STOP_HW) GETPROCADDRESS (cwHandle, "StopHW");
	CloseHW		= (CLOSE_HW) GETPROCADDRESS (cwHandle, "CloseHW");
	fprintf (stderr, "Functions seem to be loaded\n");
//
//	We first open the USB lib
	usb_OK		= false;
	dev_handle	= startUSB ();
	if (dev_handle == NULL)  {
	   *success = -2;
	   return;
	}
	usb_OK		= true;
//	we are going for init
	fprintf (stderr, "Loading FPGA, please wait\n");
	rc = Init (dev_handle);
	if (rc == 0) {
	   fprintf (stderr, "Loading FPGA Image failed %d\n", rc);
	   *success = -3;
	   return;
	}
	rc = OpenHW (dev_handle, theRate);
	fprintf (stderr, "Hardware opened %s\n", rc == 1 ? "good" : "failed");
	hardware_OK	= rc == 1;
	if (!hardware_OK) {
	   *success = -4;	
	   return;
	}
	   
	if (hardware_OK) {
	   int d_en_ext_io_ATT20 = 1;
	   int d_en_ext_io_LP30 = 1;
	   set_en_ext_io_ATT20 (dev_handle, &d_en_ext_io_ATT20);
	   set_en_ext_io_LP30 (dev_handle, &d_en_ext_io_LP30);
	}
	*success	= 0;
}

	eladLoader::~eladLoader	(void) {
	if (hardware_OK) {
	   StopHW	();
	   CloseHW ();
	}
	if (usb_OK)
	   libusb_close (dev_handle);

	if (libraryLoaded) {
	   dlclose (cwHandle);
	   dlclose (hwHandle);
	}
}
//

libusb_device_handle *eladLoader::startUSB	(void) {
libusb_device_handle	*dev_handle	= NULL;
uint16_t vendor_id =  0x1721;
uint16_t product_id = 0x0610;

	context		= 0;
int rc = libusb_init ( &context );
	
	if (rc == 0 && context != 0) {
	   libusb_set_debug (context,3); 
	}
	else {
	   fprintf (stderr,
	            "Error in libusb_init: [%d] %s\n",
	            rc, libusb_error_name (rc));
	   return NULL;
	}

	dev_handle = libusb_open_device_with_vid_pid (context,
	                                              vendor_id,
	                                              product_id);

	if (dev_handle == 0) {
	   fprintf (stderr, "Error in libusb_open_device_with_vid_pid\n");
	   fprintf (stderr,
	            "Check FDMS1 is properly connected and turned on.\n");
	   return NULL;
	}

	fprintf (stderr, "libusb_open_device_with_vid_pid OK\n");

	if (libusb_kernel_driver_active (dev_handle, 0) ){ 
	   printf("Device busy...detaching...\n"); 
	   libusb_detach_kernel_driver (dev_handle, 0); 
	} else 
	   printf("Device free from kernel, continue...\n"); 
 
	rc = libusb_claim_interface (dev_handle, 0);
	      //claim interface 0 (the first) of device
	if (rc < 0) {
	   fprintf (stderr,
	            "Cannot claim interface: [%d] %s\n",
	            rc, libusb_error_name (rc));
	   return NULL;
	}
	return dev_handle;
}

libusb_device_handle *eladLoader::getHandle (void) {
	return dev_handle;
}

bool	eladLoader::OK	(void) {
	return libraryLoaded && hardware_OK && usb_OK;
}

