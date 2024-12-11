
#include	<QSettings>
#include	"settings-handler.h"


void	store (QSettings *s, QString paragraph, QString key, QString v) {
	s	-> beginGroup (paragraph);
	s	-> setValue (key, v);
	s	-> endGroup ();
}

void	store (QSettings *s, QString paragraph, QString key, int value) {
	s	-> beginGroup (paragraph);
	s	-> setValue (key, value);
	s	-> endGroup ();
}

int	value_i (QSettings *s, QString paragraph, QString key, int def) {
int	res;
	s	-> beginGroup (paragraph);
	res	= s  ->  value (key, def). toInt ();
	s	-> endGroup ();
	return res;
}

float	value_f (QSettings *s, QString paragraph, QString key, float def) {
float     res;
        s       -> beginGroup (paragraph);
        res     = s -> value (key, def). toFloat ();
        s       -> endGroup ();
        return res;
}

QString	value_s (QSettings *s, QString paragraph, QString key, QString def) {
QString	res;
	s	-> beginGroup (paragraph);
	res	= s -> value (key, def). toString ();
	s	-> endGroup ();
	return res;
}

void	remove (QSettings *s, QString paragraph, QString key) {
	s	-> beginGroup (paragraph);
	s	-> remove (key);
	s	-> endGroup ();
}

