#
/*
 *    Copyright (C)  2017 .. 2024
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

#include	<QDesktopServices>
#include	"aboutdialog.h"
#include	"ui_aboutdialog.h"
#include	"dab-constants.h"
#include	<QDateTime>
#ifdef	__WITH_FDK_AAC__
#include <aacdecoder_lib.h>
#else
#include        "neaacdec.h"

#endif
	AboutDialog::AboutDialog (QWidget *parent) :
	                                  QDialog(parent),
	                                  ui (new Ui::AboutDialog) {

	QString support;
#ifdef	SSE_AVAILABLE	
	support		= "(spiral + sse)";
#elif	NO_SSE_AVAILABLE
	support		= "(spiral scalar)";
#elif	__AVX2__
	support		= "(yang + avx2)";
#elif	__SSE4_1__	
	support		= "(yang + sse)";
#else
	support		= "(yang scalar)";
#endif
QString	aacSupport;
#ifdef __WITH_FDK_AAC__
	LIB_INFO libInfo[FDK_MODULE_LAST];
	FDKinitLibInfo(libInfo);
	aacDecoder_GetLibInfo(libInfo);
	
	for (int i = 0; i < FDK_MODULE_LAST; i++) {
	   if (libInfo[i].module_id == FDK_AACDEC) {
	      aacSupport = QString(libInfo[i].title) + " " + QString(libInfo[i].versionStr);
	      break;
	   }
	}
#else
	aacSupport	= QString ("faad2 ") + QString (FAAD2_VERSION);
#endif
QString aacComment;
#ifdef __WITH_FDK_AAC__
	aacComment =   "<li> " + aacSupport + " , <a href=\"https://github.com/mstorsjo/fdk-aac\">fdk-aac</a> Copyright © 1995 - 2018 Fraunhofer-Gesellschaft " +
                           "zur Förderung der angewandten Forschung e.V.</li>";
#else
	aacComment =  "<li>" + aacSupport + " <a href=\"https://github.com/knik0/faad2\">FAAD2</a> Copyright © 2003-2005 M. Bakker, Nero AG</li>";
#endif

	ui -> setupUi (this);
	QPixmap p;
	p. load (":res/pauze-slide.png", "png");
	ui -> appIcon		-> setPixmap (p);
	ui -> appName 		-> setText ("<b>Qt-DAB</b>");
	ui -> author 		-> setText(tr("Developed by")+" Jan van Katwijk (<a href=\"mailto:J.vanKatwijk@gmail.com\">J.vanKatwijk@gmail.com</a>)");
	ui -> author		-> setTextInteractionFlags (Qt::TextBrowserInteraction);
	ui -> version		-> setText (QString("Version-6.%1").arg ("9.6"));
	QString theDate		= QString (__DATE__) + " " + QString (__TIME__);
	ui -> buildInfo ->	setText (QString ("Built on ") + theDate + QString (", Commit ") + QString (GITHASH) + " with " + support);
	ui -> sourceLocation	-> setText ("Sources are at <a href=\"https://github.com/JvanKatwijk/qt-dab\">github> </a>");
	ui -> sourceLocation	-> setOpenExternalLinks(true);
	ui -> qtVersion		-> setText (QString(tr("Qt-DAB uses Qt %1")).arg(QT_VERSION_STR));

	ui -> libraries -> setText (tr("Qt-DAB uses following libraries (and greatly acknowledges the copyright):")+
                           "<ul>" +
	                   "<li>The excellent Qt framework by Qt, and the Qwt library (version 6.2) by Uwe Rathmann</li>" +
                           "<li><a href=\"www.fftw.org\">FFTW</a> by Matteo Frigo and Steven G Johnsom</li>" +
                           "<li><a href=\"https://github.com/Opendigitalradio/ka9q-fec\">Reed Solonon</a> by Phil Karn, KA9Q</li>" +
                           "<li><a href=\"https://www.spiral.net/software/viterbi.html\">viterbi code generator</a> by the Spiral project</li>" +
                           "<li><a href=\"https://github.com/williamyang98/ViterbiDecoderCpp\"> the excellent header only viterbi library</a> by  William Yang, based on Phil Karn's library</li>" +
                           "<li><a href=\"www.portaudio.com/\">Portaudio</a> by Ross Bencina, Phil Burke and many others</li>" +
                           "<li>An MP2 decoder by Martin J Fiedler (martin.fiedler@gmx.net)</li>" +
	                  "<li>\"NewsService Journaline(R) Decoder\", CopyRight (c) 2003, 2001-2014 Fraunhofer IID, Erlangen, Germany</li>" +
	                   aacComment +
				   "</ul>");
	ui -> acknowledgementsLabel -> setText (tr("Special thanks to") +
                         "<ul>"
	                 "<li> Herman Wijnants, for continuous suggestions and feedback</li>" +
	                 "<li> Andreas Mikula, for continuous support </li>" +
	                 "<li> Jarod Middelman, for continuous feedback</li>" +
	                 "<li> Stefan Poeschel, for providing code for saving AAC </li>" +
	                 "<li> Rolf Zerr, for an improved TII decoder and some other improvements </li>" +
	                 "</ul>");
//	ui -> configuration -> setTextInteractionFlags (Qt::TextBrowserInteraction);
	ui -> configuration -> setOpenExternalLinks(true);
	ui -> configuration -> setText (tr("Qt-DAB is configured with:")+
                           "<ul>"
#if defined (HAVE_SDRPLAY_V3) || defined (HAVE_SDRPLAY_V2)
                           "<li> All SDRplay RSP devices<a href=\"https://www.sdrplay.com/\">SDRplay/a></li>"
#endif
#if defined (HAVE_RTLSDR) || defined  (HAVE_RTLSDR_V3) || defined ( HAVE_RTLSDR_V4)
                           "<li> rtlsdr dabstick <a href=\"https://osmocom.org/projects/rtl-sdr/wiki/rtl-sdr\">rtlsdr</a>with improvements from \"olddab\" </li>"
#endif
#ifdef HAVE_AIRSPY_2
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
ui->disclaimer->setText("<p>Copyright © 2016-2024 Jan van Katwijk</p>"
                            "<p>Permission is hereby granted, free of charge, to any person obtaining a copy of this software "
                            "and associated documentation files (the “Software”), to deal in the Software without restriction, "
                            "including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, "
                            "and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, "
                            "subject to the following conditions: </p>"
                            "The above copyright notice and this permission notice shall be included in all copies or substantial "
                            "portions of the Software.</p>"
                            "<p>THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING "
                            "BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. "
                            "IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, "
                            "WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH "
                            "THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

    QObject::connect(
        ui->version, &QLabel::linkActivated,
        [=]( const QString & link ) { QDesktopServices::openUrl(QUrl::fromUserInput(link)); }
        );

    QObject::connect(
                ui->author, &QLabel::linkActivated,
                [=]( const QString & link ) { QDesktopServices::openUrl(QUrl::fromUserInput(link)); }
            );
    QObject::connect(
                ui->libraries, &QLabel::linkActivated,
                [=]( const QString & link ) { QDesktopServices::openUrl(QUrl::fromUserInput(link)); }
            );

    //resize(minimumSizeHint());
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

