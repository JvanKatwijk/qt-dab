
#include "circular-button.h"
#include	<QMouseEvent>
#include <QPainter>

		circularButton::circularButton (QWidget *parent):
	                                            QPushButton(parent) {

}

void	circularButton::paintEvent (QPaintEvent *) {
//	Do not paint base implementation -> no styles are applied

	QColor background = isDown() ? QColor("grey") : QColor("lightgrey");
	int diameter = qMin(height(), width());

	QPainter painter (this);
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.translate(width() / 2, height() / 2);

	painter.setPen(QPen(QColor ("black"), 2));
	painter.setBrush (QBrush(background));
	painter.drawEllipse(QRect(-diameter / 2, -diameter / 2, diameter, diameter));
}

void	circularButton::resizeEvent (QResizeEvent *e) {
	QPushButton::resizeEvent (e);
	int diameter = qMin(height(), width()) + 4 ;
	int xOff =(width() -diameter ) / 2;
	int yOff =(height() - diameter) / 2;
	setMask(QRegion(xOff,yOff, diameter, diameter,QRegion::Ellipse));
}

QSize   circularButton::sizeHint ()const {
QSize   temp = QPushButton::sizeHint ();
        return QSize (2 * temp. rwidth () / 3, temp. rheight ());
//      return QSize (2 * temp. rwidth () / 3, 2 * temp. rheight () / 3);
}

void    circularButton::mousePressEvent (QMouseEvent *e) {
        if (e -> button () == Qt::RightButton) {
           emit rightClicked ();
        }
        else
           emit clicked ();
}

