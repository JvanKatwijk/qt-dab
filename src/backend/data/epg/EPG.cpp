/******************************************************************************\
 * British Broadcasting Corporation
 * Copyright (c) 2006-2008
 *
 * Author(s):
 *	Julian Cable
 *
 * Description:
 *	ETSI DAB/DRM Electronic Programme Guide class
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

#include "EPG.h"
#include "epgutil.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QStringList>
#include <QRegExp>
#include <algorithm>
#include <iostream>
#ifdef _WIN32
# include <windows.h>
#endif

const
	EPG::gl
	EPG::genre_list[] = {
	{"1", "Audio-Video"},
	{"1.0", "Proprietary"},
	{"1.1", "PlayRecording"},
	{"1.1.1", "Pure entertainment"},
	{"1.1.2", "Informative entertainment"},
	{"1.10", "Mute"},
	{"1.11", "VolumeUp"},
	{"1.12", "VolumeDown"},
	{"1.13", "Loop/Repeat"},
	{"1.14", "Shuffle"},
	{"1.15", "SkipToStart"},
	{"1.16", "SkipToEnd"},
	{"1.17", "CopyCD"},
	{"1.2", "PlayStream"},
	{"1.2.1", "Government"},
	{"1.2.2", "Pure information"},
	{"1.2.3", "Infotainment"},
	{"1.2.4", "Advice"},
	{"1.3", "Record"},
	{"1.3.1", "School Programmes"},
	{"1.3.1.1", "Primary"},
	{"1.3.1.2", "Secondary"},
	{"1.3.1.3", "Tertiary"},
	{"1.3.2", "Lifelong/further education"},
	{"1.4", "Preview"},
	{"1.5", "Pause"},
	{"1.6", "FastForward"},
	{"1.6.1", "Gambling"},
	{"1.6.2", "Home Shopping"},
	{"1.7", "Rewind"},
	{"1.7.1", "Fund Raising"},
	{"1.7.2", "Social Action"},
	{"1.8", "SkipForward"},
	{"1.8.1", "General enrichment"},
	{"1.8.2", "Inspirational enrichment"},
	{"1.9", "SkipBackward"},
	{"1.9.1", "Very Easy"},
	{"1.9.2", "Easy"},
	{"1.9.3", "Medium"},
	{"1.9.4", "Difficult"},
	{"1.9.5", "Very Difficult"},
	{"10", "For more information"},
	{"11", "Programme review information"},
	{"12", "Recap"},
	{"13", "The making of"},
	{"14", "Support"},
	{"15", "Segmentation"},
	{"16", "Derived"},
	{"17", "TVA RMPI document"},
	{"18", "Content Package"},
	{"2", "Video "},
	{"2.0", "Proprietary"},
	{"2.1", "Zoom"},
	{"2.1.1 ", "Bulletin"},
	{"2.1.2", "Magazine"},
	{"2.1.2.1", "Presenter led magazine"},
	{"2.1.2.2", "Clip led magazine"},
	{"2.1.3", "Event"},
	{"2.1.3.1", "Commented event"},
	{"2.1.3.2", "Uncommented event"},
	{"2.1.4", "Documentary"},
	{"2.1.5", "Discussion/Interview/Debate/Talkshow"},
	{"2.1.6", "Lecture/Speech/Presentation"},
	{"2.1.7", "Textual (incl. relayed teletext)"},
	{"2.1.8", "Phone-in"},
	{"2.1.9", "DJ with discs"},
	{"2.2", "SlowMotion"},
	{"2.2.1", "Fictional portrayal of life"},
	{"2.2.2", "Readings"},
	{"2.2.3", "Dramatic documentary"},
	{"2.3", "CCOn"},
	{"2.3.1", "Anime"},
	{"2.3.2", "Computer"},
	{"2.3.3", "Cartoon"},
	{"2.3.4", "Puppetry"},
	{"2.3.4.1", "Real time puppetry"},
	{"2.3.4.2", "Physical model animation"},
	{"2.4", "StepForward"},
	{"2.4.1", "Hosted show"},
	{"2.4.1.1", "Simple game show"},
	{"2.4.1.2", "Big game show"},
	{"2.4.2", "Panel-show"},
	{"2.4.2.1", "Simple game show"},
	{"2.4.2.2", "Big game show"},
	{"2.4.3", "Non-hosted show"},
	{"2.4.4", "Standup comedian(s)"},
	{"2.4.5", "Reality Show"},
	{"2.4.5.1", "Observational show"},
	{"2.4.5.2", "Controlled show"},
	{"2.5", "StepBackward"},
	{"2.5.1", "Solo performance"},
	{"2.5.2", "Small ensemble performance"},
	{"2.5.3", "Large ensemble performance"},
	{"2.5.4", "Mixed"},
	{"2.6", "void"},
	{"2.7", "INTERACTIVE"},
	{"2.7.1", "LOCAL INTERACTIVITY"},
	{"2.7.1.1", "Static informational"},
	{"2.7.1.10", "Elimination and timer"},
	{"2.7.1.11", "Categories"},
	{"2.7.1.12", "Level based quiz/game"},
	{"2.7.1.13", "Following a sequence"},
	{"2.7.1.14", "Local multi player"},
	{"2.7.1.15", "Multi stream audio-video"},
	{"2.7.1.16", "Enhanced advertisement"},
	{"2.7.1.17", "Logic based games"},
	{"2.7.1.18", "Word games"},
	{"2.7.1.19", "Positional games"},
	{"2.7.1.2", "Dynamic informational"},
	{"2.7.1.20", "Board games"},
	{"2.7.1.21", "Text based gaming"},
	{"2.7.1.22", "Dynamic 2D/3D graphics"},
	{"2.7.1.3", "Viewing chats"},
	{"2.7.1.4", "Quiz - Basic multiple choice"},
	{"2.7.1.5", "Quiz - Text or number entry answers"},
	{"2.7.1.6", "Re-ordering"},
	{"2.7.1.7", "Positional"},
	{"2.7.1.8", "Sync quiz"},
	{"2.7.1.9", "Timer quiz"},
	{"2.7.2", "INTERMITTENT RESPONSE"},
	{"2.7.2.1", "Single impulse vote"},
	{"2.7.2.10", "Multi player TS networked services/games"},
	{"2.7.2.11", "Interactive advertisement"},
	{"2.7.2.2", "Impulse vote from choices"},
	{"2.7.2.3", "Impulse Yes/No vote"},
	{"2.7.2.4", "Impulse vote with a value"},
	{"2.7.2.5", "Submit answers/form"},
	{"2.7.2.6", "SMS using mobile"},
	{"2.7.2.7", "SMS using TV remote"},
	{"2.7.2.8", "Impulse gambling"},
	{"2.7.2.9", "Impulse transaction"},
	{"2.7.3", "ALWAYS ON CONNECTION"},
	{"2.7.3.1", "Chat Forum"},
	{"2.7.3.10", "Impulse transaction"},
	{"2.7.3.11", "Non-linear audio-video"},
	{"2.7.3.2", "Chat Forum via web"},
	{"2.7.3.3", "Threaded mail discussions"},
	{"2.7.3.4", "Point to point"},
	{"2.7.3.5", "3rd party point to point"},
	{"2.7.3.6", "Voice chat using mic capability"},
	{"2.7.3.7", "Dual player networked services/games"},
	{"2.7.3.8", "Multi player RT networked services/games"},
	{"2.7.3.9", "Gambling services"},
	{"3", "Data"},
    {"3.0", "Proprietary"},
    {"3.1", "NON-FICTION/INFORMATION"},
    {"3.1.1", "News"},
    {"3.1.1.1", "Daily news"},
    {"3.1.1.2", "Special news/edition"},
    {"3.1.1.3", "Special News Report"},
    {"3.1.1.4", "News Commentary"},
    {"3.1.1.5", "News Periodical/General"},
    {"3.1.1.6", "National politics/National assembly"},
    {"3.1.1.7", "Business News"},
    {"3.1.1.8", "International News"},
    {"3.1.1.9", "Sports News"},
    {"3.1.1.10", "Cultural News"},
    {"3.1.1.10.1", "Arts News"},
    {"3.1.1.10.2", "Entertainment News"},
    {"3.1.1.10.3", "Film News"},
    {"3.1.1.10.4", "Music News"},
    {"3.1.1.10.5", "Radio in the News"},
    {"3.1.1.10.6", "TV in the News"},
    {"3.1.1.11", "Regional News"},
    {"3.1.1.12", "Traffic Information"},
    {"3.1.1.13", "Weather forecasts"},
    {"3.1.1.14", "Service information"},
    {"3.1.1.15", "Public affairs"},
    {"3.1.1.16", "Current affairs"},
    {"3.1.1.17", "Consumer affairs"},
    {"3.1.2", "Religion/Philosophies"},
    {"3.1.2.1", "Religion"},
    {"3.1.2.1.1", "Buddhism"},
    {"3.1.2.1.2", "Hinduism"},
    {"3.1.2.1.3", "Christianity"},
    {"3.1.2.1.4", "Islam"},
    {"3.1.2.1.5", "Judaism"},
    {"3.1.2.1.8", "Shintoism"},
    {"3.1.2.1.9", "Baha'i"},
    {"3.1.2.1.10", "Confucianism"},
    {"3.1.2.1.11", "Jainism"},
    {"3.1.2.1.12", "Sikhism"},
    {"3.1.2.1.13", "Taoism"},
    {"3.1.2.1.14", "Vodun (Voodoo)"},
    {"3.1.2.1.15", "Asatru (Nordic Paganism)"},
    {"3.1.2.1.16", "Drudism"},
    {"3.1.2.1.17", "Goddess worship"},
    {"3.1.2.1.18", "Wicca"},
    {"3.1.2.1.19", "Witchcraft"},
    {"3.1.2.1.20", "Caodaism"},
    {"3.1.2.1.21", "Damanhur Community"},
    {"3.1.2.1.22", "Druse (Mowahhidoon)"},
    {"3.1.2.1.23", "Eckankar"},
    {"3.1.2.1.24", "Gnosticism"},
    {"3.1.2.1.25", "Rroma (Gypsies)"},
    {"3.1.2.1.26", "Hare Krishna and ISKCON"},
    {"3.1.2.1.27", "Lukumi (Santeria)"},
    {"3.1.2.1.28", "Macumba"},
    {"3.1.2.1.29", "Native American spirituality"},
    {"3.1.2.1.30", "New Age"},
    {"3.1.2.1.31", "Osho"},
    {"3.1.2.1.32", "Satanism"},
    {"3.1.2.1.33", "Scientology"},
    {"3.1.2.1.34", "Thelema"},
    {"3.1.2.1.35", "Unitarian-Universalism"},
    {"3.1.2.1.36", "The Creativity Movement"},
    {"3.1.2.1.37", "Zoroastrianism"},
    {"3.1.2.1.38", "Quakerism"},
    {"3.1.2.1.39", "Rastafarianism"},
    {"3.1.2.2", "Non-religious philosophies"},
    {"3.1.2.2.1", "Communism"},
    {"3.1.2.2.2", "Humanism"},
    {"3.1.2.2.5", "Libertarianism"},
    {"3.1.2.2.7", "Deism"},
    {"3.1.2.2.8", "Falun Gong and Falun Dafa"},
    {"3.1.2.2.9", "Objectivism"},
    {"3.1.2.2.10", "Universism"},
    {"3.1.2.2.11", "Atheism"},
    {"3.1.2.2.12", "Agnosticism"},
    {"3.1.3", "General non-fiction"},
    {"3.1.3.1", "Political"},
    {"3.1.3.1.1", "Capitalism"},
    {"3.1.3.1.2", "Fascism"},
    {"3.1.3.1.3", "Republicanism"},
    {"3.1.3.1.4", "Socialism"},
    {"3.1.3.2", "Social"},
    {"3.1.3.3", "Economic"},
    {"3.1.3.4", "Legal"},
    {"3.1.3.5", "Finance"},
    {"3.1.3.6", "Education"},
    {"3.1.3.6.1", "Pre-School"},
    {"3.1.3.6.2", "Primary Education"},
    {"3.1.3.6.3", "Secondary Education"},
    {"3.1.3.6.4", "Colleges and Universities"},
    {"3.1.3.6.5", "Adult education"},
    {"3.1.3.6.6", "Non-formal education"},
    {"3.1.3.6.7", "Homework"},
    {"3.1.3.6.8", "Reading groups"},
    {"3.1.3.6.9", "Distance learning"},
    {"3.1.3.6.10", "Religious schools"},
    {"3.1.3.6.11", "Student organisations"},
    {"3.1.3.6.12", "Testing in Education"},
    {"3.1.3.6.13", "Theory and methods of Education"},
    {"3.1.3.6.14", "Interdisciplinary studies"},
    {"3.1.3.7", "International affairs"},
    {"3.1.3.8", "Military/Defence"},
    {"3.1.3.9", "Industry/Manufacturing"},
    {"3.1.3.10", "Agriculture"},
    {"3.1.3.11", "Construction"},
    {"3.1.4", "Arts"},
    {"3.1.4.1", "Music"},
    {"3.1.4.2", "Dance"},
    {"3.1.4.3", "Theatre"},
    {"3.1.4.4", "Opera"},
    {"3.1.4.5", "Cinema"},
    {"3.1.4.6", "Poetry"},
    {"3.1.4.8", "Plastic arts"},
    {"3.1.4.9", "Fine arts"},
    {"3.1.4.10", "Experimental arts"},
    {"3.1.4.11", "Architecture"},
    {"3.1.4.12", "Showbiz"},
    {"3.1.5", "Humanities"},
    {"3.1.5.1", "Literature"},
    {"3.1.5.2", "Languages"},
    {"3.1.5.3", "History"},
    {"3.1.5.4", "Culture/Tradition/Anthropology/Ethnic studies"},
    {"3.1.5.5", "War/Conflict"},
    {"3.1.5.6", "Philosophy"},
    {"3.1.5.7", "Political Science"},
    {"3.1.6", "Sciences"},
    {"3.1.6.1", "Applied sciences"},
    {"3.1.6.2", "Nature"},
    {"3.1.6.2.1", "Biology"},
    {"3.1.6.2.2", "Geology"},
    {"3.1.6.2.3", "Botany"},
    {"3.1.6.2.4", "Zoology"},
    {"3.1.6.3", "Animals/Wildlife"},
    {"3.1.6.4", "Environment"},
    {"3.1.6.5", "Space"},
    {"3.1.6.6", "Physical sciences"},
    {"3.1.6.6.1", "Physics"},
    {"3.1.6.6.2", "Chemistry"},
    {"3.1.6.6.3", "Mechanics"},
    {"3.1.6.6.4", "Engineering"},
    {"3.1.6.7", "Medicine"},
    {"3.1.6.7.1", "Alternative Medicine"},
    {"3.1.6.8", "Technology"},
    {"3.1.6.9", "Physiology"},
    {"3.1.6.10", "Psychology"},
    {"3.1.6.11", "Social"},
    {"3.1.6.12", "Spiritual"},
    {"3.1.6.13", "Mathematics"},
    {"3.1.6.14", "Archaeology"},
    {"3.1.6.15", "Statistics"},
    {"3.1.6.16", "Liberal Arts and Science"},
    {"3.1.7", "Human interest"},
    {"3.1.7.1", "Reality"},
    {"3.1.7.2", "Society/Show business/Gossip"},
    {"3.1.7.3", "Biography/Notable personalities"},
    {"3.1.7.4", "Personal problems"},
    {"3.1.7.5", "Investigative journalism"},
    {"3.1.7.6", "Museums"},
    {"3.1.7.7", "Religious buildings"},
    {"3.1.7.8", "Personal stories"},
    {"3.1.7.9", "Family life"},
    {"3.1.7.10", "libraries"},
    {"3.1.8", "Transport and Communications"},
    {"3.1.8.1", "Air"},
    {"3.1.8.2", "Land"},
    {"3.1.8.3", "Sea"},
    {"3.1.8.4", "Space"},
    {"3.1.9", "Events"},
    {"3.1.9.1", "Anniversary"},
    {"3.1.9.2", "Fairs"},
    {"3.1.9.3", "Tradeshows"},
    {"3.1.9.4", "Musicals"},
    {"3.1.9.5", "Exhibition"},
    {"3.1.9.6", "Royal Events"},
    {"3.1.9.7", "State Events"},
    {"3.1.9.8", "International Events"},
    {"3.1.9.9", "National Events"},
    {"3.1.9.10", "Local/Regional Events"},
    {"3.1.9.11", "Seasonal Events"},
    {"3.1.9.12", "Sporting Events"},
    {"3.1.9.13", "Festivals"},
    {"3.1.9.14", "Concerts"},
    {"3.1.9.15", "Funeral/Memorial"},
    {"3.1.10", "Media"},
    {"3.1.10.1", "Advertising"},
    {"3.1.10.2", "Print media"},
    {"3.1.10.3", "Television"},
    {"3.1.10.4", "Radio"},
    {"3.1.10.5", "New media"},
    {"3.1.10.6", "Marketing"},
    {"3.1.11", "Listings"},
    {"3.2", "Sports"},
    {"3.2.1", "Athletics"},
    {"3.2.1.1", "Field"},
    {"3.2.1.2", "Track"},
    {"3.2.1.3", "Combined athletics"},
    {"3.2.1.4", "Running"},
    {"3.2.1.5", "Cross-country"},
    {"3.2.1.6", "Triathlon"},
    {"3.2.2", "Cycling/Bicycle"},
    {"3.2.2.1", "Mountainbike"},
    {"3.2.2.2", "Bicross"},
    {"3.2.2.3", "Indoor cycling"},
    {"3.2.2.4", "Road cycling"},
    {"3.2.3", "Team sports"},
    {"3.2.3.1", "Football (American)"},
    {"3.2.3.2", "Football (Australian)"},
    {"3.2.3.3", "Football (Gaelic)"},
    {"3.2.3.4", "Football (Indoor)"},
    {"3.2.3.5", "Beach soccer"},
    {"3.2.3.6", "Bandy"},
    {"3.2.3.7", "Baseball"},
    {"3.2.3.8", "Basketball"},
    {"3.2.3.9", "Cricket"},
    {"3.2.3.10", "Croquet"},
    {"3.2.3.11", "Faustball"},
    {"3.2.3.12", "Football (Soccer)"},
    {"3.2.3.13", "Handball"},
    {"3.2.3.14", "Hockey"},
    {"3.2.3.15", "Korfball"},
    {"3.2.3.16", "Lacrosse"},
    {"3.2.3.17", "Netball"},
    {"3.2.3.18", "Roller skating"},
    {"3.2.3.19", "Rugby"},
    {"3.2.3.19.1", "Rugby union"},
    {"3.2.3.19.2", "Rugby league"},
    {"3.2.3.20", "Softball"},
    {"3.2.3.21", "Volleyball"},
    {"3.2.3.22", "Beach volley"},
    {"3.2.3.23", "Hurling"},
    {"3.2.3.24", "Frisbee"},
    {"3.2.3.25", "Kabadi"},
    {"3.2.3.26", "Camogie"},
    {"3.2.3.27", "Shinty"},
    {"3.2.3.28", "Street Soccer"},
    {"3.2.4", "Racket sports"},
    {"3.2.4.1", "Badminton"},
    {"3.2.4.2", "Racketball"},
    {"3.2.4.3", "Short tennis"},
    {"3.2.4.4", "Soft tennis"},
    {"3.2.4.5", "Squash"},
    {"3.2.4.6", "Table tennis"},
    {"3.2.4.7", "Tennis"},
    {"3.2.5", "Martial Arts"},
    {"3.2.5.1", "Aikido"},
    {"3.2.5.2", "Jai-alai"},
    {"3.2.5.3", "Judo"},
    {"3.2.5.4", "Ju-jitsu"},
    {"3.2.5.5", "Karate"},
    {"3.2.5.6", "Sumo/Fighting games"},
    {"3.2.5.7", "Sambo"},
    {"3.2.5.8", "Taekwondo"},
    {"3.2.6", "Water sports"},
    {"3.2.6.1", "Bodyboarding"},
    {"3.2.6.2", "Yatching"},
    {"3.2.6.3", "Canoeing"},
    {"3.2.6.4", "Diving"},
    {"3.2.6.5", "Fishing"},
    {"3.2.6.6", "Polo"},
    {"3.2.6.7", "Rowing"},
    {"3.2.6.8", "Sailing"},
    {"3.2.6.9", "Sub-aquatics"},
    {"3.2.6.10", "Surfing"},
    {"3.2.6.11", "Swimming"},
    {"3.2.6.12", "Water polo"},
    {"3.2.6.13", "Water skiing"},
    {"3.2.6.14", "Windsurfing"},
    {"3.2.7", "Winter sports"},
    {"3.2.7.1", "Bobsleigh/Tobogganing"},
    {"3.2.7.2", "Curling"},
    {"3.2.7.3", "Ice-hockey"},
    {"3.2.7.4", "Ice-skating"},
    {"3.2.7.5", "Luge"},
    {"3.2.7.6", "Skating"},
    {"3.2.7.7", "Skibob"},
    {"3.2.7.8", "Skiing"},
    {"3.2.7.9", "Sleddog"},
    {"3.2.7.10", "Snowboarding"},
    {"3.2.7.11", "Alpine skiing"},
    {"3.2.7.12", "Freestyle skiing "},
    {"3.2.7.13", "Inline skating"},
    {"3.2.7.14", "Nordic skiing"},
    {"3.2.7.15", "Ski jumping"},
    {"3.2.7.16", "Speed skating"},
    {"3.2.7.17", "Figure skating"},
    {"3.2.7.18", "Ice-dance"},
    {"3.2.7.19", "Marathon"},
    {"3.2.7.20", "Short-track"},
    {"3.2.7.21", "Biathlon"},
    {"3.2.8", "Motor sports"},
    {"3.2.8.1", "Auto racing"},
    {"3.2.8.2", "Motor boating"},
    {"3.2.8.3", "Motor cycling"},
    {"3.2.8.4", "Formula 1"},
    {"3.2.8.5", "Indy car"},
    {"3.2.8.6", "Karting"},
    {"3.2.8.7", "Rally"},
    {"3.2.8.8", "Trucking"},
    {"3.2.8.9", "Tractor pulling"},
    {"3.2.8.10", "Stock car"},
    {"3.2.8.11", "Hill Climb"},
    {"3.2.8.12", "Trials"},
    {"3.2.9", "'Social' sports"},
    {"3.2.9.1", "Billiards"},
    {"3.2.9.2", "Boules"},
    {"3.2.9.3", "Bowling"},
    {"3.2.9.5", "Dance sport"},
    {"3.2.9.6", "Darts"},
    {"3.2.9.7", "Pool"},
    {"3.2.9.8", "Snooker"},
    {"3.2.9.9", "Tug-of-war"},
    {"3.2.9.10", "Balle pelote"},
    {"3.2.9.11", "Basque pelote"},
    {"3.2.9.12", "Trickshot"},
    {"3.2.10", "Gymnastics"},
    {"3.2.10.1", "Asymmetric bars"},
    {"3.2.10.2", "Beam"},
    {"3.2.10.3", "Horse"},
    {"3.2.10.4", "Mat"},
    {"3.2.10.5", "Parallel bars"},
    {"3.2.10.6", "Rings"},
    {"3.2.10.7", "Trampoline"},
    {"3.2.11", "Equestrian"},
    {"3.2.11.1", "Cart"},
    {"3.2.11.2", "Dressage"},
    {"3.2.11.3", "Horse racing"},
    {"3.2.11.4", "Polo"},
    {"3.2.11.5", "Jumping"},
    {"3.2.11.6", "Crossing"},
    {"3.2.11.7", "Trotting"},
    {"3.2.12", "Adventure sports"},
    {"3.2.12.1", "Archery"},
    {"3.2.12.2", "Extreme sports"},
    {"3.2.12.3", "Mountaineering"},
    {"3.2.12.4", "Climbing"},
    {"3.2.12.5", "Orienteering"},
    {"3.2.12.6", "Shooting"},
    {"3.2.12.7", "Sport acrobatics"},
    {"3.2.12.8", "Rafting"},
    {"3.2.12.9", "Caving"},
    {"3.2.12.10", "Skateboarding"},
    {"3.2.12.11", "Treking"},
    {"3.2.13", "Strength-based sports"},
    {"3.2.13.1", "Body-building"},
    {"3.2.13.2", "Boxing"},
    {"3.2.13.3", "Combative sports"},
    {"3.2.13.4", "Power-lifting"},
    {"3.2.13.5", "Weight-lifting"},
    {"3.2.13.6", "Wrestling"},
    {"3.2.14", "Air sports"},
    {"3.2.14.1", "Ballooning"},
    {"3.2.14.2", "Hang gliding"},
    {"3.2.14.3", "Sky diving"},
    {"3.2.14.4", "Delta-plane"},
    {"3.2.14.5", "Parachuting"},
    {"3.2.14.6", "Kiting"},
    {"3.2.14.7", "Aeronautics"},
    {"3.2.14.8", "Gliding"},
    {"3.2.14.9", "Flying"},
    {"3.2.14.10", "Aerobatics"},
    {"3.2.15", "Golf"},
    {"3.2.16", "Fencing"},
    {"3.2.17", "Dog racing"},
    {"3.2.18", "Casting"},
    {"3.2.19", "Maccabi"},
    {"3.2.20", "Modern Pentathlon"},
    {"3.2.21", "Sombo"},
    {"3.2.22", "Mind Games"},
    {"3.2.22.1", "Bridge"},
    {"3.2.22.2", "Chess"},
    {"3.2.22.3", "Poker"},
    {"3.2.23", "Traditional Games"},
    {"3.2.24", "Disabled Sport"},
    {"3.2.24.1", "Physically Challenged"},
    {"3.2.24.2", "Mentally Challenged"},
    {"3.4", "FICTION/DRAMA"},
    {"3.4.1", "General light drama"},
    {"3.4.2", "Soap"},
    {"3.4.2.1", "Soap opera"},
    {"3.4.2.2", "Soap special"},
    {"3.4.2.3", "Soap talk"},
    {"3.4.3", "Romance"},
    {"3.4.4", "Legal Melodrama"},
    {"3.4.5", "Medical melodrama"},
    {"3.4.6", "Action"},
    {"3.4.6.1", "Adventure"},
    {"3.4.6.2", "Disaster"},
    {"3.4.6.3", "Mystery"},
    {"3.4.6.4", "Detective/Police"},
    {"3.4.6.5", "Historical/Epic"},
    {"3.4.6.6", "Horror"},
    {"3.4.6.7", "Science fiction"},
    {"3.4.6.8", "War"},
    {"3.4.6.9", "Western"},
    {"3.4.6.10", "Thriller"},
    {"3.4.6.11", "Sports"},
    {"3.4.6.12", "Martial arts"},
    {"3.4.6.13", "Epic"},
    {"3.4.7", "Fantasy/Fairy tale"},
    {"3.4.8", "Erotica"},
    {"3.4.9", "Docudrama"},
    {"3.4.10", "Musical"},
    {"3.4.13", "Classical drama"},
    {"3.4.14", "Period drama"},
    {"3.4.15", "Contemporary drama"},
    {"3.4.16", "Religious drama"},
    {"3.4.17", "Poems/Stories"},
    {"3.4.18", "Biography"},
    {"3.4.19", "Psychological drama"},
    {"3.4.20", "Political Drama"},
    {"3.5", "AMUSEMENT/ENTERTAINMENT"},
    {"3.5.2", "Quiz/Contest"},
    {"3.5.2.1", "Quiz"},
    {"3.5.2.2", "Contest"},
    {"3.5.3", "Variety/Talent"},
    {"3.5.3.1", "Cabaret"},
    {"3.5.3.2", "Talent"},
    {"3.5.4", "Surprise"},
    {"3.5.5", "Reality"},
    {"3.5.7", "Comedy"},
    {"3.5.7.1", "Broken comedy"},
    {"3.5.7.2", "Romantic comedy"},
    {"3.5.7.3", "Sitcom"},
    {"3.5.7.4", "Satire"},
    {"3.5.7.5", "Candid Camera"},
    {"3.5.7.6", "Humour"},
    {"3.5.7.7", "Spoof"},
    {"3.5.10", "Magic/Hypnotism"},
    {"3.5.11", "Circus"},
    {"3.5.12", "Dating"},
    {"3.5.13", "Bullfighting"},
    {"3.5.14", "Rodeo"},
    {"3.5.16", "Chat"},
    {"3.6", "Music"},
    {"3.6.1", "Classical music"},
    {"3.6.1.1", "Early"},
    {"3.6.1.2", "Classical"},
    {"3.6.1.3", "Romantic"},
    {"3.6.1.4", "Contemporary"},
    {"3.6.1.5", "Light classical"},
    {"3.6.1.6", "Middle Ages"},
    {"3.6.1.7", "Renaissance"},
    {"3.6.1.8", "Baroque"},
    {"3.6.1.9", "Opera"},
    {"3.6.1.10", "Solo instruments"},
    {"3.6.1.11", "Chamber"},
    {"3.6.1.12", "Symphonic"},
    {"3.6.1.13", "Vocal"},
    {"3.6.1.14", "Choral"},
    {"3.6.1.15", "Song"},
    {"3.6.1.16", "Orchestral"},
    {"3.6.1.17", "Organ"},
    {"3.6.1.18", "String Quartet"},
    {"3.6.1.19", "Experimental/Avant Garde"},
    {"3.6.2", "Jazz"},
    {"3.6.2.1", "New Orleans/Early jazz"},
    {"3.6.2.2", "Big band/Swing/Dixie"},
    {"3.6.2.3", "Blues/Soul jazz"},
    {"3.6.2.4", "Bop/Hard bop/Bebop/Postbop"},
    {"3.6.2.5", "Traditional/Smooth jazz"},
    {"3.6.2.6", "Cool jazz"},
    {"3.6.2.7", "Modern/Avant-garde/Free jazz"},
    {"3.6.2.8", "Latin and World jazz"},
    {"3.6.2.9", "Pop jazz/Jazz funk"},
    {"3.6.2.10", "Acid jazz/Fusion"},
    {"3.6.3", "Background music"},
    {"3.6.3.1", "Middle-of-the-road"},
    {"3.6.3.2", "Easy listening"},
    {"3.6.3.3", "Ambient"},
    {"3.6.3.4", "Mood music"},
    {"3.6.3.5", "Oldies"},
    {"3.6.3.6", "Love songs"},
    {"3.6.3.7", "Dance hall"},
    {"3.6.3.8", "Soundtrack"},
    {"3.6.3.9", "Trailer"},
    {"3.6.3.10", "Showtunes"},
    {"3.6.3.11", "TV"},
    {"3.6.3.12", "Cabaret"},
    {"3.6.3.13", "Instrumental"},
    {"3.6.3.14", "Sound clip"},
    {"3.6.3.15", "Retro"},
    {"3.6.4", "Pop-rock"},
    {"3.6.4.1", "Pop"},
    {"3.6.4.2", "Chanson/Ballad"},
    {"3.6.4.3", "Traditional rock and roll"},
    {"3.6.4.5", "Classic/Dance/Pop-rock"},
    {"3.6.4.6", "Folk"},
    {"3.6.4.8", "New Age"},
    {"3.6.4.11", "Seasonal/Holiday"},
    {"3.6.4.12", "Japanese pop-rock"},
    {"3.6.4.13", "Karaoke/Singing contests"},
    {"3.6.4.14", "Rock"},
    {"3.6.4.14.1", "AOR / Slow Rock / Soft Rock"},
    {"3.6.4.14.2", "Metal"},
    {"3.6.4.14.3", "Glam Rock"},
    {"3.6.4.14.4", "Punk Rock"},
    {"3.6.4.14.5", "Prog / Symphonic Rock"},
    {"3.6.4.14.6", "Alternative / Indie"},
    {"3.6.4.14.7", "Experimental / Avant Garde"},
    {"3.6.4.14.8", "Art Rock"},
    {"3.6.4.14.9", "Folk Rock"},
    {"3.6.4.14.10", "Nu Punk"},
    {"3.6.4.14.11", "Grunge"},
    {"3.6.4.14.12", "Garage Punk/Psychedelia"},
    {"3.6.4.14.13", "Heavy Rock"},
    {"3.6.4.15", "New Wave"},
    {"3.6.4.16", "Easy listening / Exotica"},
    {"3.6.4.17", "Singer/Songwriter"},
    {"3.6.5", "Blues/Rhythm and Blues/Soul/Gospel"},
    {"3.6.5.1", "Blues"},
    {"3.6.5.2", "R and B"},
    {"3.6..5.2.1", "Hip Hop Soul"},
    {"3.6..5.2.2", "Neo Soul"},
    {"3.6..5.2.3", "New Jack Swing"},
    {"3.6.5.3", "Soul"},
    {"3.6.5.4", "Gospel"},
    {"3.6.5.5", "Rhythm and Blues"},
    {"3.6.5.6", "Funk"},
    {"3.6.5.6.1", "Afro Funk"},
    {"3.6.5.6.2", "Rare Groove"},
    {"3.6.6", "Country and Western"},
    {"3.6.7", "Rap/Hip Hop/Reggae"},
    {"3.6.7.1", "Rap/Christian rap"},
    {"3.6.7.1.1", "Gangsta Rap"},
    {"3.6.7.2", "Hip Hop/Trip-Hop"},
    {"3.6.7.2.1", "Dirty South Hip Hop"},
    {"3.6.7.2.2", "East Coast Hip Hop"},
    {"3.6.7.2.4", "UK Hip Hop"},
    {"3.6.7.2.5", "West Coast Hip Hop"},
    {"3.6.7.3", "Reggae"},
    {"3.6.7.3.1", "Dancehall"},
    {"3.6.7.3.2", "Dub"},
    {"3.6.7.3.3", "Lovers Rock"},
    {"3.6.7.3.4", "Raggamuffin"},
    {"3.6.7.3.5", "Rocksteady"},
    {"3.6.7.3.6", "Ska"},
    {"3.6.8", "Electronic/Club/Urban/Dance"},
    {"3.6.8.1", "Acid/Punk/Acid Punk"},
    {"3.6.8.2", "Disco"},
    {"3.6.8.3", "Techno/Euro-Techno/Techno-Industrial/Industrial"},
    {"3.6.8.4", "House/Techno House"},
    {"3.6.8.4.1", "Progressive House"},
    {"3.6.8.5", "Rave"},
    {"3.6.8.6", "Jungle/Tribal"},
    {"3.6.8.7", "Trance"},
    {"3.6.8.11", "Drum and Bass"},
    {"3.6.8.14", "Dance/Dance-pop"},
    {"3.6.8.15", "Garage (1990s)"},
    {"3.6.8.16", "UK Garage"},
    {"3.6.8.16.1", "2 Step"},
    {"3.6.8.16.2", "4/4 Vocal Garage"},
    {"3.6.8.16.3", "8 Bar"},
    {"3.6.8.16.4", "Dubstep"},
    {"3.6.8.16.5", "Eski-Beat"},
    {"3.6.8.16.6", "Grime"},
    {"3.6.8.16.7", "Soulful House and Garage"},
    {"3.6.8.16.8", "Speed Garage"},
    {"3.6.8.16.9", "Sublow"},
    {"3.6.8.17", "Breakbeat"},
    {"3.6.8.18", "Broken Beat"},
    {"3.6.8.22", "Ambient Dance"},
    {"3.6.8.23", "Alternative Dance"},
    {"3.6.9", "World/Traditional/Ethnic/Folk music"},
    {"3.6.9.1", "Africa"},
    {"3.6.9.2", "Asia"},
    {"3.6.9.3", "Australia/Oceania"},
    {"3.6.9.4", "Caribbean"},
    {"3.6.9.4.1", "Calypso"},
    {"3.6.9.4.2", "SOCA"},
    {"3.6.9.5", "Europe"},
    {"3.6.9.6", "Latin America"},
    {"3.6.9.7", "Middle East"},
    {"3.6.9.8", "North America"},
    {"3.6.9.9", "Fusion"},
    {"3.6.9.10", "Modern"},
    {"3.6.10", "Hit-Chart/Song Requests"},
    {"3.6.11", "Children's songs"},
    {"3.6.12", "Event music"},
    {"3.6.12.1", "Wedding music"},
    {"3.6.12.2", "Sports music"},
    {"3.6.12.3", "Ceremonial/Chants"},
    {"3.6.13", "Spoken"},
    {"3.6.14", "Dance"},
    {"3.6.14.1", "Ballet"},
    {"3.6.14.2", "Tap"},
    {"3.6.14.3", "Modern"},
    {"3.6.14.4", "Classical"},
    {"3.6.14.5", "Ballroom"},
    {"3.6.15", "Religious music"},
    {"3.6.16", "Era"},
    {"3.6.16.1", "Medieval (before 1400)"},
    {"3.6.16.2", "Renaissance (1400-1600)"},
    {"3.6.16.3", "Baroque (1600-1760)"},
    {"3.6.16.4", "Classical (1730-1820)"},
    {"3.6.16.5", "Romantic (1815-1910"},
    {"3.6.16.6", "20th Century"},
    {"3.6.16.6.1", "1910s"},
    {"3.6.16.6.2", "1920s"},
    {"3.6.16.6.3", "1930s"},
    {"3.6.16.6.4", "1940s"},
    {"3.6.16.6.5", "1950s"},
    {"3.6.16.6.6", "1960s"},
    {"3.6.16.6.7", "1970s"},
    {"3.6.16.6.8", "1980s"},
    {"3.6.16.6.9", "1990s"},
    {"3.6.16.7", "21st Century"},
    {"3.6.16.7.1", "2000s"},
    {"3.6.16.7.2", "2010s"},
    {"3.6.16.7.3", "2020s"},
    {"3.6.16.7.4", "2030s"},
    {"3.6.16.7.5", "2040s"},
    {"3.6.16.7.6", "2050s"},
    {"3.6.16.7.7", "2060s"},
    {"3.6.16.7.8", "2070s"},
    {"3.6.16.7.9", "2080s"},
    {"3.6.16.7.10", "2090s"},
    {"3.7", "INTERACTIVE GAMES"},
    {"3.7.1", "CONTENT GAMES CATEGORIES"},
    {"3.7.1.1", "Action"},
    {"3.7.1.2", "Adventure"},
    {"3.7.1.3", "Fighting"},
    {"3.7.1.4", "Online"},
    {"3.7.1.5", "Platform"},
    {"3.7.1.6", "Puzzle"},
    {"3.7.1.7", "RPG/ MUDs"},
    {"3.7.1.8", "Racing"},
    {"3.7.1.9", "Simulation"},
    {"3.7.1.10", "Sports"},
    {"3.7.1.11", "Strategy"},
    {"3.7.1.12", "Wrestling"},
    {"3.7.1.13", "Classic/Retro"},
    {"3.7.2", "STYLE"},
    {"3.7.2.1", "Logic based"},
    {"3.7.2.2", "Word games"},
    {"3.7.2.3", "Positional"},
    {"3.7.2.4", "Board games"},
    {"3.7.2.5", "Text environments"},
    {"3.7.2.6", "Dynamic 2D/3D graphics"},
    {"3.7.2.7", "Non-linear video"},
    {"3.8", "LEISURE/HOBBY/LIFESTYLE"},
    {"3.8.1", "General Consumer Advice"},
    {"3.8.1.1", "Road safety"},
    {"3.8.1.2", "Consumer advice"},
    {"3.8.1.3", "Employment Advice"},
    {"3.8.1.4", "Self-help"},
    {"3.8.2", "Computing/Technology"},
    {"3.8.2.1", "Technology/Computing"},
    {"3.8.2.2", "Computer Games"},
    {"3.8.3", "Cookery, Food, Drink"},
    {"3.8.3.1", "Cookery"},
    {"3.8.3.2", "Food and Drink"},
    {"3.8.4", "Homes/Interior/Gardening"},
    {"3.8.4.1", "Do-it-yourself"},
    {"3.8.4.2", "Home Improvement"},
    {"3.8.4.3", "Gardening"},
    {"3.8.4.4", "Property Buying and Selling"},
    {"3.8.5", "Hobbies"},
    {"3.8.5.1", "Fishing"},
    {"3.8.5.2", "Pets"},
    {"3.8.5.3", "Craft/Handicraft"},
    {"3.8.5.4", "Art"},
    {"3.8.5.5", "Music"},
    {"3.8.5.6", "Board Games"},
    {"3.8.5.7", "Card Cames"},
    {"3.8.5.8", "Gaming"},
    {"3.8.5.9", "Shopping"},
    {"3.8.5.10", "Collectibles/Antiques"},
    {"3.8.5.11", "Jewellery"},
    {"3.8.5.12", "Aviation"},
    {"3.8.5.13", "Trains"},
    {"3.8.5.14", "Boating"},
    {"3.8.5.15", "Ornithology"},
    {"3.8.6", "Cars and Motoring"},
    {"3.8.6.1", "Car"},
    {"3.8.6.2", "Motorcycle"},
    {"3.8.7", "Personal/Lifestyle/Family"},
    {"3.8.7.1", "Fitness / Keep-fit"},
    {"3.8.7.2", "Personal health"},
    {"3.8.7.3", "Fashion"},
    {"3.8.7.4", "House Keeping"},
    {"3.8.7.5", "Parenting"},
    {"3.8.7.6", "Beauty"},
    {"3.8.9", "Travel/Tourism"},
    {"3.8.9.1", "Holidays"},
    {"3.8.9.2", "Adventure/Expeditions"},
    {"3.8.9.3", "Outdoor pursuits"},
    {"3.9", "Adult"},
	{"4", "Commerce"},
	{"4.0", "Proprietary"},
	{"4.1", "Buy"},
	{"4.11", "LANGUAGE OF TARGET AUDIENCE"},
	{"4.2", "AddToWishList"},
	{"4.2.1", "Children"},
	{"4.2.1.0", "specific single age"},
	{"4.2.1.1", "age 4-7"},
	{"4.2.1.2", "age 8-13"},
	{"4.2.1.3", "age 14-15"},
	{"4.2.1.4", "age 0-3"},
	{"4.2.2", "Adults"},
	{"4.2.2.1", "age 16-17"},
	{"4.2.2.2", "age 18-24"},
	{"4.2.2.3", "age 25-34"},
	{"4.2.2.4", "age 35-44"},
	{"4.2.2.5", "age 45-54"},
	{"4.2.2.6", "age 55-64"},
	{"4.2.2.7", "age 65+"},
	{"4.2.2.8", "specific single age"},
	{"4.2.3", "Adults"},
	{"4.2.3.1", "Age 25-34"},
	{"4.2.3.2", "Age 35-44"},
	{"4.2.3.3", "Age 45-54"},
	{"4.2.3.4", "Age 55-64"},
	{"4.2.3.5", "Age 65+"},
	{"4.2.3.6", "Specific single age"},
	{"4.2.4", "All ages"},
	{"4.3", "AddToCart"},
	{"4.3.1", "Ethnic"},
	{"4.3.1.1", "Immigrant groups"},
	{"4.3.1.2", "Indigineous"},
	{"4.3.2", "Religious"},
	{"4.4", "OCCUPATIONAL GROUPS"},
	{"4.4.1", "AB"},
	{"4.4.1.1", "A"},
	{"4.4.1.2", "B"},
	{"4.4.2", "C1C2"},
	{"4.4.2.1", "C1"},
	{"4.4.2.2", "C2"},
	{"4.4.3", "DE"},
	{"4.4.3.1", "D"},
	{"4.4.3.2", "E"},
	{"4.5", "OTHER SPECIAL INTEREST/OCCUPATIONAL GROUPS"},
	{"4.6", "GENDER"},
	{"4.6.1", "Primarily for males"},
	{"4.6.2", "Primarily for females"},
	{"4.6.3", "For males and females"},
	{"4.7", "GEOGRAPHICAL"},
	{"4.7.1", "Universal"},
	{"4.7.2", "Continental"},
	{"4.7.3", "National"},
	{"4.7.4", "Regional"},
	{"4.7.5", "Local"},
	{"4.7.6", "Multinational"},
	{"4.8", "EDUCATION STANDARD"},
	{"4.8.1", "Primary"},
	{"4.8.2", "Secondary"},
	{"4.8.3", "Tertiary"},
	{"4.8.4", "Post Graduate/Life Long Learning"},
	{"4.9", "LIFESTYLE STAGES"},
	{"4.9.1", "Single"},
	{"4.9.2", "Couple"},
	{"4.9.3", "Family with Children 0-3"},
	{"4.9.4", "Family with Children 4-7"},
	{"4.9.5", "Family with Children 8-15"},
	{"4.9.6", "Family with Children 16+"},
	{"4.9.7", "Empty Nester"},
	{"4.9.8", "Retired"},
	{"5", "Educational notes"},
	{"5.1", "Studio"},
	{"5.1.1", "Live"},
	{"5.1.2", "As live"},
	{"5.1.3", "Edited"},
	{"5.10", "Online Distribution"},
	{"5.10.1", "Made on location"},
	{"5.10.1.1", "Live"},
	{"5.10.1.2", "As Live"},
	{"5.10.1.3", "Edited"},
	{"5.10.2", "Made in studio"},
	{"5.10.2.1", "Live"},
	{"5.10.2.2", "As Live"},
	{"5.10.2.3", "Edited"},
	{"5.10.3", "Made on consumer equipment"},
	{"5.10.3.1", "Live"},
	{"5.10.3.2", "As Live"},
	{"5.10.3.3", "Edited"},
	{"5.11", "Offline Distribution"},
	{"5.2", "Made on Location"},
	{"5.2.1", "Live"},
	{"5.2.2", "As live"},
	{"5.2.3", "Edited"},
	{"5.3", "Cinema industry originated"},
	{"5.4", "Made on film (but not originating from the cinema industry)"},
	{"5.5", "Home video"},
	{"5.6", "Multimedia format (I.e. text/computer, etc.)"},
	{"5.7", "Cinema"},
	{"5.7.1", "Made on location"},
	{"5.7.2", "Made in studio"},
	{"5.7.3", "Made by the consumer"},
	{"5.8", "TV"},
	{"5.8.1", "Made on location"},
	{"5.8.1.1", "Live"},
	{"5.8.1.2", "As Live"},
	{"5.8.1.3", "Edited"},
	{"5.8.2", "Made in studio"},
	{"5.8.2.1", "Live"},
	{"5.8.2.2", "As Live"},
	{"5.8.2.3", "Edited"},
	{"5.8.3", "Made by the consumer"},
	{"5.9", "Radio"},
	{"5.9.1", "Made on location"},
	{"5.9.1.1", "Live"},
	{"5.9.1.2", "As Live"},
	{"5.9.1.3", "Edited"},
	{"5.9.2", "Made in studio"},
	{"5.9.2.1", "Live"},
	{"5.9.2.2", "As Live"},
	{"5.9.2.3", "Edited"},
	{"5.9.3", "Made on consumer equipment (home audio)"},
	{"5.9.3.1", "Live"},
	{"5.9.3.2", "As Live"},
	{"5.9.3.3", "Edited"},
	{"6", "Recommendation"},
	{"6.0", "ALERT NOT REQUIRED"},
	{"6.0.1",
	 "No content that requires alerting in any of the categories below"},
	{"6.1", "SEX"},
	{"6.1.1", "No sex descriptors"},
	{"6.1.2", "Obscured or implied sexual activity"},
	{"6.1.3", "Frank portrayal of sex and sexuality"},
	{"6.1.4", "Scenes of explicit sexual behaviour suitable for adults only"},
	{"6.1.4.1",
	 "One scene of explicit sexual behaviour suitable for adults only"},
	{"6.1.4.2",
	 "Occasional scenes of explicit sexual behaviour suitable for adults only"},
	{"6.1.4.3",
	 "Frequent scenes of explicit sexual behaviour suitable for adults only"},
	{"6.1.5", "Sexual Violence"},
	{"6.1.5.1", "One scene of sexual violence"},
	{"6.1.5.2", "Occasional scenes of sexual violence"},
	{"6.1.5.3", "Frequent scenes of sexual Violence"},
	{"6.1.6", "Verbal sexual References"},
	{"6.1.6.1", "One verbal sexual reference"},
	{"6.1.6.2", "Occasional verbal sexual references"},
	{"6.1.6.3", "Frequent verbal sexual references"},
	{"6.10", "STROBING"},
	{"6.10.1", "No strobing"},
	{"6.10.2",
	 "Strobing that could impact on those suffering from Photosensitive epilepsy"},
	{"6.10.2.1",
	 "One scene of strobing that could impact on those suffering from photosensitive epilepsy"},
	{"6.10.2.2",
	 "Occasional strobing that could impact on those suffering from photosensitive epilepsy"},
	{"6.10.2.3",
	 "Frequent strobing that could impact on those suffering from photosensitive epilepsy"},
	{"6.2", "NUDITY"},
	{"6.2.1", "No nudity descriptors"},
	{"6.2.2", "Partial nudity"},
	{"6.2.2.1", "One scene of partial nudity"},
	{"6.2.2.2", "Occasional scenes of partial nudity"},
	{"6.2.2.3", "Frequent scenes of partial nudity"},
	{"6.2.3", "Full frontal nudity"},
	{"6.2.3.1", "One scene of full frontal nudity"},
	{"6.2.3.2", "Occasional scenes of full frontal nudity"},
	{"6.2.3.3", "Frequent scenes of full frontal nudity"},
	{"6.3", "VIOLENCE - HUMAN BEINGS"},
	{"6.3.1", "No violence descriptors human beings"},
	{"6.3.2", "Deliberate infliction of pain to human beings"},
	{"6.3.2.1", "One Scene of deliberate infliction of pain to human beings"},
	{"6.3.2.2", "Occasional deliberate infliction of pain to human beings"},
	{"6.3.2.3", "Frequent deliberate infliction of pain to human beings"},
	{"6.3.3",
	 "Infliction of strong psychological or physical pain to human beings"},
	{"6.3.3.1",
	 "One scene of infliction of strong psychological or physical pain to human beings"},
	{"6.3.3.2",
	 "Occasional scenes of infliction of strong psychological or physical pain to human beings"},
	{"6.3.3.3",
	 "Frequent scenes of infliction of strong psychological or physical pain to human beings"},
	{"6.3.4", "Deliberate killing of human beings"},
	{"6.3.4.1", "One scene of deliberate killing of human beings"},
	{"6.3.4.2", "Occasional deliberate killing of human beings"},
	{"6.3.4.3", "Frequent deliberate killing of human beings"},
	{"6.4", "VIOLENCE - ANIMALS"},
	{"6.4.1", "No violence descriptors animals"},
	{"6.4.2", "Deliberate infliction of pain to animals"},
	{"6.4.2.1", "One scene of deliberate infliction of pain to animals"},
	{"6.4.2.2", "Occasional deliberate infliction of pain to animals"},
	{"6.4.2.3", "Frequent deliberate infliction of pain to animals"},
	{"6.4.3", "Deliberate killing of animals"},
	{"6.4.3.1", "One scene of deliberate killing of animals"},
	{"6.4.3.2", "Occasional deliberate killing of animals"},
	{"6.5", "VIOLENCE - FANTASY CHARACTERS"},
	{"6.5.1", "No violence descriptors"},
	{"6.5.2",
	 "Deliberate infliction of pain to fantasy characters (including animation)"},
	{"6.5.2.1",
	 "One scene of deliberate infliction of pain to fantasy characters (including animation)"},
	{"6.5.2.2",
	 "Occasional deliberate infliction of pain to fantasy characters (including animation)"},
	{"6.5.2.3",
	 "Frequent deliberate infliction of pain to fantasy characters (including animation)"},
	{"6.5.3",
	 "Deliberate killing of fantasy characters (including animation)"},
	{"6.5.3.1",
	 "One scene of deliberate killing of fantasy characters (including animation)"},
	{"6.5.3.2",
	 "Occasional deliberate killing of fantasy characters (including animation)"},
	{"6.5.3.3",
	 "Frequent deliberate killing of fantasy characters (including animation)"},
	{"6.6", "LANGUAGE"},
	{"6.6.1", "No language descriptors"},
	{"6.6.10",
	 "Occasional use of offensive language (racist, homophobic, sexist)"},
	{"6.6.11",
	 "Frequent use of offensive language (racist, homophobic, sexist)"},
	{"6.6.12", "One use of offensive language (racist, homophobic, sexist)"},
	{"6.6.2", "Occasional use of mild swear words and profanities"},
	{"6.6.3", "Frequent use of mild swear words and profanities"},
	{"6.6.4", "Occasional use of very strong language"},
	{"6.6.5", "Frequent use of very strong language"},
	{"6.6.6", "One use of very strong language"},
	{"6.6.7", "Occasional use of strong language"},
	{"6.6.8", "Frequent use of strong language"},
	{"6.6.9", "One use of strong language"},
	{"6.7", "DISTURBING SCENES"},
	{"6.7.1", "No disturbing scenes descriptors"},
	{"6.7.2",
	 "Factual material that may cause distress, including verbal descriptions of traumatic events and the telling of sensitive human interest stories."},
	{"6.7.3",
	 "Mild scenes of blood and gore (including medical procedures, injuries from accidents, terrorists attack, murder, disaster, war)"},
	{"6.7.3.1", "One mild scene of blood and gore"},
	{"6.7.3.2", "Occasional mild scenes of blood and gore"},
	{"6.7.3.3", "Frequent mild scenes of blood and gore"},
	{"6.7.4", "Severe scenes of blood and gore (as 6.7.3 above)"},
	{"6.7.4.1", "One severe scene of blood and gore"},
	{"6.7.4.2",
	 "Occasional severe scenes of blood and gore (as 6.7.3 above)"},
	{"6.7.4.3", "Frequent severe scenes of blood and gore (as 6.7.3 above)"},
	{"6.7.5", "Scenes with extreme horror effects"},
	{"6.7.5.1", "One scene with extreme horror effects"},
	{"6.7.5.2", "Occasional scenes with extreme horror effects"},
	{"6.7.5.3", "Frequent scenes with extreme horror effects"},
	{"6.8", "DISCRIMINATION"},
	{"6.8.1", "No discrimination descriptors"},
	{"6.8.2",
	 "Deliberate discrimination or the portrayal of deliberate discrimination"},
	{"6.9", "ILLEGAL DRUGS"},
	{"6.9.1", "No illegal drugs descriptors"},
	{"6.9.2", "Portrayal of illegal drug use"},
	{"6.9.2.1", "One scene of illegal drug use"},
	{"6.9.2.2", "Occasional portrayal of illegal drug use"},
	{"6.9.2.3", "Frequent portrayal of illegal drug use"},
	{"6.9.3", "Portrayal of illegal drug use with instructive detail"},
	{"6.9.3.1", "One scene of illegal drug use with instructive detail"},
	{"6.9.3.2",
	 "Occasional portrayal of illegal drug use with instructive detail"},
	{"6.9.3.3",
	 "Frequent portrayal of illegal drug use with instructive detail"},
	{"7", "GroupRecommendation"},
	{"7.0", "Proprietary"},
	{"7.1", "Linear"},
	{"7.1.1", "Audio only"},
	{"7.1.2", "Video only"},
	{"7.1.3", "Audio and video"},
	{"7.1.4", "Multimedia"},
	{"7.1.4.1", "Text"},
	{"7.1.4.2", "Graphics"},
	{"7.1.4.3", "Application"},
	{"7.1.5", "Data"},
	{"7.2", "Non Linear"},
	{"7.2.1", "Audio only"},
	{"7.2.2", "Video only"},
	{"7.2.3", "Audio and video"},
	{"7.2.4", "Multimedia"},
	{"7.2.4.1", "Text"},
	{"7.2.4.2", "Graphics"},
	{"7.2.4.3", "Application"},
	{"7.2.5", "Data"},
	{"7.3", "AUDIO VIDEO ENHANCEMENTS"},
	{"7.3.1", "Linear with non-sync"},
	{"7.3.10", "Linear broadcast with online insertions"},
	{"7.3.11", "Other"},
	{"7.3.2", "Linear with sync"},
	{"7.3.3", "Multi stream audio"},
	{"7.3.4", "Multi stream video"},
	{"7.3.5", "Non-linear one stream av show"},
	{"7.3.6", "Non-linear multi stream"},
	{"7.3.7", "Hybrid NVOD"},
	{"7.3.8", "Mix and match"},
	{"7.3.9", "Parallel 'layer controlled' audio or video support"},
	{"8", "Commercial advert"},
	{"8.0", "Proprietary"},
	{"8.1", "Alternative"},
	{"8.10", "Confrontational"},
	{"8.11", "Contemporary"},
	{"8.12", "Crazy"},
	{"8.13", "Cutting edge"},
	{"8.14", "Eclectic"},
	{"8.15", "Edifying"},
	{"8.16", "Exciting"},
	{"8.17", "Fast-moving"},
	{"8.18", "Frantic"},
	{"8.19", "Fun"},
	{"8.2", "Analytical"},
	{"8.20", "Gripping"},
	{"8.21", "Gritty"},
	{"8.22", "Gutsy"},
	{"8.23", "Happy"},
	{"8.24", "Heart-rending"},
	{"8.25", "Heart-warming"},
	{"8.26", "Hot"},
	{"8.27", "Humorous"},
	{"8.28", "Innovative"},
	{"8.29", "Insightful"},
	{"8.3", "Astonishing"},
	{"8.30", "Inspirational"},
	{"8.31", "Intriguing"},
	{"8.32", "Irreverent"},
	{"8.33", "Laid back"},
	{"8.34", "Outrageous"},
	{"8.35", "Peaceful"},
	{"8.36", "Powerful"},
	{"8.37", "Practical"},
	{"8.38", "Rollercoaster"},
	{"8.39", "Romantic"},
	{"8.4", "Ambitious"},
	{"8.40", "Rousing"},
	{"8.41", "Sad"},
	{"8.42", "Satirical"},
	{"8.43", "Serious"},
	{"8.44", "Sexy"},
	{"8.45", "Shocking"},
	{"8.46", "Silly"},
	{"8.47", "Spooky"},
	{"8.48", "Stunning"},
	{"8.49", "Stylish"},
	{"8.5", "Black"},
	{"8.50", "Terrifying"},
	{"8.51", "Thriller"},
	{"8.52", "Violent"},
	{"8.53", "Wacky"},
	{"8.6", "Breathtaking"},
	{"8.7", "Chilling"},
	{"8.8", "Coarse"},
	{"8.9", "Compelling"},
	{"9", "Direct product purchase"},
	{0, 0},
};

	EPG::EPG (CParameter& NParameters):
	                        Parameters(NParameters) {
	for (int i = 0; true; i++) {
	   if (genre_list [i].genre == 0)
			break;
	   genres [genre_list [i].genre] = genre_list [i].desc;
	}

	dir = (Parameters. sDataFilesDirectory + "/epg").c_str();
	if (!QFileInfo (dir).exists())
		QDir().mkdir(dir);
	servicesFilename = dir + "/services.xml";
	//cerr << servicesFilename.toStdString() << endl;
	loadChannels (servicesFilename);
	saveChannels (servicesFilename);
}

EPG&	EPG::operator = (const EPG& e) {
	progs		= e.progs;
	genres		= e.genres;
	servicesFilename = e.servicesFilename;
	basic		= e.basic;
	advanced 	= e.advanced;
	Parameters 	= e.Parameters;
	return *this;
}

void EPG::addChannel (const string& label, uint32_t sid) {
	Parameters. ServiceInformation [sid]. label. insert (label);
	Parameters. ServiceInformation [sid]. id = sid;
}

QDomDocument
	EPG::getFile (const QDate& date, uint32_t sid, bool bAdvanced) {
CDateAndTime d;

	d. year		= date. year ();
	d. month	= date. month ();
	d. day		= date. day ();

	QString fileName (epgFilename (d, sid, 1, bAdvanced).c_str ());
	CEPGDecoder epg;
	QFile file (dir + "/" +fileName);
	if (!file.open (QIODevice::ReadOnly)) {
	   return epg.doc;
	}
	vector < _BYTE > vecData;
	vecData. resize (file. size ());
	vecData. resize (file. size ());
	file. read ((char *) &vecData. front (), file.size ());
	file. close ();
	epg. decode (vecData);
	epg. doc. documentElement ().insertBefore (
	                           epg. doc. createComment (fileName),
	                           epg. doc. documentElement ().firstChild ()
	                         );
	return epg.doc;
}

void	EPG::parseDoc (const QDomDocument &doc) {
QDomNodeList programmes =
	            doc. elementsByTagName ("programme");
	if (programmes. size () == 0)
	   return;
	QDomNode l1 = programmes.item (0);
	while (!l1. isNull ()) {
	   if (l1. nodeName () == "programme") {
	      ProgrammeType p;
	      p. parse (l1. toElement ());
	      time_t start = p. start ();
	      if (min_time <= start && start <= max_time) {
	         QMap<shortCRIDType,ProgrammeType>::
	                       ConstIterator existing = progs. find (start);
	         if (existing != progs. end ()) {
	            p. augment (existing. value ());
	         }
	      }
	      progs [start] = p;
	   }
	   l1 = l1. nextSibling ();
	}
}

/*
<service>
<serviceID id="e1.ce15.c221.0" />
<shortName>Radio 1</shortName>
<mediumName>BBC Radio 1</mediumName>
</service>
*/

