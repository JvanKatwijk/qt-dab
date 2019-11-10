
#ifndef	__SMALL_COMBOBOX__
#define	__SMALL_COMBOBOX__
#include	<QComboBox>
#include	<QSize>

class	smallComboBox : public QComboBox {
Q_OBJECT
public:
	smallComboBox	(QWidget *);
	~smallComboBox	();
QSize	sizeHint	()const;
};
#endif

