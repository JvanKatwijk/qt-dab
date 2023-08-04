#
/*
 *    Copyright (C) 2014 .. 2019
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB 
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
 *
 */
#include	"xml-descriptor.h"

	xmlDescriptor::~xmlDescriptor	() {
}

void	xmlDescriptor::printDescriptor	() {
	fprintf (stderr, "sampleRate =	%d\n", sampleRate);
	fprintf (stderr, "nrChannels	= %d\n", nrChannels);
	fprintf (stderr, "bitsperChannel = %d\n", bitsperChannel);
	fprintf (stderr, "container	= %s\n", container. toLatin1 (). data ());
	fprintf (stderr, "byteOrder	= %s\n",
	                              byteOrder. toLatin1 (). data ());
	fprintf (stderr, "iqOrder	= %s\n",
	                              iqOrder. toLatin1 (). data ());
	fprintf (stderr, "nrBlocks	= %d (%d)\n",
	                              nrBlocks,  (int)(blockList. size ()));
	for (int i = 0; i < (int)blockList. size (); i ++)
	   fprintf (stderr, ">>>   %d %d %s %d %s\n",
	                   blockList. at (i). blockNumber,
	                   blockList. at (i). nrElements,
	                   blockList. at (i). typeofUnit. toLatin1 (). data (),
	                   blockList. at (i). frequency,
	                   blockList. at (i). modType. toLatin1 (). data ());
}
	
void	xmlDescriptor::setSamplerate	(int sr) {
	   this	-> sampleRate = sr;
}

void	xmlDescriptor::setChannels 	(int	nrChannels,
	                                 int	bitsperChannel,
	                                 QString	ct,
	                                 QString	byteOrder) {
	this	-> nrChannels		= nrChannels;
	this	-> bitsperChannel	= bitsperChannel;
	this	-> container		= ct;
	this	-> byteOrder		= byteOrder;
}

void	xmlDescriptor::addChannelOrder (int channelOrder, QString Value) {
	if (channelOrder > 1) 
	   return;
	if (channelOrder == 0)	// first element
	   this	-> iqOrder	= Value == "I" ? "I_ONLY" : "Q_ONLY";
	else
	if ((this -> iqOrder == "I_ONLY") && (Value == "Q"))
	   this -> iqOrder = "IQ";
	else
	if ((this -> iqOrder == "Q_ONLY") && (Value == "I"))
	   this -> iqOrder = "QI";
}

void	xmlDescriptor::add_dataBlock (int currBlock,  int Count,
                                      int  blockNumber, QString Unit) {
Blocks	b;
	b. blockNumber	= blockNumber;
	b. nrElements	= Count;
	b. typeofUnit	= Unit;
	blockList. push_back (b);
}

void	xmlDescriptor::add_freqtoBlock	(int blockno, int freq) {
	blockList. at (blockno). frequency = freq;
}

