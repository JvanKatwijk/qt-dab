
/*
 *    Copyright (C) 2015 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the Qt-DAB program
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

#ifndef	__MOT_CONTENT_TYPE__
#define	__MOT_CONTENT_TYPE__

// Content type codes, reference:
// https://www.etsi.org/deliver/etsi_ts/101700_101799/101756/01.06.01_60/ts_101756v010601p.pdf page 20.

enum MOTContentBaseType {
	MOTBaseTypeGeneralData		= 0x00,
	MOTBaseTypeText			= 0x01,
	MOTBaseTypeImage		= 0x02,
	MOTBaseTypeAudio		= 0x03,
	MOTBaseTypeVideo		= 0x04,
	MOTBaseTypeTransport		= 0x05,
	MOTBaseTypeSystem		= 0x06,
	MOTBaseTypeApplication		= 0x07,
	MOTBaseTypeProprietary		= 0x3f
};

enum MOTContentType {
	// Masks
	MOTCTBaseTypeMask		= 0x3f00,
	MOTCTSubTypeMask		= 0x00ff,

	// General Data: 0x00xx
	MOTCTGeneralDataObjectTransfer	= 0x0000,
	MOTCTGeneralDataMIMEHTTP	= 0x0001,
	// Text formats: 0x01xx
	MOTCTTextASCII			= 0x0100,
	MOTCTTextLatin1			= 0x0101,
	MOTCTTextHTML			= 0x0102,
	MOTCTTextPDF			= 0x0103,
	// Image formats: 0x02xx
	MOTCTImageGIF			= 0x0200,
	MOTCTImageJFIF			= 0x0201,
	MOTCTImageBMP			= 0x0202,
	MOTCTImagePNG			= 0x0203,
	MOTCTAudioMPEG1Layer1		= 0x0300,
	MOTCTAudioMPEG1Layer2		= 0x0301,
	MOTCTAudioMPEG1Layer3		= 0x0302,
	MOTCTAudioMPEG2Layer1		= 0x0303,
	MOTCTAudioMPEG2Layer2		= 0x0304,
	MOTCTAudioMPEG2Layer3		= 0x0305,
	MOTCTAudioPCM			= 0x0306,
	MOTCTAudioAIFF			= 0x0307,
	MOTCTAudioATRAC			= 0x0308,
	MOTCTAudioUndefined		= 0x0309,
	MOTCTAudioMPEG4			= 0x030a,
	// Video formats: 0x04xx
	MOTCTVideoMPEG1			= 0x0400,
	MOTCTVideoMPEG2			= 0x0401,
	MOTCTVideoMPEG4			= 0x0402,
	MOTCTVideoH263			= 0x0403,
	// MOT transport: 0x05xx
	MOTCTTransportHeaderUpdate	= 0x0500,
	MOTCTTransportHeaderOnly	= 0x0501,
	// System: 0x06xx
	MOTCTSystemMHEG			= 0x0600,
	MOTCTSystemJava			= 0x0601,
	// Application Specific: 0x07xx
	MOTCTApplication		= 0x0700,
	// Proprietary: 0x3fxx
	MOTCTProprietary		= 0x3f00
};

/**
 * Return the base type from the MOTContentType
 */
inline MOTContentBaseType getContentBaseType (MOTContentType ct) {
	return static_cast<MOTContentBaseType>(
			(ct & MOTCTBaseTypeMask)
			>> 8
	);
}

/**
 * Return the sub type from the MOTContentType
 */
inline uint8_t getContentSubType(MOTContentType ct) {
	return static_cast<uint8_t>(
			(ct & MOTCTSubTypeMask)
	);
}

#endif