void	EPG::saveChannels (const QString &fileName) {
QFile f (fileName);

	if (!f.open (QIODevice::WriteOnly)) {
	   return;
	}
	QDomDocument doc ("serviceInformation");
	QDomElement root = doc.createElement ("serviceInformation");
	doc. appendChild (root);
	QDomElement ensemble = doc.createElement ("ensemble");
	root. appendChild (ensemble);
	for (map < uint32_t,
	     CServiceInformation >::const_iterator i = Parameters.ServiceInformation.begin();
		 i != Parameters.ServiceInformation. end(); i++) {
	   const CServiceInformation& si = i -> second;
	   QDomElement service = doc.createElement ("service");
	   QDomElement serviceID = doc.createElement ("serviceID");
	   serviceID. setAttribute ("id", QString::number (ulong (si.id), 16));
	   service. appendChild (serviceID);
	   for (set<string>::const_iterator j = si.label.begin();
	                             j != si.label.end(); j++) {
	      QDomElement shortName = doc.createElement ("shortName");
	      QDomText text = doc. createTextNode (QString().fromUtf8(j->c_str()));
	      shortName. appendChild (text);
	      service. appendChild (shortName);
	   }
	   ensemble.appendChild (service);
	}
	QTextStream stream (&f);
	stream << doc.toString ();
	f.close ();
}

