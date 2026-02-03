#
/*
 *    Copyright (C)  2015 .. 2025
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
 */

#include        <QDateTime>
#ifdef  __WITH_FDK_AAC__
#include <aacdecoder_lib.h>
#else
#include        "neaacdec.h"
#endif
#include	"radio.h"

#include	"copyrightLabel.h"

	copyrightText::copyrightText (RadioInterface *theCaller,
	                              const QString &version):
	                                            superFrame (nullptr) {
//
	titleLabel	= new QLabel;
	titleLabel	-> setAlignment (Qt::AlignCenter);
	titleLabel	-> setText ("<b>Qt-DAB</b>");

	authorLabel	= new QLabel;
	authorLabel	-> setText(tr("Qt-DAB is developed by")+" Jan van Katwijk (<a href=\"mailto:J.vanKatwijk@gmail.com\">J.vanKatwijk@gmail.com</a>)");
        authorLabel	-> setTextInteractionFlags (Qt::TextBrowserInteraction);

////////////////////////////////////////////////////////////////////////////
//	version and build info
	versionLabel	= new QLabel;
	versionLabel	-> setText (QString("Current Version-6.%1").arg (version));
//
	qtVersionLabel	= new QLabel;
	qtVersionLabel	-> setText (QString(tr("Qt-DAB uses Qt %1 and Qwt %2")).arg(QT_VERSION_STR). arg (QWT_VERSION_STR));

///////////////////////////////////////////////////////////////////////////
	QString support;
#if defined (SSE_AVAILABLE)
        support         = "(spiral + sse)";
#elif defined (NO_SSE_AVAILABLE)
        support         = "(spiral scalar)";
#elif defined(__AVX2__)
        support         = "(yang + avx2)";
#elif defined (__SSE4_1__)
        support         = "(yang + sse)";
#else
        support         = "(yang scalar)";
#endif
	QString theDate	= QString (__DATE__) + " " + QString (__TIME__);
	buildLabel	= new QLabel;
	buildLabel	-> setWordWrap (true);
	buildLabel	-> setText (QString ("Built on ") + theDate + 
 QString (", Commit ") + QString (GITHASH) + " with " + support);

//	source location
	sourceLocationLabel	= new QLabel;
	sourceLocationLabel	-> setText ("Sources are at <a href=\"https://github.com/JvanKatwijk/qt-dab\">github> </a>");
	sourceLocationLabel	-> setOpenExternalLinks (true);

////////////////////////////////////////////////////////////////////////
QString	aacSupport;
#ifdef __WITH_FDK_AAC__
	LIB_INFO libInfo [FDK_MODULE_LAST];
	FDKinitLibInfo (libInfo);
	aacDecoder_GetLibInfo (libInfo);
	
	for (int i = 0; i < FDK_MODULE_LAST; i++) {
	   if (libInfo [i]. module_id == FDK_AACDEC) {
	      aacSupport = QString(libInfo[i].title) + " "
	                                 + QString(libInfo[i].versionStr);
	      break;
	   }
	}
#else
	aacSupport	= QString ("faad2 ") + QString (FAAD2_VERSION);
#endif

QString aacComment;
#ifdef __WITH_FDK_AAC__
	aacComment =   "<li> " + aacSupport + " , <a href=\"https://github.com/mstorsjo/fdk-aac\">fdk-aac</a><br> Copyright © 1995 - 2018 Fraunhofer-Gesellschaft " +
                           "zur Förderung der angewandten Forschung e.V.</li>";
#else
	aacComment =  "<li>" + aacSupport + " <a href=\"https://github.com/knik0/faad2\">FAAD2</a> Copyright © 2003-2005 M. Bakker, Nero AG</li>";
#endif
////////////////////////////////////////////////////////////////////
//	we use a lot of libraries from others, ack their use
///////////////////////////////////////////////////////////////////
	librariesLabel	= new QLabel;
	librariesLabel	-> setWordWrap (true);
	librariesLabel -> setText (tr("Qt-DAB uses following libraries (and greatly acknowledges the copyright of their owners):")+
                           "<ul>" +
	                   "<li>The excellent Qt framework by Qt, and the Qwt library by Uwe Rathmann</li>" +
                           "<li><a href=\"www.fftw.org\">FFTW</a> by Matteo Frigo and Steven G Johnsom</li>" +
                           "<li><a href=\"https://github.com/Opendigitalradio/ka9q-fec\">Reed Solomon</a> by Phil Karn, KA9Q</li>" +
                           "<li><a href=\"https://www.spiral.net/software/viterbi.html\">viterbi code generator</a> by the Spiral project</li>" +
                           "<li><a href=\"https://github.com/williamyang98/ViterbiDecoderCpp\"> the excellent \"header only\" viterbi library</a> by  William Yang, based on Phil Karn's library</li>" +
                           "<li><a href=\"www.portaudio.com/\">Portaudio</a> by Ross Bencina, Phil Burke and many others</li>" +
                           "<li>An MP2 decoder by Martin J Fiedler (martin.fiedler@gmx.net)</li>" +
	                  "<li>\"NewsService Journaline(R) Decoder\", CopyRight (c) 2003, 2001-2014 Fraunhofer IID, Erlangen, Germany</li>" +
	                   aacComment +
				   "</ul>");
/////////////////////////////////////////////////////////////////////////
//	configuration
////////////////////////////////////////////////////////////////////////

	configurationLabel	= new QLabel;
	configurationLabel -> setWordWrap (true);
	configurationLabel -> setOpenExternalLinks (true);
	configurationLabel -> setText (tr("This version of Qt-DAB is configured with:")+
                           "<ul>"
#if defined (HAVE_SDRPLAY_V3) || defined (HAVE_SDRPLAY_V2)
                           "<li> All SDRplay RSP devices<a href=\"https://www.sdrplay.com/\">SDRplay/a></li>"
#endif
#if defined (HAVE_RTLSDR) || defined  (HAVE_RTLSDR_V3) || defined ( HAVE_RTLSDR_V4)
                           "<li> rtlsdr dabstick <a href=\"https://osmocom.org/projects/rtl-sdr/wiki/rtl-sdr\">rtlsdr</a>with improvements from \"olddab\" </li>"
#endif
#ifdef HAVE_AIRSPY
                           "<li> Airspy devices <a href=\"https://github.com/airspy\">AirSpy</a></li>"
#endif
#ifdef	HAVE_HACKRF 
                           "<li>Hackrf devices<a href=\"https://greatscottgadgets.com/hackrf/one\">Hackrf One</a>.</li>"
#endif
#ifdef	HAVE_LIME 
                           "<li> Lime SDR devices<a href=\"https://limemicro.com/products/limesdr\">Lime sdr</a> by Lime micro systems</li>"
#endif
#ifdef	HAVE_PLUTO 
                           "<li>Adalm Pluto device<a href=\"https://www.analog.com/\">Adalm Pluto One</a></li>"
#endif
#ifdef	HAVE_RTL_TCP
			   "<li>Client for RTL_TCP</li>"
#endif
#ifdef HAVE_SOAPY
                           "<li>Soapy interface<a href=\"https://github.com/pothosware/SoapySDR/wiki\">SoapySDR</a></li>"
#endif
#ifdef HAVE_SPYSERVER_16
                           "<li>SpyServer interface<a href=\"https://airspy.com/download/\">spyserver</a></li>"
#endif
                           "<li>  and has support for reading (and writing) files in different formats, including files (in xml and in RIFF/Raw64 format) with sizes larger than 4 Gb.</li>"
	"</ul>");
////////////////////////////////////////////////////////////////////////
//	acknowlegdements
////////////////////////////////////////////////////////////////////////
	acknowledgementsLabel	= new QLabel;
	acknowledgementsLabel -> setText (tr("Many people gave valuable feedback and input, Special thanks to") +
                         "<ul>"
	                 "<li> Herman Wijnants, for continuous suggestions and feedback</li>" +
	                 "<li> Andreas Mikula, for continuous support </li>" +
	                 "<li> Jarod Middelman, for continuous feedback</li>" +
	                 "<li> Stefan Poeschel, for providing code for saving AAC </li>" +
	                 "<li> Rolf Zerr, for improving code elements a.o TII decoder </li>" +
	                 "</ul>");
//////////////////////////////////////////////////////////////////////////
//	License issues
///////////////////////////////////////////////////////////////////////////
	disclaimerLabel	= new QTextEdit;
	disclaimerLabel -> setText("<p>Copyright © 2016-2026 Jan van Katwijk</p>"
                            "<p>Qt-DAB is distributed under the GPL V2 license "
                            "in the hope that it will be useful, "
                            "but WITHOUT ANA WARRANTY; without even the"
                            "implied warranty of MERCHANTIBILITY or FITNESS FOR, "
                            "A PARTICULAR PURPOSE. "
                            "This copyright notice shallbe included in all copies or substantial "
                            "portions of the Software.</p>");
//////////////////////////////////////////////////////////////////////////
//	binding the "close" signal
////////////////////////////////////////////////////////////////////////////
	connect (this, &superFrame::frameClosed,
	         theCaller, &RadioInterface::copyrightText_closed);
////////////////////////////////////////////////////////////////////////////
//	build up the layout
///////////////////////////////////////////////////////////////////////////
	theLayout	= new QVBoxLayout;
	theLayout	-> addWidget (titleLabel);
	theLayout	-> addWidget (versionLabel);
	theLayout	-> addWidget (authorLabel);
	theLayout	-> addWidget (buildLabel);
	theLayout	-> addWidget (qtVersionLabel);
	theLayout	-> addWidget (sourceLocationLabel);
	theLayout	-> addWidget (librariesLabel);
	theLayout	-> addWidget (configurationLabel);
	theLayout	-> addWidget (acknowledgementsLabel);
	theLayout	-> addWidget (disclaimerLabel);
	this	-> setLayout (theLayout);
	show ();
}

	copyrightText::~copyrightText () {
	hide ();
}


