
#ifndef	__SMALL_BUTTON__
#define	__SMALL_BUTTON__
#include	<QPushButton>
#include	<QSize>

class	smallPushButton : public QPushButton {
Q_OBJECT
public:
	smallPushButton	(QWidget *);
	~smallPushButton	();
QSize	sizeHint	()const;
};
#endif

