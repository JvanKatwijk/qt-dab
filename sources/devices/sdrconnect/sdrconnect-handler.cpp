#
/*
 *    Copyright (C) 2026
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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
//
//	handler for connecting to the SDRconnect program.
//	
#include	"sdrconnect-handler.h"

	sdrConnectHandler::sdrConnectHandler	():
	                       _O_Buffer (16 * 32768) {

	OK_to_run	= false;
	theMessager	= nullptr;
	setupUi (&myFrame);
	myFrame. show ();
	hostnameLabel   -> setInputMask ("000.000.000.000");
        hostnameLabel   -> setText ("127.0.0.1");
	portLabel	-> setValue (5454);
	connect (connectButton, &QPushButton::clicked,
                 this, &sdrConnectHandler::handle_hostName);
}

//	handle hostname is called whenever the user acknowledges the
//	hostname, no guarantee that there is a connection
void	sdrConnectHandler::handle_hostName	() {
QString	hostName	= hostnameLabel -> text ();
int	portNumber	= portLabel	-> value ();
	if (theMessager != nullptr)
	   return;
	theMessager	= new messageHandler (hostName, portNumber,
	                                           KHz (227360), &_O_Buffer);
	connect (theMessager, &messageHandler::connection_failed,
	         this, &sdrConnectHandler::connection_failed);
	connect (theMessager, &messageHandler::signalPower,
	         this, &sdrConnectHandler::signalPower);
	connect (theMessager, &messageHandler::dataAvailable,
	         this, &sdrConnectHandler::dataAvailable);
	connect (theMessager, &messageHandler::rateOK,
	         this, &sdrConnectHandler::rateOK);
	connect (theMessager, &messageHandler::rateError,
	         this, &sdrConnectHandler::rateError);
	connect (theMessager, &messageHandler::send_status,
	         this, &sdrConnectHandler::show_dropCount);
}

	sdrConnectHandler::~sdrConnectHandler		() {
	OK_to_run	= false;
	delete theMessager;
}

bool	sdrConnectHandler::restartReader	(int32_t freq, int skipped) {
	if (!OK_to_run)
	   return false;
	(void)skipped;
	if (theMessager != nullptr) {
	   theMessager ->  restartReader	(freq, skipped);
	}
	return true;
}

void	sdrConnectHandler::stopReader		() {
	if ((OK_to_run) && (theMessager != nullptr))
	   theMessager -> stopReader ();
}

int32_t	sdrConnectHandler::getSamples		(std::complex<float> *b, int32_t size) {
	if (!OK_to_run)
	   return 0;
	return _O_Buffer. getDataFromBuffer (b, size);
}

int32_t	sdrConnectHandler::Samples		() {
	if (!OK_to_run)
	   return 0;
	return _O_Buffer. GetRingBufferReadAvailable ();
}

void	sdrConnectHandler::resetBuffer		() {
	_O_Buffer. FlushRingBuffer ();
}

int16_t	sdrConnectHandler::bitDepth		() {
	return 12;
}

QString	sdrConnectHandler::deviceName		() {
	return "SDRconnect";
}

bool	sdrConnectHandler::isFileInput		() {
	return false;
}

int32_t	sdrConnectHandler::getVFOFrequency	() {
	if (theMessager == nullptr)
	   return -1;
	return theMessager	-> getVFOFrequency ();
}

void	sdrConnectHandler::connection_failed	() {
	statusLabel	-> setText ("Connection failed");
}

void	sdrConnectHandler::signalPower		(double v) {
	(void)v;
}
void	sdrConnectHandler::dataAvailable	(int amount) {
	(void)amount;
}

void	sdrConnectHandler::rateOK		() {
	OK_to_run	= true;
	statusLabel	-> setText ("we can go");
}

void	sdrConnectHandler::rateError		() {
	statusLabel	-> setText ("Alas, this does not work");
}

void	sdrConnectHandler::show_dropCount	(int n) {
	if (n == 0)
	   overflowLabel	-> setStyleSheet ("QLabel {color : green}");
	else
	   overflowLabel	-> setStyleSheet ("QLabel {color : red}");
	overflowLabel	-> setText ("   ");
}