void	EPG::loadChannels (const QString &fileName) {
QDomDocument domTree;
QFile f (fileName);

	if (!f. open (QIODevice::ReadOnly)) {
	   addChannel ("BBCWorld Service", 0xE1C238);
	   return;
	}

	if (!domTree.setContent (&f)) {
	   f.close ();
	   return;
	}
	f.close ();
	QDomNodeList ensembles = domTree.elementsByTagName ("ensemble");
	QDomNode n = ensembles.item (0).firstChild ();
	while (!n. isNull ()) {
	   if (n.nodeName () == "service") {
	      QDomNode e = n.firstChild ();
	      string name;
	      QString sid;
	      while (!e. isNull ()) {
	         if (e. isElement ()) {
	            QDomElement s = e.toElement ();
	            if (s.tagName () == "shortName")
	               name = s. text (). toUtf8 (). constData ();
	            if (s.tagName () == "serviceID")
	               sid = s.attribute ("id", "0");
	         }
	         e = e. nextSibling ();
	      }
	      if (name != "") {
	         addChannel (name, sid.toUInt (nullptr, 16));
	      }
	   }
	   n = n.nextSibling ();
	}
}

QString EPG::toHTML (void) const {
QString s;
QTextStream r(&s);

	r << "<html><body><table>";
	for (QMap<shortCRIDType,
	          ProgrammeType>::const_iterator i = progs.begin ();
	         i != progs. end (); i++) {
	   const ProgrammeType& p = i.value ();
	   r << "<tr>";
	   time_t t = p. start ();
	   tm bdt = *gmtime(&t);
	   r << "<td>" << bdt.tm_hour << ":" << bdt.tm_min << "</td>";
	   r << "<td>" << p.mediumName[0].text << "</td>";
	   r << "<td>" << p.mediaDescription [0].
	                       shortDescription [0].text << "</td>";
	   r << "<td>" << (p.duration()/60) << ":"
	                             << (p.duration()%60) << "</td>";
	   r << "</tr>";
	}
	r << "</table></body></html>";
	return s;
}

