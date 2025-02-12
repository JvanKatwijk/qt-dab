/********************************************************************************
** Form generated from reading UI file 'dabradio-6.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DABRADIO_2D_6_H
#define UI_DABRADIO_2D_6_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <newpushbutton.h>
#include "clickable-label.h"
#include "qwt_thermo.h"
#include "smallcombobox.h"
#include "smallpushbutton.h"

QT_BEGIN_NAMESPACE

class Ui_dabradio
{
public:
    QHBoxLayout *horizontalLayout_14;
    QVBoxLayout *verticalLayout_3;
    QWidget *leftPane;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    clickablelabel *ensembleId;
    QLabel *transmitter_country;
    QScrollArea *ensembleWidget;
    QWidget *scrollAreaWidgetContents;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout_2;
    smallPushButton *prevServiceButton;
    smallPushButton *nextServiceButton;
    QHBoxLayout *horizontalLayout_8;
    clickablelabel *prevChannelButton;
    smallComboBox *channelSelector;
    clickablelabel *nextChannelButton;
    QHBoxLayout *horizontalLayout_7;
    newPushButton *scanListButton;
    newPushButton *presetButton;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_6;
    clickablelabel *folder_shower;
    QLCDNumber *stillMuting;
    QLabel *localTimeDisplay;
    QLabel *runtimeDisplay;
    clickablelabel *aboutLabel;
    clickablelabel *snrLabel;
    QHBoxLayout *horizontalLayout_3;
    clickablelabel *serviceButton;
    clickablelabel *serviceLabel;
    QLabel *programTypeLabel;
    QHBoxLayout *horizontalLayout_9;
    QLabel *motLabel;
    QLabel *stereoLabel;
    QLabel *rateLabel;
    QLabel *audiorateLabel;
    QLabel *psLabel;
    QLabel *sbrLabel;
    clickablelabel *soundLabel;
    QHBoxLayout *horizontalLayout_11;
    QLabel *pictureLabel;
    QSlider *volumeSlider;
    QHBoxLayout *horizontalLayout_12;
    QLabel *dynamicLabel;
    QHBoxLayout *horizontalLayout_10;
    smallPushButton *configButton;
    smallPushButton *spectrumButton;
    smallPushButton *httpButton;
    smallPushButton *scanButton;
    QHBoxLayout *horizontalLayout_5;
    QwtThermo *leftAudio;
    QwtThermo *rightAudio;
    QHBoxLayout *horizontalLayout_13;
    QLabel *distanceLabel;
    QSpacerItem *horizontalSpacer;
    smallPushButton *tiiButton;

    void setupUi(QWidget *dabradio)
    {
        if (dabradio->objectName().isEmpty())
            dabradio->setObjectName("dabradio");
        dabradio->resize(612, 329);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dabradio->sizePolicy().hasHeightForWidth());
        dabradio->setSizePolicy(sizePolicy);
        horizontalLayout_14 = new QHBoxLayout(dabradio);
        horizontalLayout_14->setObjectName("horizontalLayout_14");
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        leftPane = new QWidget(dabradio);
        leftPane->setObjectName("leftPane");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(3);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(leftPane->sizePolicy().hasHeightForWidth());
        leftPane->setSizePolicy(sizePolicy1);
        verticalLayout_2 = new QVBoxLayout(leftPane);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        ensembleId = new clickablelabel(leftPane);
        ensembleId->setObjectName("ensembleId");

        horizontalLayout->addWidget(ensembleId);

        transmitter_country = new QLabel(leftPane);
        transmitter_country->setObjectName("transmitter_country");

        horizontalLayout->addWidget(transmitter_country);


        verticalLayout_2->addLayout(horizontalLayout);

        ensembleWidget = new QScrollArea(leftPane);
        ensembleWidget->setObjectName("ensembleWidget");
        ensembleWidget->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 224, 153));
        ensembleWidget->setWidget(scrollAreaWidgetContents);

        verticalLayout_2->addWidget(ensembleWidget);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");

        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        prevServiceButton = new smallPushButton(leftPane);
        prevServiceButton->setObjectName("prevServiceButton");

        horizontalLayout_2->addWidget(prevServiceButton);

        nextServiceButton = new smallPushButton(leftPane);
        nextServiceButton->setObjectName("nextServiceButton");

        horizontalLayout_2->addWidget(nextServiceButton);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        prevChannelButton = new clickablelabel(leftPane);
        prevChannelButton->setObjectName("prevChannelButton");

        horizontalLayout_8->addWidget(prevChannelButton);

        channelSelector = new smallComboBox(leftPane);
        channelSelector->setObjectName("channelSelector");

        horizontalLayout_8->addWidget(channelSelector);

        nextChannelButton = new clickablelabel(leftPane);
        nextChannelButton->setObjectName("nextChannelButton");

        horizontalLayout_8->addWidget(nextChannelButton);


        verticalLayout_2->addLayout(horizontalLayout_8);


        verticalLayout_3->addWidget(leftPane);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        scanListButton = new newPushButton(dabradio);
        scanListButton->setObjectName("scanListButton");

        horizontalLayout_7->addWidget(scanListButton);

        presetButton = new newPushButton(dabradio);
        presetButton->setObjectName("presetButton");

        horizontalLayout_7->addWidget(presetButton);


        verticalLayout_3->addLayout(horizontalLayout_7);


        horizontalLayout_14->addLayout(verticalLayout_3);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        folder_shower = new clickablelabel(dabradio);
        folder_shower->setObjectName("folder_shower");

        horizontalLayout_6->addWidget(folder_shower);

        stillMuting = new QLCDNumber(dabradio);
        stillMuting->setObjectName("stillMuting");

        horizontalLayout_6->addWidget(stillMuting);

        localTimeDisplay = new QLabel(dabradio);
        localTimeDisplay->setObjectName("localTimeDisplay");

        horizontalLayout_6->addWidget(localTimeDisplay);

        runtimeDisplay = new QLabel(dabradio);
        runtimeDisplay->setObjectName("runtimeDisplay");

        horizontalLayout_6->addWidget(runtimeDisplay);

        aboutLabel = new clickablelabel(dabradio);
        aboutLabel->setObjectName("aboutLabel");

        horizontalLayout_6->addWidget(aboutLabel);

        snrLabel = new clickablelabel(dabradio);
        snrLabel->setObjectName("snrLabel");

        horizontalLayout_6->addWidget(snrLabel);


        verticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        serviceButton = new clickablelabel(dabradio);
        serviceButton->setObjectName("serviceButton");
        serviceButton->setMaximumSize(QSize(51, 16777215));

        horizontalLayout_3->addWidget(serviceButton);

        serviceLabel = new clickablelabel(dabradio);
        serviceLabel->setObjectName("serviceLabel");

        horizontalLayout_3->addWidget(serviceLabel);

        programTypeLabel = new QLabel(dabradio);
        programTypeLabel->setObjectName("programTypeLabel");
        programTypeLabel->setMaximumSize(QSize(91, 16777215));

        horizontalLayout_3->addWidget(programTypeLabel);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        motLabel = new QLabel(dabradio);
        motLabel->setObjectName("motLabel");

        horizontalLayout_9->addWidget(motLabel);

        stereoLabel = new QLabel(dabradio);
        stereoLabel->setObjectName("stereoLabel");

        horizontalLayout_9->addWidget(stereoLabel);

        rateLabel = new QLabel(dabradio);
        rateLabel->setObjectName("rateLabel");

        horizontalLayout_9->addWidget(rateLabel);

        audiorateLabel = new QLabel(dabradio);
        audiorateLabel->setObjectName("audiorateLabel");

        horizontalLayout_9->addWidget(audiorateLabel);

        psLabel = new QLabel(dabradio);
        psLabel->setObjectName("psLabel");

        horizontalLayout_9->addWidget(psLabel);

        sbrLabel = new QLabel(dabradio);
        sbrLabel->setObjectName("sbrLabel");

        horizontalLayout_9->addWidget(sbrLabel);

        soundLabel = new clickablelabel(dabradio);
        soundLabel->setObjectName("soundLabel");

        horizontalLayout_9->addWidget(soundLabel);


        verticalLayout->addLayout(horizontalLayout_9);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        pictureLabel = new QLabel(dabradio);
        pictureLabel->setObjectName("pictureLabel");

        horizontalLayout_11->addWidget(pictureLabel);

        volumeSlider = new QSlider(dabradio);
        volumeSlider->setObjectName("volumeSlider");
        volumeSlider->setOrientation(Qt::Orientation::Vertical);

        horizontalLayout_11->addWidget(volumeSlider);


        verticalLayout->addLayout(horizontalLayout_11);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        dynamicLabel = new QLabel(dabradio);
        dynamicLabel->setObjectName("dynamicLabel");
        QFont font;
        font.setPointSize(14);
        font.setBold(false);
        font.setItalic(false);
        dynamicLabel->setFont(font);
        dynamicLabel->setWordWrap(true);

        horizontalLayout_12->addWidget(dynamicLabel);


        verticalLayout->addLayout(horizontalLayout_12);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        configButton = new smallPushButton(dabradio);
        configButton->setObjectName("configButton");

        horizontalLayout_10->addWidget(configButton);

        spectrumButton = new smallPushButton(dabradio);
        spectrumButton->setObjectName("spectrumButton");

        horizontalLayout_10->addWidget(spectrumButton);

        httpButton = new smallPushButton(dabradio);
        httpButton->setObjectName("httpButton");

        horizontalLayout_10->addWidget(httpButton);

        scanButton = new smallPushButton(dabradio);
        scanButton->setObjectName("scanButton");

        horizontalLayout_10->addWidget(scanButton);


        verticalLayout->addLayout(horizontalLayout_10);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        leftAudio = new QwtThermo(dabradio);
        leftAudio->setObjectName("leftAudio");
        leftAudio->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
        leftAudio->setLowerBound(-20.000000000000000);
        leftAudio->setUpperBound(0.000000000000000);
        leftAudio->setScaleMaxMajor(9);
        leftAudio->setScaleMaxMinor(2);
        leftAudio->setScaleStepSize(3.000000000000000);
        leftAudio->setOrientation(Qt::Orientation::Horizontal);
        leftAudio->setScalePosition(QwtThermo::ScalePosition::TrailingScale);
        leftAudio->setOriginMode(QwtThermo::OriginMode::OriginMinimum);
        leftAudio->setSpacing(2);
        leftAudio->setPipeWidth(6);

        horizontalLayout_5->addWidget(leftAudio);

        rightAudio = new QwtThermo(dabradio);
        rightAudio->setObjectName("rightAudio");
        rightAudio->setLowerBound(-20.000000000000000);
        rightAudio->setUpperBound(0.000000000000000);
        rightAudio->setScaleMaxMajor(9);
        rightAudio->setScaleMaxMinor(4);
        rightAudio->setScaleStepSize(3.000000000000000);
        rightAudio->setOrientation(Qt::Orientation::Horizontal);
        rightAudio->setScalePosition(QwtThermo::ScalePosition::TrailingScale);
        rightAudio->setSpacing(3);
        rightAudio->setBorderWidth(1);
        rightAudio->setPipeWidth(6);

        horizontalLayout_5->addWidget(rightAudio);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName("horizontalLayout_13");
        distanceLabel = new QLabel(dabradio);
        distanceLabel->setObjectName("distanceLabel");
        QFont font1;
        font1.setPointSize(9);
        distanceLabel->setFont(font1);

        horizontalLayout_13->addWidget(distanceLabel);

        horizontalSpacer = new QSpacerItem(168, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer);

        tiiButton = new smallPushButton(dabradio);
        tiiButton->setObjectName("tiiButton");

        horizontalLayout_13->addWidget(tiiButton);


        verticalLayout->addLayout(horizontalLayout_13);


        horizontalLayout_14->addLayout(verticalLayout);


        retranslateUi(dabradio);

        QMetaObject::connectSlotsByName(dabradio);
    } // setupUi

    void retranslateUi(QWidget *dabradio)
    {
        dabradio->setWindowTitle(QCoreApplication::translate("dabradio", "Qt-DAB-6.8", nullptr));
#if QT_CONFIG(tooltip)
        ensembleId->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>The name of the ensemble in the currently selected channel. Touching the name shows a widget with a description of the ensemble content.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        ensembleId->setText(QCoreApplication::translate("dabradio", "ensemble", nullptr));
        transmitter_country->setText(QCoreApplication::translate("dabradio", "country", nullptr));
#if QT_CONFIG(tooltip)
        prevServiceButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head></head><body>\n"
"<p>Select the previous service on the list of services<br />(the previous one of the first element is the last element of the list).</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        prevServiceButton->setText(QCoreApplication::translate("dabradio", "-", nullptr));
#if QT_CONFIG(tooltip)
        nextServiceButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Select the next service on the current ensemble.<br/>(the next of the last element is the first element).</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        nextServiceButton->setText(QCoreApplication::translate("dabradio", "+", nullptr));
        prevChannelButton->setText(QCoreApplication::translate("dabradio", "prevLabel", nullptr));
        nextChannelButton->setText(QCoreApplication::translate("dabradio", "nextLabel", nullptr));
#if QT_CONFIG(tooltip)
        scanListButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Result of a (single) scan is stored in the list. Left mouse click on an element will select the service, named by the element. Right mouse click causes the service name to be added to the preset list.</p><p>If clearScan is selected (the default), the list is cleared when scanning.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        scanListButton->setText(QCoreApplication::translate("dabradio", "scanlist", nullptr));
#if QT_CONFIG(tooltip)
        presetButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Swutch between the <span style=\" font-style:italic;\">ensemble view</span> and the <span style=\" font-style:italic;\">favorites view.</span> The ensemble view shows the list of services in the currently active ensemble. The favories view shows the list of services selected as favorite. </p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        presetButton->setText(QCoreApplication::translate("dabradio", "favorites", nullptr));
#if QT_CONFIG(tooltip)
        folder_shower->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Touching this icon insrtructs the software to start a shell and run the explorer (or the Linux equivalent)  showing the folder/directory where Qt-DAB stores its files</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        folder_shower->setText(QCoreApplication::translate("dabradio", "files", nullptr));
#if QT_CONFIG(tooltip)
        localTimeDisplay->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>The local time. If you want UTC instead, set the utc selector on the configuration and control widget.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        localTimeDisplay->setText(QCoreApplication::translate("dabradio", "local", nullptr));
#if QT_CONFIG(tooltip)
        runtimeDisplay->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>The run time is what the name suggests, the time the current instance of the software is running.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        runtimeDisplay->setText(QCoreApplication::translate("dabradio", "runtime", nullptr));
        aboutLabel->setText(QCoreApplication::translate("dabradio", "\302\251", nullptr));
#if QT_CONFIG(tooltip)
        snrLabel->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>A simple &quot;signal strength&quot; indicator, telling in graphical terms the SNR of the signal.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        snrLabel->setText(QCoreApplication::translate("dabradio", "TextLabel", nullptr));
#if QT_CONFIG(tooltip)
        serviceButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Touching this icon controls the visibility of the so-called technical widget. The technical widget shows all details of the currently selected (audio)service.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        serviceButton->setText(QCoreApplication::translate("dabradio", "tt", nullptr));
#if QT_CONFIG(tooltip)
        serviceLabel->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>The service label, i.e. the name of the currently selected service</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        serviceLabel->setText(QCoreApplication::translate("dabradio", "service", nullptr));
        programTypeLabel->setText(QCoreApplication::translate("dabradio", "pty", nullptr));
#if QT_CONFIG(tooltip)
        motLabel->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Indicator whether or not MOT data is part of the service. If not, the color will be red, otherwise it takes the color of the other indicators.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        motLabel->setText(QCoreApplication::translate("dabradio", "mot", nullptr));
        stereoLabel->setText(QCoreApplication::translate("dabradio", "stereo", nullptr));
        rateLabel->setText(QCoreApplication::translate("dabradio", "ratelabel", nullptr));
        audiorateLabel->setText(QCoreApplication::translate("dabradio", "48000", nullptr));
        psLabel->setText(QCoreApplication::translate("dabradio", "ps", nullptr));
        sbrLabel->setText(QCoreApplication::translate("dabradio", "sbr", nullptr));
#if QT_CONFIG(tooltip)
        soundLabel->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>This icon shows whether or not audio is (or should be) generated, and can be used as &quot;mute&quot; button.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        soundLabel->setText(QCoreApplication::translate("dabradio", "volume", nullptr));
        pictureLabel->setText(QCoreApplication::translate("dabradio", "No Slide", nullptr));
#if QT_CONFIG(tooltip)
        volumeSlider->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>The Volume slider is part of the Qt_Audio audio subsystem.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        dynamicLabel->setText(QCoreApplication::translate("dabradio", "TextLabel", nullptr));
#if QT_CONFIG(tooltip)
        configButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Hide or  show the configuration-control widget</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        configButton->setText(QCoreApplication::translate("dabradio", "controls", nullptr));
#if QT_CONFIG(tooltip)
        spectrumButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Touch this button to show or hide the so-called spectrum widget, a widget with a number of displays showing the incoming signal.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        spectrumButton->setText(QCoreApplication::translate("dabradio", "spectrum", nullptr));
#if QT_CONFIG(tooltip)
        httpButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Start (or stop) a simple webserver for showing a map on which the locations of the transmitters found are shown</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        httpButton->setText(QCoreApplication::translate("dabradio", "http", nullptr));
#if QT_CONFIG(tooltip)
        scanButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Show or hide the scanmonitor. Scan control is completely moved to the scanmonitor</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        scanButton->setText(QCoreApplication::translate("dabradio", "scan", nullptr));
#if QT_CONFIG(tooltip)
        distanceLabel->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>If configured, this line shows the name of the transmitter that is received.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        distanceLabel->setText(QCoreApplication::translate("dabradio", "TextLabel", nullptr));
        tiiButton->setText(QCoreApplication::translate("dabradio", "tii disply", nullptr));
    } // retranslateUi

};

namespace Ui {
    class dabradio: public Ui_dabradio {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DABRADIO_2D_6_H
