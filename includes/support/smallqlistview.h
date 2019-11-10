
#ifndef	__SMALL_QLISTVIEW__
#define	__SMALL_QLISTVIEW__
#include	<QListView>
#include	<QSize>

class	smallQListView : public QListView {
Q_OBJECT
public:
	smallQListView	(QWidget *);
	~smallQListView	();
QSize	sizeHint	()const;
};
#endif