void	xmlDescriptor::add_modtoBlock (int blockno, QString modType) {
	blockList. at (blockno). modType	= modType;
}
//
//	precondition: file exists and is readable.
//	Note that after the object is filled, the
//	file pointer points to where the contents starts
	xmlDescriptor::xmlDescriptor (FILE *f, bool *ok) {
QDomDocument xmlDoc;
QByteArray xmlText;
int	zeroCount = 0;
//
//	set default values
	sampleRate	= 2048000;
	nrChannels	= 2;
	bitsperChannel	= 16;
	container	= "int16_t";
	byteOrder	= QString ("MSB");
	iqOrder		="IQ";
	uint8_t		theChar;
	while (zeroCount < 500) {
	   theChar = fgetc (f);
	   if (theChar == 0)
	      zeroCount ++;
	   else 
	      zeroCount = 0;

	   xmlText. append (theChar);
	}

	xmlDoc. setContent (xmlText);
	QDomElement root        = xmlDoc. documentElement ();
	QDomNodeList nodes = root. childNodes ();

	fprintf (stderr, "document has %d topnodes\n", nodes. count ());
	for (int i = 0; i < nodes. count (); i ++) {
	   if (nodes. at (i). isComment ()) {
	      continue;
	   }
	   QDomElement component = nodes. at (i). toElement ();
	   if (component. tagName () == "Recorder") {
	      this -> recorderName = component. attribute ("Name", "??");
	      this -> recorderVersion = component. attribute ("Version", "??");
	   }
	   if (component. tagName () == "Device") {
	      this -> deviceName = component. attribute ("Name", "unknown");
	      this ->  deviceModel = component. attribute ("Model", "???");
	   }

	   if (component. tagName () == "Time") 
	      this -> recordingTime = component. attribute ("Value", "???");

	   if (component. tagName () == "Sample") {
	      QDomNodeList childNodes = component. childNodes ();
	      for (int k = 0; k < childNodes. count (); k ++) {
	         QDomElement Child = childNodes. at (k). toElement ();
	         if (Child. tagName () == "Samplerate") {
	            QString SR = Child. attribute ("Value", "2048000");
	            QString Hz = Child. attribute ("Unit", "Hz");
	            int factor = Hz == "Hz" ? 1 :
	                         (Hz == "KHz") || (Hz == "Khz") ? 1000 :
	                         1000000;
	            setSamplerate (SR. toInt () * factor);
	         }
	         if (Child. tagName  () == "Channels") {
	            QString Amount = Child. attribute ("Amount", "2");
	            QString Bits   = Child. attribute ("Bits", "8");
	            QString Container   = Child.
	                                attribute ("Container", "uint8_t");
	            QString Ordering   = Child.
	                                attribute ("Ordering", "N/A");
	            setChannels (Amount. toInt (),
	                         Bits. toInt (),
	                         Container,
	                         Ordering);
	            QDomNodeList subnodes = Child. childNodes ();
	            int channelOrder = 0;
	            for (int k = 0; k < subnodes.count (); k ++) {
	               QDomElement subChild = subnodes. at (k).  toElement ();
	               if (subChild. tagName () == "Channel") {
	                  QString Value = subChild. attribute ("Value", "I");
	                  addChannelOrder (channelOrder, Value);
	                  channelOrder ++;
	               }
	            } 
	         }
	      }
	   }
	   if (component. tagName () == "Datablocks") {
//	      QString Count = component.attribute ("Count", "3");
	      this ->  nrBlocks = 0;
	      int currBlock	= 0;
	      QDomNodeList nodes = component. childNodes ();
	      for (int j = 0; j < nodes. count (); j ++) {
	         if (nodes. at (j). isComment ()) {
	            continue;
	         }
	      fprintf (stderr, "Datablocks has %d siblings\n", nodes. count ());
	         QDomElement Child = nodes. at (j). toElement ();
	         if (Child. tagName () == "Datablock") {
	            fprintf (stderr, "weer een block\n");
	            int Count = (Child. attribute ("Count", "100")). toInt ();
	            int Number = (Child. attribute ("Number", "10")). toInt ();
	            QString Unit = Child.  attribute ("Channel", "Channel");
	            add_dataBlock (currBlock, Count, Number, Unit);
	            QDomNodeList nodeList = Child. childNodes ();
	            for (int k = 0; k < nodeList. count (); k ++) {
	               if (nodeList. at (k). isComment ()) {
//	                  fprintf (stderr, "Comment gevonden\n");
	                  continue;
	               }
	               QDomElement subChild = nodeList. at (k). toElement ();
	               if (subChild. tagName () == "Frequency") {
	                  QString Unit = subChild.
	                                     attribute ("Unit", "Hz");
	                  int Value = 
	                      (subChild. attribute ("Value", "200")). toInt ();
	                  int Frequency =
	                       Unit == "Hz" ? Value :
	                       ((Unit == "KHz") | (Unit == "Khz")) ? Value * 1000 :
	                       Value * 1000000;
	                     add_freqtoBlock (currBlock, Frequency);
	                }
	                if (subChild. tagName () == "Modulation") {
	                   QString Value = subChild.
	                                      attribute ("Value", "DAB");
	                   add_modtoBlock (currBlock, Value);
	                 }
	            }
	            currBlock ++;
	         }
	      }
	      nrBlocks = currBlock;
	   }
	}
	*ok	= nrBlocks > 0;
	printDescriptor ();
}