QString EPG::toCSV () const {
QString s;
QTextStream r(&s);

	for (QMap <shortCRIDType,
	           ProgrammeType>::const_iterator i = progs. begin ();
	         i != progs. end (); i++) {
	   const ProgrammeType& p = i. value ();
	   time_t t = p.start ();
	   tm bdt = *gmtime(&t);
	   r << bdt. tm_hour << ":" << bdt. tm_min << "\t";
	   r << p. mediumName [0].text << "\t";
	   r << p.mediaDescription [0]. shortDescription [0]. text << "\t";
	   r << (p.duration()/60) << ":" << (p.duration()%60) << "\t";
	   r << "\n";
	}
	return s;
}

time_t EPGTime::parseTime (const QString& time) {
time_t t=0;

	if(time. length () == 0) 
	   return t; // invalid

	QRegExp q("[-T:+Z]");
	QStringList sl = time. split(q);
#ifdef _WIN32
	SYSTEMTIME st;
	st. wYear = 1970;
	st. wMonth = 1;
	st. wDay = 1;
	st. wHour = 0;
	st. wMinute = 0;
	st. wSecond = 0;
	FILETIME tmp, zero;
	SystemTimeToFileTime (&st, &tmp);
	LocalFileTimeToFileTime (&tmp, &zero);
	ULARGE_INTEGER unix_zero;
	unix_zero. LowPart = zero. dwLowDateTime;
	unix_zero. HighPart = zero. dwHighDateTime;

	st. wYear	= sl[0]. toUInt ();
	st. wMonth	= sl[1]. toUInt ();
	st. wDay	= sl[2]. toUInt ();
	st. wHour	= sl[3]. toUInt ();
	st. wMinute	= sl[4]. toUInt ();
	st. wSecond	= sl[5]. toUInt ();
	FILETIME to;
	SystemTimeToFileTime (&st, &tmp);
	LocalFileTimeToFileTime (&tmp, &to);
	ULARGE_INTEGER ft;
	ft.LowPart	= to. dwLowDateTime;
	ft.HighPart	= to. dwHighDateTime;

	if (sl. count ()==8) {
	   int hh = sl [6]. toInt ();
	   int mm = sl [7]. toInt ();
	   ULARGE_INTEGER offset;
	   offset. QuadPart = 10000000LL * 60LL * (60LL * hh + mm);
	   if (time [19]=='+') // + offset means UTC is earlier
	      ft. QuadPart -= offset. QuadPart;
	   else
	      ft. QuadPart += offset.QuadPart;
	}

	t = time_t ((ft. QuadPart - unix_zero.QuadPart)/10000000LL);
#else
	tm bdt;
	bdt. tm_year	= sl [0]. toUInt () - 1900;
	bdt. tm_mon	= sl [1]. toUInt () - 1;
	bdt. tm_mday	= sl [2]. toUInt ();
	bdt. tm_hour	= sl [3]. toUInt ();
	bdt. tm_min	= sl [4]. toUInt ();
	bdt. tm_sec	= sl [5]. toUInt ();
	t		= mktime (&bdt);
// time zone offset
	if (sl.count () == 8) {
	   int hh	= sl [6]. toInt ();
	   int mm	= sl [7]. toInt ();
	   int secs	= 60 * (60 * hh + mm);
	   if (time [19] == '+') // + offset means UTC is earlier
	      t -= secs;
	   else
	      t += secs;
	}
#endif
	return t;
}

