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

#include	"audio-selector.h"
#include	<QVBoxLayout>
#include	"radio.h"

	audioSelector::audioSelector (RadioInterface *mr):
	                                               QFrame (nullptr) {
	this	-> myRadioInterface	= mr;
	toptext		= new QLabel;
	toptext ->      setText ("detected audiochannels");
        selectorDisplay = new QListView (this);
        QVBoxLayout *layOut = new QVBoxLayout;
        layOut  ->      addWidget (toptext);
        layOut  ->      addWidget (selectorDisplay);
        setWindowTitle ("audio selector");
        setLayout(layOut);
	audioChannels         = QStringList ();
        channelList. setStringList (audioChannels);
        selectorDisplay -> setModel (&channelList);
        connect (selectorDisplay, &QListView::clicked,
                 this, &audioSelector::select_channel);
}

	audioSelector::~audioSelector	() {
}

void	audioSelector::addtoList	(const QString &channel) {
	audioChannels << channel;
	channelList. setStringList (audioChannels);
	selectorDisplay	-> setModel (&channelList);
	selectorDisplay	-> adjustSize ();
	adjustSize ();
}

void	audioSelector::select_channel	(QModelIndex m) {
	emit selected (audioChannels. at (m. row ()), m. row ());
}

int	audioSelector::set_channel	(const QString &channel) {
	for (int i = 0; i < audioChannels. size (); i ++)  {
	   if (audioChannels [i] == channel)
	      return i;
	}
	return -1;
}

void	audioSelector::clear		() {
	audioChannels = QStringList ();
        channelList. setStringList (audioChannels);
        selectorDisplay -> setModel (&channelList);
        selectorDisplay -> adjustSize ();
        adjustSize ();
}
