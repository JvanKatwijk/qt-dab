

#ifndef	__SPECTRUM_SCOPE_H
#define	__SPECTRUM_SCOPE_H

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

class	spectrumScope: public QObject {
Q_OBJECT
public:
		spectrumScope	(QwtPlot *,
	                         int,
	                         QSettings *);
		~spectrumScope	();
	void	showSpectrum	(double *, double *, int, int);
	void	setBitDepth	(int);

private:

	QwtPlotCurve	spectrumCurve;
	QSettings	*dabSettings;
	QwtPlotPicker	*lm_picker;
	QColor		displayColor;
	QColor		gridColor;
	QColor		curveColor;

	int		normalizer;
	double		X_axis	[256];
	double		Y_value	[256];
	int16_t		displaySize;
	QwtPlotMarker	*Marker;
	QwtPlot		*plotgrid;
	QwtPlotGrid	*grid;
	QwtPlotGrid	*grid_2;
	int32_t		indexforMarker;
	void		ViewSpectrum		(double *, double *, double, int);
	float		get_db			(float);
	
private slots:
	void		rightMouseClick		(const QPointF &);
};

#endif