int	EPGTime::parseDuration (const QString & duration) {

	if(duration == "")
	   return 0; // invalid

	if (duration [0] != 'P')
	   return 0; // invalid

	QRegExp r ("PT((\\d+)H)?((\\d+)M)?((\\d+)S)?");
	if (r. indexIn (duration) == -1)
	   return 0;

	QStringList dur = r. capturedTexts ();
	int h = 0, m = 0;
	if (dur [2] != "")
	   h = dur [2]. toInt ();
	if (dur [4] != "")
	   m = dur [4]. toInt ();
	return 60 * h + m;
}

void	EPGTime::parse (QDomElement e) {
	actualTime	= parseTime (e. attribute ("actualTime"));
	actualDuration	= parseDuration (e.attribute ("actualDuration", ""));
	time		= parseTime (e. attribute ("time"));
	duration	= parseDuration (e. attribute ("duration", ""));
}

void	EPGTime::augment (const EPGTime& t) {

	if (t. time != 0)
	   time = t. time;
	if (t. actualTime != 0)
	   actualTime = t. actualTime;
	if (t. duration != 0)
	   duration = t.duration;
	if (t. actualDuration != 0)
	   actualDuration = t.actualDuration;
}

void	EPGRelativeTime::augment (const EPGRelativeTime& t) {

	if (t. time != 0)
	   time = t.time;
	if (t. actualTime != 0)
	   actualTime = t. actualTime;
	if (t. duration != 0)
	   duration = t. duration;
	if (t. actualDuration != 0)
	   actualDuration = t. actualDuration;
}

