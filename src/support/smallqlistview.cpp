
#include "smallqlistview.h"

	smallQListView::smallQListView (QWidget *parent):
	                                    QListView(parent) {
}

	smallQListView::~smallQListView () {}

QSize	smallQListView::sizeHint () const {
QSize	temp = QListView::sizeHint ();
	return QSize (temp. rwidth () / 4, temp. rheight ());
}


