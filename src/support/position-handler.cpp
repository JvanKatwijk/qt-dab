
#include	"position-handler.h"
#include	<QWidget>

void	set_position_and_size (QSettings * settings,
	                       QWidget *w, const QString &key) {
	settings	-> beginGroup (key);
	int x	= settings	-> value (key + "-x", 100). toInt ();
	int y	= settings	-> value (key + "-y", 100). toInt ();
	int wi	= settings	-> value (key + "-w", 300). toInt ();
	int he	= settings	-> value (key + "-h", 200). toInt ();
	settings	-> endGroup ();
	w 	-> resize (QSize (wi, he));
	w	-> move (QPoint (x, y));
}

void	store_widget_position (QSettings *settings,
	                       QWidget *w, const QString &key) {
	settings	-> beginGroup (key);
	settings	-> setValue (key + "-x", w -> pos (). x ());
	settings	-> setValue (key + "-y", w -> pos (). y ());
	settings	-> setValue (key + "-w", w -> size (). width ());
	settings	-> setValue (key + "-h", w -> size (). height ());
	settings	-> endGroup ();
}