void	EPGBearer::augment(const EPGBearer& b) {

	if (b. trigger != 0)
	   trigger = b.trigger;
	if (b. id != "")
	   id = b. id;
}

void	LocationType::parse (QDomElement e) {
QDomNode l3 = e. firstChild ();
EPGTime t;

	while (!l3. isNull ()) {
	   if (l3. isElement ()) {
	      QDomElement e = l3. toElement ();
	      if (e.tagName () == "time") {
	         t. parse (e);
	      }
	   }
	   l3 = l3. nextSibling ();
	}
	time. push_back (t);
}

void	LocationType::augment (const LocationType& l) {

	if ((time. size () == 1) && (l. time. size () == 1)) {
	   time [0]. augment (l. time [0]);
	}
	else {
	   for (size_t i = 0; i < l. time. size (); i++)
	      time. push_back (l. time [i]);
	}

	if ((relativeTime. size () == 1) && (l. relativeTime. size ()==1)) {
	   relativeTime [0].augment (l. relativeTime [0]);
	}
	else {
	   for (size_t i = 0; i < l. relativeTime. size (); i++)
	      relativeTime. push_back (l. relativeTime [i]);
	}

	if ((bearer. size () == 1) && (l. bearer. size () == 1)) {
	   bearer [0]. augment (l. bearer [0]);
	}
	else {
	   for (size_t i = 0; i < l.bearer.size (); i++)
	      bearer. push_back (l. bearer [i]);
	}
}

