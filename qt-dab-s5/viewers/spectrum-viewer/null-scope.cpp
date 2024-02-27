#include	"null-scope.h"
#include	<QSettings>
#include        <QColor>
#include        <QPen>


	nullScope::nullScope (QwtPlot *nullScope,
	                              int displaySize,
	                              QSettings	*dabSettings):
	                                  spectrumCurve ("") {
QString	colorString	= "black";
//bool	brush;

	(void)displaySize;
	this	-> dabSettings		= dabSettings;
	dabSettings	-> beginGroup ("spectrumViewer");
	colorString	= dabSettings -> value ("displayColor",
	                                           "black"). toString();
	displayColor	= QColor (colorString);
	colorString	= dabSettings -> value ("gridColor",
	                                           "white"). toString();
	gridColor	= QColor (colorString);
	colorString	= dabSettings -> value ("curveColor",
	                                            "white"). toString();
	curveColor	= QColor (colorString);
//	brush		= dabSettings -> value ("brush", 0). toInt () == 1;
	dabSettings	-> endGroup ();
	plotgrid		= nullScope;
	plotgrid		-> setCanvasBackground (displayColor);
	grid			= new QwtPlotGrid;
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid	-> setMajPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid	-> setMajorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid	-> enableXMin (true);
	grid	-> enableYMin (true);
#if defined QWT_VERSION && ((QWT_VERSION >> 8) < 0x0601)
	grid	-> setMinPen (QPen(gridColor, 0, Qt::DotLine));
#else
	grid	-> setMinorPen (QPen(gridColor, 0, Qt::DotLine));
#endif
	grid	-> attach (plotgrid);

	spectrumCurve. setPen (QPen(curveColor, 2.0));
	spectrumCurve. setOrientation (Qt::Horizontal);
	spectrumCurve. setBaseline	(0);
	spectrumCurve. attach (plotgrid);
}

	nullScope::~nullScope	() {
	delete		grid;
}

void	nullScope::show_nullPeriod	(float	*V,
	                                 int	amount) {
float	max	= 0;
double X_axis [amount];
double Y_values [amount];
	
	for (int i = 0; i < amount; i ++) {
	   X_axis [i] = i;
	   if (V [i] > max)
	      max = V [i];
	}

	for (int i = 0; i < amount; i ++)
	   Y_values [i] = V [i] / max * 1.5;
	   
	plotgrid	-> setAxisScale (QwtPlot::xBottom,
				         (double)X_axis [0],
				         X_axis [amount - 1]);
	plotgrid	-> enableAxis (QwtPlot::xBottom);
	plotgrid	-> setAxisScale (QwtPlot::yLeft,
				         0, 1.5 );
	spectrumCurve. setBaseline (0);
	Y_values [0]		= 0;
	Y_values [amount - 1]	= 0;

	spectrumCurve. setSamples (X_axis, Y_values, amount);
	plotgrid	-> replot (); 
}

