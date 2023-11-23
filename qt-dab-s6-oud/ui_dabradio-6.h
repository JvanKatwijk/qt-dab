/********************************************************************************
** Form generated from reading UI file 'dabradio-6.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DABRADIO_2D_6_H
#define UI_DABRADIO_2D_6_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <newpushbutton.h>
#include "smallcombobox.h"
#include "smallpushbutton.h"
#include "smallqlistview.h"

QT_BEGIN_NAMESPACE

class Ui_dabradio
{
public:
    QHBoxLayout *horizontalLayout_9;
    QVBoxLayout *verticalLayout_2;
    QWidget *leftPane;
    QVBoxLayout *verticalLayout_3;
    smallQListView *ensembleDisplay;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout_2;
    smallPushButton *prevServiceButton;
    smallPushButton *nextServiceButton;
    smallComboBox *channelSelector;
    QHBoxLayout *horizontalLayout_8;
    smallPushButton *prevChannelButton;
    smallPushButton *nextChannelButton;
    QHBoxLayout *horizontalLayout_7;
    newPushButton *scanListButton;
    newPushButton *presetButton;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_6;
    QLCDNumber *stillMuting;
    QLabel *localTimeDisplay;
    QLabel *runtimeDisplay;
    QLabel *copyrightLabel;
    QLCDNumber *cpuMonitor;
    QHBoxLayout *horizontalLayout;
    QLabel *ensembleId;
    QLabel *transmitter_country;
    QLabel *transmitter_coordinates;
    QHBoxLayout *horizontalLayout_3;
    QLabel *stereoLabel;
    QLabel *serviceLabel;
    QLabel *programTypeLabel;
    QLabel *pictureLabel;
    QLabel *dynamicLabel;
    QGridLayout *gridLayout;
    smallPushButton *configButton;
    smallPushButton *spectrumButton;
    smallPushButton *detailButton;
    smallPushButton *muteButton;
    smallPushButton *httpButton;
    smallPushButton *contentButton;
    smallPushButton *scheduleButton;
    smallPushButton *scanButton;
    QHBoxLayout *horizontalLayout_5;
    QLabel *distanceLabel;

    void setupUi(QWidget *dabradio)
    {
        if (dabradio->objectName().isEmpty())
            dabradio->setObjectName(QString::fromUtf8("dabradio"));
        dabradio->resize(559, 408);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dabradio->sizePolicy().hasHeightForWidth());
        dabradio->setSizePolicy(sizePolicy);
        horizontalLayout_9 = new QHBoxLayout(dabradio);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        leftPane = new QWidget(dabradio);
        leftPane->setObjectName(QString::fromUtf8("leftPane"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(3);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(leftPane->sizePolicy().hasHeightForWidth());
        leftPane->setSizePolicy(sizePolicy1);
        verticalLayout_3 = new QVBoxLayout(leftPane);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        ensembleDisplay = new smallQListView(leftPane);
        ensembleDisplay->setObjectName(QString::fromUtf8("ensembleDisplay"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy2.setHorizontalStretch(2);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(ensembleDisplay->sizePolicy().hasHeightForWidth());
        ensembleDisplay->setSizePolicy(sizePolicy2);
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu Sans"));
        font.setPointSize(10);
        ensembleDisplay->setFont(font);

        verticalLayout_3->addWidget(ensembleDisplay);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));

        verticalLayout_3->addLayout(horizontalLayout_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        prevServiceButton = new smallPushButton(leftPane);
        prevServiceButton->setObjectName(QString::fromUtf8("prevServiceButton"));

        horizontalLayout_2->addWidget(prevServiceButton);

        nextServiceButton = new smallPushButton(leftPane);
        nextServiceButton->setObjectName(QString::fromUtf8("nextServiceButton"));

        horizontalLayout_2->addWidget(nextServiceButton);


        verticalLayout_3->addLayout(horizontalLayout_2);

        channelSelector = new smallComboBox(leftPane);
        channelSelector->addItem(QString());
        channelSelector->setObjectName(QString::fromUtf8("channelSelector"));

        verticalLayout_3->addWidget(channelSelector);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        prevChannelButton = new smallPushButton(leftPane);
        prevChannelButton->setObjectName(QString::fromUtf8("prevChannelButton"));

        horizontalLayout_8->addWidget(prevChannelButton);

        nextChannelButton = new smallPushButton(leftPane);
        nextChannelButton->setObjectName(QString::fromUtf8("nextChannelButton"));

        horizontalLayout_8->addWidget(nextChannelButton);


        verticalLayout_3->addLayout(horizontalLayout_8);


        verticalLayout_2->addWidget(leftPane);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        scanListButton = new newPushButton(dabradio);
        scanListButton->setObjectName(QString::fromUtf8("scanListButton"));

        horizontalLayout_7->addWidget(scanListButton);

        presetButton = new newPushButton(dabradio);
        presetButton->setObjectName(QString::fromUtf8("presetButton"));

        horizontalLayout_7->addWidget(presetButton);


        verticalLayout_2->addLayout(horizontalLayout_7);


        horizontalLayout_9->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        stillMuting = new QLCDNumber(dabradio);
        stillMuting->setObjectName(QString::fromUtf8("stillMuting"));

        horizontalLayout_6->addWidget(stillMuting);

        localTimeDisplay = new QLabel(dabradio);
        localTimeDisplay->setObjectName(QString::fromUtf8("localTimeDisplay"));

        horizontalLayout_6->addWidget(localTimeDisplay);

        runtimeDisplay = new QLabel(dabradio);
        runtimeDisplay->setObjectName(QString::fromUtf8("runtimeDisplay"));

        horizontalLayout_6->addWidget(runtimeDisplay);

        copyrightLabel = new QLabel(dabradio);
        copyrightLabel->setObjectName(QString::fromUtf8("copyrightLabel"));

        horizontalLayout_6->addWidget(copyrightLabel);

        cpuMonitor = new QLCDNumber(dabradio);
        cpuMonitor->setObjectName(QString::fromUtf8("cpuMonitor"));
        QFont font1;
        font1.setPointSize(10);
        cpuMonitor->setFont(font1);
        cpuMonitor->setFrameShape(QFrame::NoFrame);
        cpuMonitor->setFrameShadow(QFrame::Plain);
        cpuMonitor->setSegmentStyle(QLCDNumber::Flat);

        horizontalLayout_6->addWidget(cpuMonitor);


        verticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        ensembleId = new QLabel(dabradio);
        ensembleId->setObjectName(QString::fromUtf8("ensembleId"));

        horizontalLayout->addWidget(ensembleId);

        transmitter_country = new QLabel(dabradio);
        transmitter_country->setObjectName(QString::fromUtf8("transmitter_country"));

        horizontalLayout->addWidget(transmitter_country);

        transmitter_coordinates = new QLabel(dabradio);
        transmitter_coordinates->setObjectName(QString::fromUtf8("transmitter_coordinates"));
        QFont font2;
        font2.setPointSize(13);
        transmitter_coordinates->setFont(font2);

        horizontalLayout->addWidget(transmitter_coordinates);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        stereoLabel = new QLabel(dabradio);
        stereoLabel->setObjectName(QString::fromUtf8("stereoLabel"));
        stereoLabel->setMaximumSize(QSize(61, 16777215));

        horizontalLayout_3->addWidget(stereoLabel);

        serviceLabel = new QLabel(dabradio);
        serviceLabel->setObjectName(QString::fromUtf8("serviceLabel"));

        horizontalLayout_3->addWidget(serviceLabel);

        programTypeLabel = new QLabel(dabradio);
        programTypeLabel->setObjectName(QString::fromUtf8("programTypeLabel"));
        programTypeLabel->setMaximumSize(QSize(91, 16777215));

        horizontalLayout_3->addWidget(programTypeLabel);


        verticalLayout->addLayout(horizontalLayout_3);

        pictureLabel = new QLabel(dabradio);
        pictureLabel->setObjectName(QString::fromUtf8("pictureLabel"));

        verticalLayout->addWidget(pictureLabel);

        dynamicLabel = new QLabel(dabradio);
        dynamicLabel->setObjectName(QString::fromUtf8("dynamicLabel"));
        QFont font3;
        font3.setPointSize(14);
        font3.setBold(true);
        font3.setItalic(true);
        font3.setWeight(75);
        dynamicLabel->setFont(font3);

        verticalLayout->addWidget(dynamicLabel);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        configButton = new smallPushButton(dabradio);
        configButton->setObjectName(QString::fromUtf8("configButton"));

        gridLayout->addWidget(configButton, 0, 0, 1, 1);

        spectrumButton = new smallPushButton(dabradio);
        spectrumButton->setObjectName(QString::fromUtf8("spectrumButton"));

        gridLayout->addWidget(spectrumButton, 0, 1, 1, 1);

        detailButton = new smallPushButton(dabradio);
        detailButton->setObjectName(QString::fromUtf8("detailButton"));

        gridLayout->addWidget(detailButton, 0, 2, 1, 1);

        muteButton = new smallPushButton(dabradio);
        muteButton->setObjectName(QString::fromUtf8("muteButton"));

        gridLayout->addWidget(muteButton, 0, 3, 1, 2);

        httpButton = new smallPushButton(dabradio);
        httpButton->setObjectName(QString::fromUtf8("httpButton"));

        gridLayout->addWidget(httpButton, 1, 0, 1, 1);

        contentButton = new smallPushButton(dabradio);
        contentButton->setObjectName(QString::fromUtf8("contentButton"));

        gridLayout->addWidget(contentButton, 1, 1, 1, 1);

        scheduleButton = new smallPushButton(dabradio);
        scheduleButton->setObjectName(QString::fromUtf8("scheduleButton"));

        gridLayout->addWidget(scheduleButton, 1, 2, 1, 2);

        scanButton = new smallPushButton(dabradio);
        scanButton->setObjectName(QString::fromUtf8("scanButton"));

        gridLayout->addWidget(scanButton, 1, 4, 1, 1);


        verticalLayout->addLayout(gridLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        distanceLabel = new QLabel(dabradio);
        distanceLabel->setObjectName(QString::fromUtf8("distanceLabel"));

        horizontalLayout_5->addWidget(distanceLabel);


        verticalLayout->addLayout(horizontalLayout_5);


        horizontalLayout_9->addLayout(verticalLayout);


        retranslateUi(dabradio);

        QMetaObject::connectSlotsByName(dabradio);
    } // setupUi

    void retranslateUi(QWidget *dabradio)
    {
        dabradio->setWindowTitle(QCoreApplication::translate("dabradio", "Qt-DAB-6.1", nullptr));
#if QT_CONFIG(tooltip)
        ensembleDisplay->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Selecting a service is by clicking with the <span style=\" font-weight:600; font-style:italic;\">left </span>mouse button on the name of the service.</p><p>Clicking with the <span style=\" font-weight:600; font-style:italic;\">right mouse button on the name of the selected service </span>in this list will add the service to the presets.</p><p>Clicking with the right mouse button on the name of any other of the listed (audio)services will start a background process, with which the AAC frames are stored onto a file</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        prevServiceButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head></head><body>\n"
"<p>Select the previous service on the list of services<br />(the previous one of the first element is the last element of the list).</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        prevServiceButton->setText(QCoreApplication::translate("dabradio", "-", nullptr));
#if QT_CONFIG(tooltip)
        nextServiceButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head /> <body><p>Select the next service on the list of services <br />(the next of the last element is the first element).</body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        nextServiceButton->setText(QCoreApplication::translate("dabradio", "+", nullptr));
        channelSelector->setItemText(0, QCoreApplication::translate("dabradio", "channels", nullptr));

#if QT_CONFIG(tooltip)
        channelSelector->setToolTip(QCoreApplication::translate("dabradio", "Channel selector.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        prevChannelButton->setToolTip(QCoreApplication::translate("dabradio", "Select the previous channel.", nullptr));
#endif // QT_CONFIG(tooltip)
        prevChannelButton->setText(QCoreApplication::translate("dabradio", "-", nullptr));
#if QT_CONFIG(tooltip)
        nextChannelButton->setToolTip(QCoreApplication::translate("dabradio", "Select the next channel of the list maintained in the channel selector.", nullptr));
#endif // QT_CONFIG(tooltip)
        nextChannelButton->setText(QCoreApplication::translate("dabradio", "+", nullptr));
#if QT_CONFIG(tooltip)
        scanListButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Result of a (single) scan is stored in the list. Left mouse click on an element will select the service, named by the element. Right mouse click causes the service name to be added to the preset list.</p><p>If clearScan is selected (the default), the list is cleared when scanning.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        scanListButton->setText(QCoreApplication::translate("dabradio", "scanlist", nullptr));
        presetButton->setText(QCoreApplication::translate("dabradio", "favorites", nullptr));
        localTimeDisplay->setText(QCoreApplication::translate("dabradio", "local", nullptr));
        runtimeDisplay->setText(QCoreApplication::translate("dabradio", "runtime", nullptr));
        copyrightLabel->setText(QCoreApplication::translate("dabradio", "\302\251", nullptr));
#if QT_CONFIG(tooltip)
        cpuMonitor->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>cpuLoad. The number gives an estimate of the overall load of the CPU of the machine you are working on. So, it only gives an idea of the load caused by Qt-DAB if no other (large) programs are active.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        ensembleId->setText(QCoreApplication::translate("dabradio", "ensemble", nullptr));
        transmitter_country->setText(QCoreApplication::translate("dabradio", "country", nullptr));
        transmitter_coordinates->setText(QCoreApplication::translate("dabradio", "TII", nullptr));
        stereoLabel->setText(QCoreApplication::translate("dabradio", "stereo", nullptr));
        serviceLabel->setText(QCoreApplication::translate("dabradio", "service", nullptr));
        programTypeLabel->setText(QCoreApplication::translate("dabradio", "pty", nullptr));
        pictureLabel->setText(QCoreApplication::translate("dabradio", "No Slide", nullptr));
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
        detailButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Show or hide a widget showing details of the currently selected service</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        detailButton->setText(QCoreApplication::translate("dabradio", "details", nullptr));
#if QT_CONFIG(tooltip)
        muteButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Touch this button to mute - or unmute - the audio. The duration of muting is displayed. Duration can be set in the configuration and control widget.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        muteButton->setText(QCoreApplication::translate("dabradio", "mute", nullptr));
#if QT_CONFIG(tooltip)
        httpButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Start (or stop) a simple webserver for showing a map on which the locations of the transmitters found are shown</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        httpButton->setText(QCoreApplication::translate("dabradio", "http", nullptr));
#if QT_CONFIG(tooltip)
        contentButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Touch this button to see what the content of the currently selected ensemble is.</p><p>Specification is pretty detailed and can be saved in a &quot;.csv&quot; file</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        contentButton->setText(QCoreApplication::translate("dabradio", "content", nullptr));
#if QT_CONFIG(tooltip)
        scheduleButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Touch to add a command to the schedule list. Commands can be given for up to 7 days ahead.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        scheduleButton->setText(QCoreApplication::translate("dabradio", "scheduler", nullptr));
#if QT_CONFIG(tooltip)
        scanButton->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>Start or stop a scan. The scanMode can be selected in the configuration-control widget</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        scanButton->setText(QCoreApplication::translate("dabradio", "scan", nullptr));
#if QT_CONFIG(tooltip)
        distanceLabel->setToolTip(QCoreApplication::translate("dabradio", "<html><head/><body><p>If configured, this line shows the name of the transmitter that is received.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        distanceLabel->setText(QCoreApplication::translate("dabradio", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class dabradio: public Ui_dabradio {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DABRADIO_2D_6_H