void	MessageType::parse (QDomElement e) {

	lang	= e. attribute ("xml:lang");
	text	= e. text ();
}

void	MediaDescriptionType::parse (QDomElement e) {
QDomNode n = e. firstChild ();

	while (!n. isNull ()) {
	   if (n. isElement ()) {
	      QDomElement e1 = n. toElement ();
	      if (e1. tagName () == "shortDescription") {
	         MessageType m;
	         m. parse (e1);
	         shortDescription. push_back (m);
	      }
	      if (e1.tagName () == "longDescription") {
	         MessageType m;
	         m. parse (e1);
	         longDescription. push_back (m);
	      }
	   }
	   n = n. nextSibling ();
	}
}

void	MediaDescriptionType::augment (const MediaDescriptionType& m) {

	if ((shortDescription. size () == 0)) {
	   shortDescription = m. shortDescription;
	}
	else {
	   for (size_t i = 0; i < m. shortDescription. size (); i++)
	      shortDescription. push_back (m. shortDescription [i]);
	}

	if ((longDescription. size() == 0)) {
	   longDescription = m. longDescription;
	}
	else {
	   for (size_t i = 0; i < m. longDescription. size (); i++)
	      longDescription. push_back (m. longDescription [i]);
	}
}

void	GenreType::parse (QDomElement e) {

	href		= e. attribute ("href", "");
	QString type	= e. attribute ("type", "main");
	if (type == "main")
	   type = main;
	else
	if (type == "secondary")
	   type = secondary;
	else
	if (type == "other")
	   type = other;
}

