/********************************************************************************
** Form generated from reading UI file 'scopewidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCOPEWIDGET_H
#define UI_SCOPEWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_scopeWidget
{
public:
    QHBoxLayout *horizontalLayout_10;
    QVBoxLayout *verticalLayout_5;
    QTabWidget *tabWidget;
    QWidget *spectr;
    QHBoxLayout *horizontalLayout;
    QSlider *spectrumSlider;
    QwtPlot *spectrumDisplay;
    QWidget *correlation;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_9;
    QSlider *correlationSlider;
    QwtPlot *correlationDisplay;
    QHBoxLayout *horizontalLayout_2;
    QLabel *correlationsVector;
    QSpinBox *correlationLength;
    QWidget *null_period;
    QwtPlot *nullDisplay;
    QWidget *tii_scope;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_8;
    QSlider *tiiSlider;
    QwtPlot *tiiDisplay;
    QLabel *tiiLabel;
    QWidget *channel;
    QHBoxLayout *horizontalLayout_7;
    QSlider *channelSlider;
    QwtPlot *channelPlot;
    QWidget *stdDev;
    QHBoxLayout *horizontalLayout_5;
    QSlider *deviationSlider;
    QwtPlot *devPlot;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_4;
    QSlider *waterfallSlider;
    QwtPlot *waterfallDisplay;
    QHBoxLayout *horizontalLayout_6;
    QLabel *syncLabel;
    QProgressBar *ficError_display;
    QVBoxLayout *verticalLayout_6;
    QVBoxLayout *verticalLayout_2;
    QwtPlot *iqDisplay;
    QSlider *scopeSlider;
    QHBoxLayout *horizontalLayout_3;
    QLCDNumber *frequencyDisplay;
    QLabel *label_7;
    QGridLayout *gridLayout;
    QLabel *label_6;
    QLCDNumber *coarse_correctorDisplay;
    QLabel *label_8;
    QLCDNumber *fine_correctorDisplay;
    QLabel *label;
    QLCDNumber *frequencyOffsetDisplay;
    QLabel *label_5;
    QLCDNumber *snrDisplay;
    QLabel *label_2;
    QLCDNumber *timeOffsetDisplay;
    QLabel *label_3;
    QLCDNumber *clock_errorDisplay;
    QLabel *label_4;
    QLCDNumber *quality_display;

    void setupUi(QWidget *scopeWidget)
    {
        if (scopeWidget->objectName().isEmpty())
            scopeWidget->setObjectName(QString::fromUtf8("scopeWidget"));
        scopeWidget->resize(700, 449);
        horizontalLayout_10 = new QHBoxLayout(scopeWidget);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        tabWidget = new QTabWidget(scopeWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setMaximumSize(QSize(421, 231));
        spectr = new QWidget();
        spectr->setObjectName(QString::fromUtf8("spectr"));
        horizontalLayout = new QHBoxLayout(spectr);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        spectrumSlider = new QSlider(spectr);
        spectrumSlider->setObjectName(QString::fromUtf8("spectrumSlider"));
        spectrumSlider->setOrientation(Qt::Vertical);

        horizontalLayout->addWidget(spectrumSlider);

        spectrumDisplay = new QwtPlot(spectr);
        spectrumDisplay->setObjectName(QString::fromUtf8("spectrumDisplay"));
        spectrumDisplay->setMinimumSize(QSize(0, 175));

        horizontalLayout->addWidget(spectrumDisplay);

        tabWidget->addTab(spectr, QString());
        correlation = new QWidget();
        correlation->setObjectName(QString::fromUtf8("correlation"));
        verticalLayout = new QVBoxLayout(correlation);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        correlationSlider = new QSlider(correlation);
        correlationSlider->setObjectName(QString::fromUtf8("correlationSlider"));
        correlationSlider->setValue(50);
        correlationSlider->setOrientation(Qt::Vertical);

        horizontalLayout_9->addWidget(correlationSlider);

        correlationDisplay = new QwtPlot(correlation);
        correlationDisplay->setObjectName(QString::fromUtf8("correlationDisplay"));
        correlationDisplay->setMinimumSize(QSize(0, 141));
        correlationDisplay->setMaximumSize(QSize(16777215, 150));

        horizontalLayout_9->addWidget(correlationDisplay);


        verticalLayout->addLayout(horizontalLayout_9);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        correlationsVector = new QLabel(correlation);
        correlationsVector->setObjectName(QString::fromUtf8("correlationsVector"));

        horizontalLayout_2->addWidget(correlationsVector);

        correlationLength = new QSpinBox(correlation);
        correlationLength->setObjectName(QString::fromUtf8("correlationLength"));
        correlationLength->setMinimum(400);
        correlationLength->setMaximum(8005);
        correlationLength->setSingleStep(50);
        correlationLength->setValue(600);

        horizontalLayout_2->addWidget(correlationLength);


        verticalLayout->addLayout(horizontalLayout_2);

        tabWidget->addTab(correlation, QString());
        null_period = new QWidget();
        null_period->setObjectName(QString::fromUtf8("null_period"));
        nullDisplay = new QwtPlot(null_period);
        nullDisplay->setObjectName(QString::fromUtf8("nullDisplay"));
        nullDisplay->setGeometry(QRect(0, 10, 481, 171));
        nullDisplay->setMaximumSize(QSize(16777215, 171));
        tabWidget->addTab(null_period, QString());
        tii_scope = new QWidget();
        tii_scope->setObjectName(QString::fromUtf8("tii_scope"));
        verticalLayout_3 = new QVBoxLayout(tii_scope);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        tiiSlider = new QSlider(tii_scope);
        tiiSlider->setObjectName(QString::fromUtf8("tiiSlider"));
        tiiSlider->setValue(50);
        tiiSlider->setOrientation(Qt::Vertical);

        horizontalLayout_8->addWidget(tiiSlider);

        tiiDisplay = new QwtPlot(tii_scope);
        tiiDisplay->setObjectName(QString::fromUtf8("tiiDisplay"));
        tiiDisplay->setMaximumSize(QSize(401, 140));

        horizontalLayout_8->addWidget(tiiDisplay);


        verticalLayout_3->addLayout(horizontalLayout_8);

        tiiLabel = new QLabel(tii_scope);
        tiiLabel->setObjectName(QString::fromUtf8("tiiLabel"));

        verticalLayout_3->addWidget(tiiLabel);

        tabWidget->addTab(tii_scope, QString());
        channel = new QWidget();
        channel->setObjectName(QString::fromUtf8("channel"));
        horizontalLayout_7 = new QHBoxLayout(channel);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        channelSlider = new QSlider(channel);
        channelSlider->setObjectName(QString::fromUtf8("channelSlider"));
        channelSlider->setValue(50);
        channelSlider->setOrientation(Qt::Vertical);

        horizontalLayout_7->addWidget(channelSlider);

        channelPlot = new QwtPlot(channel);
        channelPlot->setObjectName(QString::fromUtf8("channelPlot"));

        horizontalLayout_7->addWidget(channelPlot);

        tabWidget->addTab(channel, QString());
        stdDev = new QWidget();
        stdDev->setObjectName(QString::fromUtf8("stdDev"));
        horizontalLayout_5 = new QHBoxLayout(stdDev);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        deviationSlider = new QSlider(stdDev);
        deviationSlider->setObjectName(QString::fromUtf8("deviationSlider"));
        deviationSlider->setValue(50);
        deviationSlider->setOrientation(Qt::Vertical);

        horizontalLayout_5->addWidget(deviationSlider);

        devPlot = new QwtPlot(stdDev);
        devPlot->setObjectName(QString::fromUtf8("devPlot"));

        horizontalLayout_5->addWidget(devPlot);

        tabWidget->addTab(stdDev, QString());

        verticalLayout_5->addWidget(tabWidget);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        waterfallSlider = new QSlider(scopeWidget);
        waterfallSlider->setObjectName(QString::fromUtf8("waterfallSlider"));
        waterfallSlider->setMaximum(100);
        waterfallSlider->setValue(50);
        waterfallSlider->setOrientation(Qt::Vertical);

        horizontalLayout_4->addWidget(waterfallSlider);

        waterfallDisplay = new QwtPlot(scopeWidget);
        waterfallDisplay->setObjectName(QString::fromUtf8("waterfallDisplay"));
        waterfallDisplay->setMaximumSize(QSize(400, 159));

        horizontalLayout_4->addWidget(waterfallDisplay);


        verticalLayout_4->addLayout(horizontalLayout_4);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        syncLabel = new QLabel(scopeWidget);
        syncLabel->setObjectName(QString::fromUtf8("syncLabel"));

        horizontalLayout_6->addWidget(syncLabel);

        ficError_display = new QProgressBar(scopeWidget);
        ficError_display->setObjectName(QString::fromUtf8("ficError_display"));
        ficError_display->setValue(24);

        horizontalLayout_6->addWidget(ficError_display);


        verticalLayout_4->addLayout(horizontalLayout_6);


        verticalLayout_5->addLayout(verticalLayout_4);


        horizontalLayout_10->addLayout(verticalLayout_5);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        iqDisplay = new QwtPlot(scopeWidget);
        iqDisplay->setObjectName(QString::fromUtf8("iqDisplay"));
        iqDisplay->setMaximumSize(QSize(219, 138));

        verticalLayout_2->addWidget(iqDisplay);

        scopeSlider = new QSlider(scopeWidget);
        scopeSlider->setObjectName(QString::fromUtf8("scopeSlider"));
        scopeSlider->setValue(22);
        scopeSlider->setOrientation(Qt::Horizontal);

        verticalLayout_2->addWidget(scopeSlider);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        frequencyDisplay = new QLCDNumber(scopeWidget);
        frequencyDisplay->setObjectName(QString::fromUtf8("frequencyDisplay"));
        frequencyDisplay->setFrameShape(QFrame::NoFrame);
        frequencyDisplay->setFrameShadow(QFrame::Plain);
        frequencyDisplay->setDigitCount(7);

        horizontalLayout_3->addWidget(frequencyDisplay);

        label_7 = new QLabel(scopeWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_3->addWidget(label_7);


        verticalLayout_2->addLayout(horizontalLayout_3);


        verticalLayout_6->addLayout(verticalLayout_2);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_6 = new QLabel(scopeWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 0, 0, 1, 1);

        coarse_correctorDisplay = new QLCDNumber(scopeWidget);
        coarse_correctorDisplay->setObjectName(QString::fromUtf8("coarse_correctorDisplay"));
        coarse_correctorDisplay->setFrameShape(QFrame::NoFrame);
        coarse_correctorDisplay->setSegmentStyle(QLCDNumber::Flat);

        gridLayout->addWidget(coarse_correctorDisplay, 0, 1, 1, 1);

        label_8 = new QLabel(scopeWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout->addWidget(label_8, 1, 0, 1, 1);

        fine_correctorDisplay = new QLCDNumber(scopeWidget);
        fine_correctorDisplay->setObjectName(QString::fromUtf8("fine_correctorDisplay"));
        fine_correctorDisplay->setFrameShape(QFrame::NoFrame);
        fine_correctorDisplay->setSegmentStyle(QLCDNumber::Flat);

        gridLayout->addWidget(fine_correctorDisplay, 1, 1, 1, 1);

        label = new QLabel(scopeWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 2, 0, 1, 1);

        frequencyOffsetDisplay = new QLCDNumber(scopeWidget);
        frequencyOffsetDisplay->setObjectName(QString::fromUtf8("frequencyOffsetDisplay"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        frequencyOffsetDisplay->setFont(font);
        frequencyOffsetDisplay->setFrameShape(QFrame::NoFrame);
        frequencyOffsetDisplay->setSegmentStyle(QLCDNumber::Flat);

        gridLayout->addWidget(frequencyOffsetDisplay, 2, 1, 1, 1);

        label_5 = new QLabel(scopeWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 3, 0, 1, 1);

        snrDisplay = new QLCDNumber(scopeWidget);
        snrDisplay->setObjectName(QString::fromUtf8("snrDisplay"));
        snrDisplay->setFrameShape(QFrame::NoFrame);
        snrDisplay->setSegmentStyle(QLCDNumber::Flat);

        gridLayout->addWidget(snrDisplay, 3, 1, 1, 1);

        label_2 = new QLabel(scopeWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 4, 0, 1, 1);

        timeOffsetDisplay = new QLCDNumber(scopeWidget);
        timeOffsetDisplay->setObjectName(QString::fromUtf8("timeOffsetDisplay"));
        timeOffsetDisplay->setFont(font);
        timeOffsetDisplay->setFrameShape(QFrame::NoFrame);
        timeOffsetDisplay->setSegmentStyle(QLCDNumber::Flat);

        gridLayout->addWidget(timeOffsetDisplay, 4, 1, 1, 1);

        label_3 = new QLabel(scopeWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 5, 0, 1, 1);

        clock_errorDisplay = new QLCDNumber(scopeWidget);
        clock_errorDisplay->setObjectName(QString::fromUtf8("clock_errorDisplay"));
        clock_errorDisplay->setFont(font);
        clock_errorDisplay->setFrameShape(QFrame::NoFrame);
        clock_errorDisplay->setSegmentStyle(QLCDNumber::Flat);

        gridLayout->addWidget(clock_errorDisplay, 5, 1, 1, 1);

        label_4 = new QLabel(scopeWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 6, 0, 1, 1);

        quality_display = new QLCDNumber(scopeWidget);
        quality_display->setObjectName(QString::fromUtf8("quality_display"));
        quality_display->setFont(font);
        quality_display->setFrameShape(QFrame::NoFrame);
        quality_display->setSegmentStyle(QLCDNumber::Flat);

        gridLayout->addWidget(quality_display, 6, 1, 1, 1);


        verticalLayout_6->addLayout(gridLayout);


        horizontalLayout_10->addLayout(verticalLayout_6);


        retranslateUi(scopeWidget);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(scopeWidget);
    } // setupUi

    void retranslateUi(QWidget *scopeWidget)
    {
        scopeWidget->setWindowTitle(QCoreApplication::translate("scopeWidget", "spectrumscope", nullptr));
#if QT_CONFIG(tooltip)
        tabWidget->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The tabwidget here contains the tabs for the different scopes, supported by this Qt-DAB implementation</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        spectrumDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The spectrum of the incoming signal.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        tabWidget->setTabText(tabWidget->indexOf(spectr), QCoreApplication::translate("scopeWidget", "Spectrum", nullptr));
#if QT_CONFIG(tooltip)
        correlationDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Fine time synchronization is by correlation. Ideally the match is on or around the 504-th sample.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        correlationsVector->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The result of matching is shown here. The best match is usually with a sample on or around</p><p>sample number 504. The other matches - if any - are shown as two numbers, the offset in the input and the additional distance - compared to the distance of the best match - in km.</p><p>Note that it is NOT the distance between transmitter and rfeceiver.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        correlationsVector->setText(QCoreApplication::translate("scopeWidget", "TextLabel", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(correlation), QCoreApplication::translate("scopeWidget", "Corr", nullptr));
#if QT_CONFIG(tooltip)
        nullDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The samples from the end of the NULL period and the start of the first data block of the DAB frame</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        tabWidget->setTabText(tabWidget->indexOf(null_period), QCoreApplication::translate("scopeWidget", "NULL", nullptr));
#if QT_CONFIG(tooltip)
        tiiDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The TII is encoded in the NULL period of a DAB frame. The scope shows the spectrum, the small vertical  hair-like projections are the &quot;bins&quot;. Their position in the spectrum and their mutial ditances define the TII value. With a database the TII can be mapped onto a transmittername and location.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        tiiLabel->setText(QCoreApplication::translate("scopeWidget", "TextLabel", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tii_scope), QCoreApplication::translate("scopeWidget", "TII", nullptr));
#if QT_CONFIG(tooltip)
        channelPlot->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The &quot;channel&quot;. To investigate the effects of the transmission, we take in a segment of 400 carriers in the first data segment (i.e. the one with the predefined data) centered around 0, each 6-th carrier and - since we know the values they should have, consider them as a &quot;pilot&quot; value and apply a well known algorithm to compute the difference with the measured values.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        tabWidget->setTabText(tabWidget->indexOf(channel), QCoreApplication::translate("scopeWidget", "channel", nullptr));
#if QT_CONFIG(tooltip)
        devPlot->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The &quot;Dev&quot; scope shows the standard deviation of the decoded signal.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        tabWidget->setTabText(tabWidget->indexOf(stdDev), QCoreApplication::translate("scopeWidget", "Dev", nullptr));
#if QT_CONFIG(tooltip)
        waterfallDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The waterfall display djows the progress of the data of the selected scope in time</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        syncLabel->setText(QCoreApplication::translate("scopeWidget", "synced", nullptr));
#if QT_CONFIG(tooltip)
        ficError_display->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The FIC quality, The FIC data contains structural information on the payload. Usually if the quality is less than 100 percent, decoding the payload will not succeed.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        iqDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>IQ constallation of the decoded signal. Ideally one sees 4 dots, one in each quadrant</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        scopeSlider->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Zoom in/out</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        frequencyDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The frequency of the selected channel.</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_7->setText(QCoreApplication::translate("scopeWidget", "MHz", nullptr));
        label_6->setText(QCoreApplication::translate("scopeWidget", "coarse corr.", nullptr));
#if QT_CONFIG(tooltip)
        coarse_correctorDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Coarse frequency correction, The software computed an estimate of the frequency offset by lokking at phasedifferences in the samples in the time domain. Based on the difference found a correction factor is applied to the input stream.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_8->setText(QCoreApplication::translate("scopeWidget", "fine corr", nullptr));
#if QT_CONFIG(tooltip)
        fine_correctorDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Fine frequency correction, computed from the phase difference between the cyclic prefix and the corresponding elements in the ofdm symbol.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label->setText(QCoreApplication::translate("scopeWidget", "freq error", nullptr));
#if QT_CONFIG(tooltip)
        frequencyOffsetDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Frequeny offset, the computed frequency error, after applying the estimated frequency correction.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_5->setText(QCoreApplication::translate("scopeWidget", "snr ", nullptr));
#if QT_CONFIG(tooltip)
        snrDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>SNR, i.e. Signal Noise ratio, compoted from the difference in signal strength between the datablocks and the nullperiod.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_2->setText(QCoreApplication::translate("scopeWidget", "time offset", nullptr));
#if QT_CONFIG(tooltip)
        timeOffsetDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Sample clock offset. </p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_3->setText(QCoreApplication::translate("scopeWidget", "clock offset", nullptr));
#if QT_CONFIG(tooltip)
        clock_errorDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>clock offset. The value indicates the number of samples too many or too few for periods of 1 second</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_4->setText(QCoreApplication::translate("scopeWidget", "signal quality", nullptr));
#if QT_CONFIG(tooltip)
        quality_display->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Quality Indicator according to the Modulation Error as specified in Tr 101 290, higher is better. The values are scaled to a range 1 .. 100</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class scopeWidget: public Ui_scopeWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCOPEWIDGET_H
