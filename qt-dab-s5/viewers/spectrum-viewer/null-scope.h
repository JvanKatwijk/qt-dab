#ifndef	__NULL_SCOPE_H
#define	__NULL_SCOPE_H

#include        "dab-constants.h"
#include	<QObject>
#include	<qwt.h>
#include	<qwt_plot.h>
#include	<qwt_plot_marker.h>
#include	<qwt_plot_grid.h>
#include	<qwt_plot_curve.h>
#include        <qwt_color_map.h>
#include        <qwt_plot_zoomer.h>
#include        <qwt_plot_textlabel.h>
#include        <qwt_plot_panner.h>
#include        <qwt_plot_layout.h>
#include	<qwt_picker_machine.h>
#include        <qwt_scale_widget.h>
#include        <QBrush>
#include        <QTimer>

class	RadioInterface;
class	QSettings;

class	nullScope: public QObject {
Q_OBJECT
public:
		nullScope	(QwtPlot *,
	                         int,
	                         QSettings *);
		~nullScope	();
	void	show_nullPeriod	(float *, int);

private:

	QwtPlotCurve	spectrumCurve;
	QSettings	*dabSettings;
	QColor		displayColor;
	QColor		gridColor;
	QColor		curveColor;

	QwtPlot		*plotgrid;
	QwtPlotGrid	*grid;
};

#endif

