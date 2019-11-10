#include "smallcombobox.h"

	smallComboBox::smallComboBox (QWidget *parent):
	                                    QComboBox(parent) {
}

	smallComboBox::~smallComboBox () {}

QSize	smallComboBox::sizeHint ()const {
QSize	temp = QComboBox::sizeHint ();
	return QSize (temp. rwidth () / 4, temp. rheight () / 4);
}


