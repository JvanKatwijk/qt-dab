/********************************************************************************
** Form generated from reading UI file 'scopewidget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCOPEWIDGET_H
#define UI_SCOPEWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_scopeWidget
{
public:
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_12;
    QVBoxLayout *verticalLayout_4;
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
    QPushButton *show_marksButton;
    QLabel *correlationsVector;
    QSpinBox *correlationLength;
    QWidget *null_period;
    QHBoxLayout *horizontalLayout_6;
    QwtPlot *nullDisplay;
    QWidget *tii_scope;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_8;
    QSlider *tiiSlider;
    QwtPlot *tiiDisplay;
    QLabel *tiiLabel;
    QWidget *channel;
    QHBoxLayout *horizontalLayout_4;
    QSlider *channelSlider;
    QwtPlot *channelPlot;
    QWidget *stdDev;
    QHBoxLayout *horizontalLayout_5;
    QSlider *deviationSlider;
    QwtPlot *devPlot;
    QHBoxLayout *horizontalLayout_7;
    QSlider *waterfallSlider;
    QwtPlot *waterfallDisplay;
    QVBoxLayout *verticalLayout_2;
    QwtPlot *iqDisplay;
    QHBoxLayout *horizontalLayout_3;
    QSlider *scopeSlider;
    QCheckBox *ncpScope_checkBox;
    QFormLayout *formLayout;
    QHBoxLayout *horizontalLayout_10;
    QLabel *channelDisplay;
    QLCDNumber *frequencyDisplay;
    QLabel *label_8;
    QLCDNumber *coarse_correctorDisplay;
    QLabel *label;
    QLCDNumber *frequencyOffsetDisplay;
    QLabel *label_5;
    QLCDNumber *snrDisplay;
    QLabel *label_2;
    QLCDNumber *timeOffsetDisplay;
    QLabel *label_3;
    QLCDNumber *clock_errorDisplay;
    QLabel *dcOffset_label;
    QLCDNumber *dcOffset_display;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_11;
    QLabel *syncLabel;
    QProgressBar *ficError_display;
    QLabel *label_6;
    QLCDNumber *ber_display;
    QLabel *label_4;
    QLCDNumber *quality_display;

    void setupUi(QWidget *scopeWidget)
    {
        if (scopeWidget->objectName().isEmpty())
            scopeWidget->setObjectName("scopeWidget");
        scopeWidget->resize(774, 607);
        verticalLayout_5 = new QVBoxLayout(scopeWidget);
        verticalLayout_5->setObjectName("verticalLayout_5");
        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName("verticalLayout_4");
        tabWidget = new QTabWidget(scopeWidget);
        tabWidget->setObjectName("tabWidget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tabWidget->setMaximumSize(QSize(16777215, 301));
        spectr = new QWidget();
        spectr->setObjectName("spectr");
        horizontalLayout = new QHBoxLayout(spectr);
        horizontalLayout->setObjectName("horizontalLayout");
        spectrumSlider = new QSlider(spectr);
        spectrumSlider->setObjectName("spectrumSlider");
        spectrumSlider->setOrientation(Qt::Orientation::Vertical);

        horizontalLayout->addWidget(spectrumSlider);

        spectrumDisplay = new QwtPlot(spectr);
        spectrumDisplay->setObjectName("spectrumDisplay");
        spectrumDisplay->setMinimumSize(QSize(0, 175));

        horizontalLayout->addWidget(spectrumDisplay);

        tabWidget->addTab(spectr, QString());
        correlation = new QWidget();
        correlation->setObjectName("correlation");
        verticalLayout = new QVBoxLayout(correlation);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        correlationSlider = new QSlider(correlation);
        correlationSlider->setObjectName("correlationSlider");
        correlationSlider->setValue(50);
        correlationSlider->setOrientation(Qt::Orientation::Vertical);

        horizontalLayout_9->addWidget(correlationSlider);

        correlationDisplay = new QwtPlot(correlation);
        correlationDisplay->setObjectName("correlationDisplay");

        horizontalLayout_9->addWidget(correlationDisplay);


        verticalLayout->addLayout(horizontalLayout_9);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        show_marksButton = new QPushButton(correlation);
        show_marksButton->setObjectName("show_marksButton");

        horizontalLayout_2->addWidget(show_marksButton);

        correlationsVector = new QLabel(correlation);
        correlationsVector->setObjectName("correlationsVector");

        horizontalLayout_2->addWidget(correlationsVector);

        correlationLength = new QSpinBox(correlation);
        correlationLength->setObjectName("correlationLength");
        correlationLength->setMinimum(400);
        correlationLength->setMaximum(8005);
        correlationLength->setSingleStep(50);
        correlationLength->setValue(600);

        horizontalLayout_2->addWidget(correlationLength);


        verticalLayout->addLayout(horizontalLayout_2);

        tabWidget->addTab(correlation, QString());
        null_period = new QWidget();
        null_period->setObjectName("null_period");
        horizontalLayout_6 = new QHBoxLayout(null_period);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        nullDisplay = new QwtPlot(null_period);
        nullDisplay->setObjectName("nullDisplay");

        horizontalLayout_6->addWidget(nullDisplay);

        tabWidget->addTab(null_period, QString());
        tii_scope = new QWidget();
        tii_scope->setObjectName("tii_scope");
        verticalLayout_3 = new QVBoxLayout(tii_scope);
        verticalLayout_3->setObjectName("verticalLayout_3");
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        tiiSlider = new QSlider(tii_scope);
        tiiSlider->setObjectName("tiiSlider");
        tiiSlider->setValue(50);
        tiiSlider->setOrientation(Qt::Orientation::Vertical);

        horizontalLayout_8->addWidget(tiiSlider);

        tiiDisplay = new QwtPlot(tii_scope);
        tiiDisplay->setObjectName("tiiDisplay");

        horizontalLayout_8->addWidget(tiiDisplay);


        verticalLayout_3->addLayout(horizontalLayout_8);

        tiiLabel = new QLabel(tii_scope);
        tiiLabel->setObjectName("tiiLabel");

        verticalLayout_3->addWidget(tiiLabel);

        tabWidget->addTab(tii_scope, QString());
        channel = new QWidget();
        channel->setObjectName("channel");
        horizontalLayout_4 = new QHBoxLayout(channel);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        channelSlider = new QSlider(channel);
        channelSlider->setObjectName("channelSlider");
        channelSlider->setMaximum(50);
        channelSlider->setValue(20);
        channelSlider->setSliderPosition(20);
        channelSlider->setOrientation(Qt::Orientation::Vertical);

        horizontalLayout_4->addWidget(channelSlider);

        channelPlot = new QwtPlot(channel);
        channelPlot->setObjectName("channelPlot");

        horizontalLayout_4->addWidget(channelPlot);

        tabWidget->addTab(channel, QString());
        stdDev = new QWidget();
        stdDev->setObjectName("stdDev");
        horizontalLayout_5 = new QHBoxLayout(stdDev);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        deviationSlider = new QSlider(stdDev);
        deviationSlider->setObjectName("deviationSlider");
        deviationSlider->setValue(50);
        deviationSlider->setOrientation(Qt::Orientation::Vertical);

        horizontalLayout_5->addWidget(deviationSlider);

        devPlot = new QwtPlot(stdDev);
        devPlot->setObjectName("devPlot");

        horizontalLayout_5->addWidget(devPlot);

        tabWidget->addTab(stdDev, QString());

        verticalLayout_4->addWidget(tabWidget);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        waterfallSlider = new QSlider(scopeWidget);
        waterfallSlider->setObjectName("waterfallSlider");
        waterfallSlider->setMaximum(100);
        waterfallSlider->setValue(50);
        waterfallSlider->setOrientation(Qt::Orientation::Vertical);

        horizontalLayout_7->addWidget(waterfallSlider);

        waterfallDisplay = new QwtPlot(scopeWidget);
        waterfallDisplay->setObjectName("waterfallDisplay");
        waterfallDisplay->setMaximumSize(QSize(448, 232));

        horizontalLayout_7->addWidget(waterfallDisplay);


        verticalLayout_4->addLayout(horizontalLayout_7);


        horizontalLayout_12->addLayout(verticalLayout_4);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        iqDisplay = new QwtPlot(scopeWidget);
        iqDisplay->setObjectName("iqDisplay");
        iqDisplay->setMaximumSize(QSize(261, 211));

        verticalLayout_2->addWidget(iqDisplay);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        scopeSlider = new QSlider(scopeWidget);
        scopeSlider->setObjectName("scopeSlider");
        scopeSlider->setValue(22);
        scopeSlider->setOrientation(Qt::Orientation::Horizontal);

        horizontalLayout_3->addWidget(scopeSlider);

        ncpScope_checkBox = new QCheckBox(scopeWidget);
        ncpScope_checkBox->setObjectName("ncpScope_checkBox");

        horizontalLayout_3->addWidget(ncpScope_checkBox);


        verticalLayout_2->addLayout(horizontalLayout_3);

        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        channelDisplay = new QLabel(scopeWidget);
        channelDisplay->setObjectName("channelDisplay");

        horizontalLayout_10->addWidget(channelDisplay);

        frequencyDisplay = new QLCDNumber(scopeWidget);
        frequencyDisplay->setObjectName("frequencyDisplay");
        frequencyDisplay->setFrameShape(QFrame::Shape::NoFrame);
        frequencyDisplay->setDigitCount(7);
        frequencyDisplay->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        horizontalLayout_10->addWidget(frequencyDisplay);


        formLayout->setLayout(0, QFormLayout::SpanningRole, horizontalLayout_10);

        label_8 = new QLabel(scopeWidget);
        label_8->setObjectName("label_8");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_8);

        coarse_correctorDisplay = new QLCDNumber(scopeWidget);
        coarse_correctorDisplay->setObjectName("coarse_correctorDisplay");
        coarse_correctorDisplay->setFrameShape(QFrame::Shape::NoFrame);
        coarse_correctorDisplay->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        formLayout->setWidget(1, QFormLayout::FieldRole, coarse_correctorDisplay);

        label = new QLabel(scopeWidget);
        label->setObjectName("label");

        formLayout->setWidget(2, QFormLayout::LabelRole, label);

        frequencyOffsetDisplay = new QLCDNumber(scopeWidget);
        frequencyOffsetDisplay->setObjectName("frequencyOffsetDisplay");
        QFont font;
        font.setBold(true);
        frequencyOffsetDisplay->setFont(font);
        frequencyOffsetDisplay->setFrameShape(QFrame::Shape::NoFrame);
        frequencyOffsetDisplay->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        formLayout->setWidget(2, QFormLayout::FieldRole, frequencyOffsetDisplay);

        label_5 = new QLabel(scopeWidget);
        label_5->setObjectName("label_5");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_5);

        snrDisplay = new QLCDNumber(scopeWidget);
        snrDisplay->setObjectName("snrDisplay");
        snrDisplay->setFrameShape(QFrame::Shape::NoFrame);
        snrDisplay->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        formLayout->setWidget(3, QFormLayout::FieldRole, snrDisplay);

        label_2 = new QLabel(scopeWidget);
        label_2->setObjectName("label_2");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_2);

        timeOffsetDisplay = new QLCDNumber(scopeWidget);
        timeOffsetDisplay->setObjectName("timeOffsetDisplay");
        timeOffsetDisplay->setFont(font);
        timeOffsetDisplay->setFrameShape(QFrame::Shape::NoFrame);
        timeOffsetDisplay->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        formLayout->setWidget(4, QFormLayout::FieldRole, timeOffsetDisplay);

        label_3 = new QLabel(scopeWidget);
        label_3->setObjectName("label_3");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_3);

        clock_errorDisplay = new QLCDNumber(scopeWidget);
        clock_errorDisplay->setObjectName("clock_errorDisplay");
        clock_errorDisplay->setFont(font);
        clock_errorDisplay->setFrameShape(QFrame::Shape::NoFrame);
        clock_errorDisplay->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        formLayout->setWidget(5, QFormLayout::FieldRole, clock_errorDisplay);

        dcOffset_label = new QLabel(scopeWidget);
        dcOffset_label->setObjectName("dcOffset_label");

        formLayout->setWidget(6, QFormLayout::LabelRole, dcOffset_label);

        dcOffset_display = new QLCDNumber(scopeWidget);
        dcOffset_display->setObjectName("dcOffset_display");
        dcOffset_display->setFrameShape(QFrame::Shape::NoFrame);
        dcOffset_display->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        formLayout->setWidget(6, QFormLayout::FieldRole, dcOffset_display);


        verticalLayout_2->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        horizontalLayout_12->addLayout(verticalLayout_2);


        verticalLayout_5->addLayout(horizontalLayout_12);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        syncLabel = new QLabel(scopeWidget);
        syncLabel->setObjectName("syncLabel");

        horizontalLayout_11->addWidget(syncLabel);

        ficError_display = new QProgressBar(scopeWidget);
        ficError_display->setObjectName("ficError_display");
        ficError_display->setValue(24);

        horizontalLayout_11->addWidget(ficError_display);

        label_6 = new QLabel(scopeWidget);
        label_6->setObjectName("label_6");

        horizontalLayout_11->addWidget(label_6);

        ber_display = new QLCDNumber(scopeWidget);
        ber_display->setObjectName("ber_display");
        ber_display->setFrameShape(QFrame::Shape::NoFrame);
        ber_display->setFrameShadow(QFrame::Shadow::Raised);
        ber_display->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        horizontalLayout_11->addWidget(ber_display);

        label_4 = new QLabel(scopeWidget);
        label_4->setObjectName("label_4");

        horizontalLayout_11->addWidget(label_4);

        quality_display = new QLCDNumber(scopeWidget);
        quality_display->setObjectName("quality_display");
        quality_display->setFont(font);
        quality_display->setFrameShape(QFrame::Shape::NoFrame);
        quality_display->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        horizontalLayout_11->addWidget(quality_display);


        verticalLayout_5->addLayout(horizontalLayout_11);


        retranslateUi(scopeWidget);

        tabWidget->setCurrentIndex(1);


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
        show_marksButton->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The encoding of the TII data (if any) can be used to get an estimate of the delay of the signal. This estimate (not always very accurate) then can be used  to label the peaks in the correlation scope (these peaks can also be used to compute a delay in the arrival of the signal of a particular transmitter) label the peaks in the correlation scope with the TII numbers.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        show_marksButton->setText(QCoreApplication::translate("scopeWidget", "show marks", nullptr));
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
        channelPlot->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The &quot;channel&quot; has effect on the signal when it travels through the air from transmitter to receiver. The &quot;transformation&quot; made by the channel on the signal can be computed since for the first datablock in a DAB frame, it is known what the result should be. So, in Qt-DAB we take 64 of these values and compare them with the values actually received. Based on that we compute an estimate of the channel impulse, i.e. the result of feeding the channel with a single pulse.</p><p>That result is made visible in this scope.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        tabWidget->setTabText(tabWidget->indexOf(channel), QCoreApplication::translate("scopeWidget", "channel", nullptr));
#if QT_CONFIG(tooltip)
        devPlot->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The &quot;Dev&quot; scope shows the standard deviation of the decoded signal.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        tabWidget->setTabText(tabWidget->indexOf(stdDev), QCoreApplication::translate("scopeWidget", "Dev", nullptr));
#if QT_CONFIG(tooltip)
        waterfallDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The waterfall display djows the progress of the data of the selected scope in time</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        iqDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>IQ constallation of the decoded signal. Ideally one sees 4 dots, one in each quadrant</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        scopeSlider->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Zoom in/out</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        ncpScope_checkBox->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>If the checkbox is set the constallation diagram of the decoded carriers is shown. The default setting is to show just the centerpoints of the &quot;clouds&quot;.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        ncpScope_checkBox->setText(QCoreApplication::translate("scopeWidget", "ncp", nullptr));
#if QT_CONFIG(tooltip)
        channelDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The currently selected channel. Note that for some forms of file input this info does not represent reality.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        channelDisplay->setText(QCoreApplication::translate("scopeWidget", "Channel", nullptr));
#if QT_CONFIG(tooltip)
        frequencyDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Frequency of the currently selected channel.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_8->setText(QCoreApplication::translate("scopeWidget", "correction", nullptr));
#if QT_CONFIG(tooltip)
        coarse_correctorDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Overall frequency correction, consisting of two parts, a fine correction, based on the phase difference between the timedomain samples in the cyclic prefix and the corresponding elements in the ofdm symbol (limited to 500 Hz) and the coarse correction, computed by correlation.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label->setText(QCoreApplication::translate("scopeWidget", "freq error", nullptr));
#if QT_CONFIG(tooltip)
        frequencyOffsetDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Frequeny offset, the computed frequency error that is measured after applying the estimated frequency correction. Compuation is by looking at the phase difference in elements in subsequent DAB frames.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_5->setText(QCoreApplication::translate("scopeWidget", "snr ", nullptr));
#if QT_CONFIG(tooltip)
        snrDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>SNR, i.e. Signal Noise ratio, computed from the difference in signal strength between the datablocks and the nullperiod.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_2->setText(QCoreApplication::translate("scopeWidget", "time offset", nullptr));
#if QT_CONFIG(tooltip)
        timeOffsetDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Sample clock offset. </p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_3->setText(QCoreApplication::translate("scopeWidget", "clock offset", nullptr));
#if QT_CONFIG(tooltip)
        clock_errorDisplay->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>clock offset. The value indicates the number of samples too many or too few for periods of 1 second</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        dcOffset_label->setText(QCoreApplication::translate("scopeWidget", "dc offset", nullptr));
#if QT_CONFIG(tooltip)
        dcOffset_display->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The DC offset in the incoming signal, computed by looking at the average amplitude of both the I and the Q component of the incoming signal.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        syncLabel->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Indicator for the time synchronization, if green, it looks like time syncing is OK, i.e. the software is able to identigy the start of the DAB frames in the incoming sample stream.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        syncLabel->setText(QCoreApplication::translate("scopeWidget", "synced", nullptr));
#if QT_CONFIG(tooltip)
        ficError_display->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>The FIC quality, The FIC data contains structural information on the payload. Usually if the quality is less than 100 percent, decoding the payload will not succeed.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_6->setText(QCoreApplication::translate("scopeWidget", "Ber (fic)", nullptr));
#if QT_CONFIG(tooltip)
        ber_display->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Bit error rate, i.e. the fraction of bits that are erroneous before applying Viterbi, i.e. lower is better.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_4->setText(QCoreApplication::translate("scopeWidget", "MER (dB)", nullptr));
#if QT_CONFIG(tooltip)
        quality_display->setToolTip(QCoreApplication::translate("scopeWidget", "<html><head/><body><p>Quality Indicator according to the Modulation Error as specified in Tr 101 290, higher is better. </p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class scopeWidget: public Ui_scopeWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCOPEWIDGET_H
