
#pragma once

#include	<QPushButton>

class	circularButton : public QPushButton {
Q_OBJECT
public:

	explicit circularButton (QWidget *parent = nullptr);
QSize	sizeHint		() const;
void	mousePressEvent		(QMouseEvent *e);
signals:
void	rightClicked		();
public slots:

protected:
	virtual void paintEvent(QPaintEvent *) override;
	virtual void resizeEvent(QResizeEvent *)override;
};
