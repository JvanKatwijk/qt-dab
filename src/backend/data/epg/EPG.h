/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2006
 *
 * Author(s):
 *	Julian Cable
 *
 * Description:
 *	ETSI DAB/DRM Electronic Programme Guide utilities
 *
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
\******************************************************************************/

#include	<QDomDocument>
#include	<QMap>
#include	<QString>
#include	<QDateTime>
#include	"epgdec.h"

typedef	time_t	TimePointType;
typedef uint32_t DurationType;
typedef string	ContentIdType;
typedef uint32_t TriggerType;
typedef string	CAType;
typedef uint32_t shortCRIDType;
typedef string	CRIDType;
typedef bool	recommendationType;
enum broadcastType {on_air,off_air} ;
typedef QString urlType;
typedef QString mimeType;

class EPGTime {
			EPGTime (void):time(0),actualTime(0),
	                               duration(0),actualDuration(0){}
	TimePointType	time, actualTime;
	DurationType	duration,actualDuration;
	void		parse		(QDomElement);
	time_t		parseTime 	(const QString & time);
	int		parseDuration	(const QString & duration);
	void		augment		(const EPGTime&);
};

class EPGRelativeTime {
			EPGRelativeTime (void):time(0),actualTime(0),
	                                       duration(0),actualDuration(0){}
	DurationType	time, actualTime;
	DurationType	duration,actualDuration;
	void		augment		(const EPGRelativeTime&);
};


class EPGBearer {
			EPGBearer	(void):id (""),trigger (0){}
	ContentIdType	id;
	TriggerType	trigger;
	void		augment		(const EPGBearer&);
};

class LocationType {
	vector<EPGTime> time;
	vector<EPGRelativeTime> relativeTime;
	vector<EPGBearer> bearer;
	void		parse		(QDomElement);
	void		augment		(const LocationType&);
};

struct MessageType {
	QString text;
	QString lang;
	void		parse		(QDomElement);
};

typedef MessageType shortNameType;
typedef MessageType mediumNameType;
typedef MessageType longNameType;
typedef MessageType keywordsType;

struct ScheduleNameGroup {
	vector<shortNameType> shortName;
	vector<mediumNameType> mediumName;
	vector<longNameType> longName;
};


struct GenreType {
	MessageType	name;
	mimeType	mimeValue;
	bool		preferred;
	MessageType	definition;
	QString		href;
	enum {undef,main,secondary,other} type;
	void		parse		(QDomElement);
	void		augment		(const GenreType&);
};

struct memberOfType {
	shortCRIDType	shortId;
	CRIDType	id;
	uint32_t	index;
};

struct linkType {
	urlType		url;
	mimeType	mimeValue;
	string		lang;
	string		description;
	TimePointType	expiryTime;
};

struct MediaDescriptionType {
	vector<MessageType> shortDescription;
	vector<MessageType> longDescription;
	mimeType	mimeValue;
	string		lang;
	urlType		url;
	string		type;
	uint16_t	width;
	uint16_t	height;
	void		parse		(QDomElement);
	void		augment		(const MediaDescriptionType&);
};

struct ProgrammeType : public ScheduleNameGroup {
//	note - schema has 1-unbounded ScheduleNameGroup refs
//	but this makes no sense
	vector<LocationType> location;
	vector<MediaDescriptionType> mediaDescription;
	vector<GenreType> genre;
	CAType CA;
	vector<keywordsType> keywords;
	vector<memberOfType> memberOf;
	vector<linkType> link;
	shortCRIDType shortId;
	CRIDType crid;
	int version;
	recommendationType recommendation;
	broadcastType broadcast;
	uint32_t bitrate;
	string lang;
	vector<ProgrammeType> programmeEvent;
	void parse(QDomElement);
	time_t start() const;
	DurationType duration() const;
	void augment(const ProgrammeType&);
};

class EPG {
public:
		EPG		(CParameter&);
	virtual ~ EPG		(void) {
	   saveChannels (servicesFilename);
	}
	/* assignment operator to help MSVC8 */
	EPG& operator = (const EPG&);

	void	loadChannels	(const QString & fileName);
	void	saveChannels	(const QString & fileName);
	void	addChannel	(const string& label, uint32_t sid);
	void	parseDoc	(const QDomDocument &);
	QDomDocument getFile	(const QDate&, uint32_t, bool);
	QString toHTML		(void) const;
	QString toCSV		(void) const;

	QMap < shortCRIDType, ProgrammeType > progs;
	QMap < QString, QString > genres;
	QString dir, servicesFilename;
	CEPGDecoder basic, advanced;
	CParameter& Parameters;
	time_t min_time, max_time;
private:
	static const
	   struct gl { const char *genre; const char* desc; } genre_list[];
}
