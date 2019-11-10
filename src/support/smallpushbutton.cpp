
#include "smallpushbutton.h"

	smallPushButton::smallPushButton (QWidget *parent):
	                                  QPushButton (parent) {
}

	smallPushButton::~smallPushButton () {}

QSize	smallPushButton::sizeHint ()const {
QSize	temp = QPushButton::sizeHint ();
	return QSize (2 * temp. rwidth () / 3, 2 * temp. rheight () / 3);
}