void	GenreType::augment (const GenreType& g) {

	if (href == "")
	   href = g. href;
	if (name. text == "")
	   name = g.name;
	if (definition. text == "")
	   definition = g.definition;
	if (mimeValue == "")
	   mimeValue = g.mimeValue;
	if (g. preferred)
	   preferred = true;
	if (type == undef)
	   type = g.type;
}

void	ProgrammeType::parse (QDomElement e) {

	shortId		= e. attribute ("shortId", "0"). toInt ();
	QDomNode n	= e. firstChild ();
	while (!n. isNull ()) {
	if (n.isElement ()) {
	   QDomElement e = n. toElement ();
	   if (e. tagName () == "location") {
	      LocationType l;
	      l. parse (e);
	      location. push_back (l);
	   }
	   if (e. tagName () == "shortName") {
	      MessageType m;
	      m. parse (e);
	      shortName. push_back (m);
	   }

	   if (e. tagName () == "mediumName") {
	      MessageType m;
	      m. parse (e);
	      mediumName. push_back (m);
	   }

	   if (e. tagName () == "longName") {
	      MessageType m;
	      m. parse (e);
	      longName. push_back (m);
	   }

	   if (e. tagName () == "mediaDescription") {
	      MediaDescriptionType m;
	      m. parse (e);
	      mediaDescription. push_back (m);
	   }

	   if (e. tagName () == "genre") {
	      GenreType g;
	      g. parse (e);
	      genre. push_back (g);
	   }
	}
	n = n. nextSibling ();
    }
}

time_t	ProgrammeType::start (void) const {
const LocationType& loc	= location [0];
const EPGTime& t	= loc. time [0];
time_t start;

	if (t. actualTime != 0)
	   start = t. actualTime;
	else
	   start = t. time;
	return start;
}

DurationType ProgrammeType::duration (void) const {
const LocationType& loc	= location [0];
const EPGTime& t	= loc. time [0];
	
	if (t. actualTime != 0) {
	   return t. actualDuration;
	}
	else {
	   return t. duration;
	}
}

void	ProgrammeType::augment (const ProgrammeType& p) {

	if ((location. size () == 0)) {
	   location = p. location;
	}
	else {
	   if (p. location. size () > 0)
	      location [0]. augment (p. location [0]);
// TODO
	}

	if (crid == "")
	   crid = p. crid;
	if ((shortName. size () == 0)) {
	   shortName = p. shortName;
	}
	else {
	   for (size_t i = 0; i < p. shortName. size (); i++)
	      shortName. push_back (p. shortName [i]);
// TODO
	}
	if (mediumName. size () == 0) {
	   mediumName = p. mediumName;
	}
	else {
	   for (size_t i = 0; i < p. mediumName. size (); i++)
	      mediumName. push_back (p. mediumName [i]);
	}
	if (longName. size () == 0) {
	   longName = p.longName;
	}
	else {
	   for (size_t i = 0; i < p. longName. size (); i++)
	      longName. push_back (p. longName [i]);
	}
	if (lang == "")
	   lang = p.lang;
	if (version == 0)
	   version = p. version;
	if (p. recommendation == 0)
	   recommendation = true;
	if (p. broadcast == off_air)
	   broadcast = p. broadcast;
	if (bitrate == 0)
	   bitrate = p. bitrate;
	if (CA == "")
	   CA = p.CA;
	if (genre. size () == 0) {
	   genre = p. genre;
	}
	else {
	   if (p. genre. size () >0)
	      genre [0]. augment (p. genre [0]);
// TODO
	}
	if (mediaDescription. size () == 0) {
	   mediaDescription = p. mediaDescription;
	}
	else {
	   if (p. mediaDescription. size () > 0)
	      mediaDescription [0]. augment (p. mediaDescription [0]);
// TODO
	}
	if (keywords. size () == 0) {
	   keywords = p. keywords;
	}
	else {
	   for (size_t i = 0; i < p. keywords. size (); i++)
	      keywords. push_back (p. keywords [i]);
	}
	if (memberOf. size () ==0) {
	   memberOf = p. memberOf;
	}
	else {
	   for (size_t i = 0; i < p. memberOf. size (); i++)
	      memberOf. push_back (p. memberOf [i]);
	}
	if (link. size () == 0) {
	   link = p. link;
	}
	else {
	   for (size_t i = 0; i < p. link. size (); i++)
	      link. push_back (p. link [i]);
	}
	if (programmeEvent. size () == 0) {
	   programmeEvent = p. programmeEvent;
	}
	else {
	   if (p. programmeEvent. size () > 0)
	      programmeEvent [0]. augment (p. programmeEvent [0]);
// TODO
	}
}
