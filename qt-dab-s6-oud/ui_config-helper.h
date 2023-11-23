/********************************************************************************
** Form generated from reading UI file 'config-helper.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIG_2D_HELPER_H
#define UI_CONFIG_2D_HELPER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <smallspinbox.h>
#include "smallcombobox.h"
#include "smallpushbutton.h"

QT_BEGIN_NAMESPACE

class Ui_configWidget
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    smallSpinBox *muteTimeSetting;
    QLabel *label_2;
    smallSpinBox *switchDelaySetting;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QRadioButton *orderAlfabetical;
    QRadioButton *orderServiceIds;
    QRadioButton *ordersubChannelIds;
    QGridLayout *gridLayout;
    smallPushButton *devicewidgetButton;
    smallPushButton *portSelector;
    smallPushButton *dlTextButton;
    smallPushButton *resetButton;
    smallPushButton *fontButton;
    smallPushButton *snrButton;
    smallPushButton *set_coordinatesButton;
    smallPushButton *loadTableButton;
    smallPushButton *dumpButton;
    smallPushButton *skinButton;
    QGridLayout *gridLayout_2;
    QCheckBox *saveServiceSelector;
    QCheckBox *loggerButton;
    QCheckBox *epg2xmlSelector;
    QCheckBox *tii_detectorMode;
    QCheckBox *utcSelector;
    QCheckBox *onTop;
    QCheckBox *closeDirect;
    QCheckBox *epgSelector;
    QCheckBox *autoBrowser;
    QCheckBox *dcRemovalSelector;
    QCheckBox *eti_activeSelector;
    QCheckBox *transmitterTags;
    QCheckBox *clearScan_Selector;
    QCheckBox *saveSlides;
    QCheckBox *transmSelector;
    QHBoxLayout *horizontalLayout_2;
    smallComboBox *scanmodeSelector;
    QPushButton *skipList_button;
    QPushButton *skipFile_button;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *servicesInBackground;
    smallComboBox *decoderSelector;
    smallComboBox *streamoutSelector;
    smallComboBox *deviceSelector;
    QButtonGroup *buttonGroup;

    void setupUi(QWidget *configWidget)
    {
        if (configWidget->objectName().isEmpty())
            configWidget->setObjectName(QString::fromUtf8("configWidget"));
        configWidget->resize(548, 464);
        verticalLayout_3 = new QVBoxLayout(configWidget);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(configWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        muteTimeSetting = new smallSpinBox(configWidget);
        muteTimeSetting->setObjectName(QString::fromUtf8("muteTimeSetting"));

        formLayout->setWidget(0, QFormLayout::FieldRole, muteTimeSetting);

        label_2 = new QLabel(configWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        switchDelaySetting = new smallSpinBox(configWidget);
        switchDelaySetting->setObjectName(QString::fromUtf8("switchDelaySetting"));

        formLayout->setWidget(1, QFormLayout::FieldRole, switchDelaySetting);


        horizontalLayout->addLayout(formLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        orderAlfabetical = new QRadioButton(configWidget);
        buttonGroup = new QButtonGroup(configWidget);
        buttonGroup->setObjectName(QString::fromUtf8("buttonGroup"));
        buttonGroup->addButton(orderAlfabetical);
        orderAlfabetical->setObjectName(QString::fromUtf8("orderAlfabetical"));

        verticalLayout->addWidget(orderAlfabetical);

        orderServiceIds = new QRadioButton(configWidget);
        buttonGroup->addButton(orderServiceIds);
        orderServiceIds->setObjectName(QString::fromUtf8("orderServiceIds"));

        verticalLayout->addWidget(orderServiceIds);

        ordersubChannelIds = new QRadioButton(configWidget);
        buttonGroup->addButton(ordersubChannelIds);
        ordersubChannelIds->setObjectName(QString::fromUtf8("ordersubChannelIds"));

        verticalLayout->addWidget(ordersubChannelIds);


        verticalLayout_2->addLayout(verticalLayout);


        horizontalLayout->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        devicewidgetButton = new smallPushButton(configWidget);
        devicewidgetButton->setObjectName(QString::fromUtf8("devicewidgetButton"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(devicewidgetButton->sizePolicy().hasHeightForWidth());
        devicewidgetButton->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QString::fromUtf8("Cantarell"));
        font.setPointSize(10);
        devicewidgetButton->setFont(font);

        gridLayout->addWidget(devicewidgetButton, 0, 0, 1, 1);

        portSelector = new smallPushButton(configWidget);
        portSelector->setObjectName(QString::fromUtf8("portSelector"));
        QFont font1;
        font1.setPointSize(12);
        portSelector->setFont(font1);

        gridLayout->addWidget(portSelector, 0, 1, 1, 1);

        dlTextButton = new smallPushButton(configWidget);
        dlTextButton->setObjectName(QString::fromUtf8("dlTextButton"));
        QFont font2;
        font2.setPointSize(10);
        dlTextButton->setFont(font2);

        gridLayout->addWidget(dlTextButton, 0, 2, 1, 2);

        resetButton = new smallPushButton(configWidget);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));
        sizePolicy.setHeightForWidth(resetButton->sizePolicy().hasHeightForWidth());
        resetButton->setSizePolicy(sizePolicy);
        resetButton->setFont(font2);

        gridLayout->addWidget(resetButton, 0, 4, 1, 2);

        fontButton = new smallPushButton(configWidget);
        fontButton->setObjectName(QString::fromUtf8("fontButton"));

        gridLayout->addWidget(fontButton, 0, 6, 1, 1);

        snrButton = new smallPushButton(configWidget);
        snrButton->setObjectName(QString::fromUtf8("snrButton"));
        sizePolicy.setHeightForWidth(snrButton->sizePolicy().hasHeightForWidth());
        snrButton->setSizePolicy(sizePolicy);
        snrButton->setFont(font);

        gridLayout->addWidget(snrButton, 1, 0, 1, 1);

        set_coordinatesButton = new smallPushButton(configWidget);
        set_coordinatesButton->setObjectName(QString::fromUtf8("set_coordinatesButton"));
        sizePolicy.setHeightForWidth(set_coordinatesButton->sizePolicy().hasHeightForWidth());
        set_coordinatesButton->setSizePolicy(sizePolicy);
        set_coordinatesButton->setFont(font);

        gridLayout->addWidget(set_coordinatesButton, 1, 1, 1, 2);

        loadTableButton = new smallPushButton(configWidget);
        loadTableButton->setObjectName(QString::fromUtf8("loadTableButton"));
        loadTableButton->setFont(font2);

        gridLayout->addWidget(loadTableButton, 1, 3, 1, 2);

        dumpButton = new smallPushButton(configWidget);
        dumpButton->setObjectName(QString::fromUtf8("dumpButton"));
        sizePolicy.setHeightForWidth(dumpButton->sizePolicy().hasHeightForWidth());
        dumpButton->setSizePolicy(sizePolicy);
        dumpButton->setFont(font2);

        gridLayout->addWidget(dumpButton, 1, 5, 1, 1);

        skinButton = new smallPushButton(configWidget);
        skinButton->setObjectName(QString::fromUtf8("skinButton"));

        gridLayout->addWidget(skinButton, 1, 6, 1, 1);


        verticalLayout_3->addLayout(gridLayout);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        saveServiceSelector = new QCheckBox(configWidget);
        saveServiceSelector->setObjectName(QString::fromUtf8("saveServiceSelector"));

        gridLayout_2->addWidget(saveServiceSelector, 0, 0, 1, 1);

        loggerButton = new QCheckBox(configWidget);
        loggerButton->setObjectName(QString::fromUtf8("loggerButton"));

        gridLayout_2->addWidget(loggerButton, 0, 1, 1, 1);

        epg2xmlSelector = new QCheckBox(configWidget);
        epg2xmlSelector->setObjectName(QString::fromUtf8("epg2xmlSelector"));

        gridLayout_2->addWidget(epg2xmlSelector, 0, 2, 1, 1);

        tii_detectorMode = new QCheckBox(configWidget);
        tii_detectorMode->setObjectName(QString::fromUtf8("tii_detectorMode"));

        gridLayout_2->addWidget(tii_detectorMode, 1, 0, 1, 1);

        utcSelector = new QCheckBox(configWidget);
        utcSelector->setObjectName(QString::fromUtf8("utcSelector"));

        gridLayout_2->addWidget(utcSelector, 1, 1, 1, 1);

        onTop = new QCheckBox(configWidget);
        onTop->setObjectName(QString::fromUtf8("onTop"));

        gridLayout_2->addWidget(onTop, 1, 2, 1, 1);

        closeDirect = new QCheckBox(configWidget);
        closeDirect->setObjectName(QString::fromUtf8("closeDirect"));

        gridLayout_2->addWidget(closeDirect, 2, 0, 1, 1);

        epgSelector = new QCheckBox(configWidget);
        epgSelector->setObjectName(QString::fromUtf8("epgSelector"));

        gridLayout_2->addWidget(epgSelector, 2, 1, 1, 1);

        autoBrowser = new QCheckBox(configWidget);
        autoBrowser->setObjectName(QString::fromUtf8("autoBrowser"));

        gridLayout_2->addWidget(autoBrowser, 2, 2, 1, 1);

        dcRemovalSelector = new QCheckBox(configWidget);
        dcRemovalSelector->setObjectName(QString::fromUtf8("dcRemovalSelector"));

        gridLayout_2->addWidget(dcRemovalSelector, 3, 0, 1, 1);

        eti_activeSelector = new QCheckBox(configWidget);
        eti_activeSelector->setObjectName(QString::fromUtf8("eti_activeSelector"));

        gridLayout_2->addWidget(eti_activeSelector, 3, 1, 1, 1);

        transmitterTags = new QCheckBox(configWidget);
        transmitterTags->setObjectName(QString::fromUtf8("transmitterTags"));

        gridLayout_2->addWidget(transmitterTags, 3, 2, 1, 1);

        clearScan_Selector = new QCheckBox(configWidget);
        clearScan_Selector->setObjectName(QString::fromUtf8("clearScan_Selector"));

        gridLayout_2->addWidget(clearScan_Selector, 4, 0, 1, 1);

        saveSlides = new QCheckBox(configWidget);
        saveSlides->setObjectName(QString::fromUtf8("saveSlides"));

        gridLayout_2->addWidget(saveSlides, 4, 1, 1, 1);

        transmSelector = new QCheckBox(configWidget);
        transmSelector->setObjectName(QString::fromUtf8("transmSelector"));

        gridLayout_2->addWidget(transmSelector, 4, 2, 1, 1);


        verticalLayout_3->addLayout(gridLayout_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        scanmodeSelector = new smallComboBox(configWidget);
        scanmodeSelector->addItem(QString());
        scanmodeSelector->addItem(QString());
        scanmodeSelector->addItem(QString());
        scanmodeSelector->setObjectName(QString::fromUtf8("scanmodeSelector"));
        scanmodeSelector->setFont(font1);

        horizontalLayout_2->addWidget(scanmodeSelector);

        skipList_button = new QPushButton(configWidget);
        skipList_button->setObjectName(QString::fromUtf8("skipList_button"));
        skipList_button->setFont(font1);

        horizontalLayout_2->addWidget(skipList_button);

        skipFile_button = new QPushButton(configWidget);
        skipFile_button->setObjectName(QString::fromUtf8("skipFile_button"));
        skipFile_button->setFont(font1);

        horizontalLayout_2->addWidget(skipFile_button);


        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        servicesInBackground = new QCheckBox(configWidget);
        servicesInBackground->setObjectName(QString::fromUtf8("servicesInBackground"));

        horizontalLayout_3->addWidget(servicesInBackground);

        decoderSelector = new smallComboBox(configWidget);
        decoderSelector->setObjectName(QString::fromUtf8("decoderSelector"));

        horizontalLayout_3->addWidget(decoderSelector);

        streamoutSelector = new smallComboBox(configWidget);
        streamoutSelector->setObjectName(QString::fromUtf8("streamoutSelector"));
        streamoutSelector->setMaximumSize(QSize(162, 16777215));

        horizontalLayout_3->addWidget(streamoutSelector);

        deviceSelector = new smallComboBox(configWidget);
        deviceSelector->setObjectName(QString::fromUtf8("deviceSelector"));

        horizontalLayout_3->addWidget(deviceSelector);


        verticalLayout_3->addLayout(horizontalLayout_3);


        retranslateUi(configWidget);

        QMetaObject::connectSlotsByName(configWidget);
    } // setupUi

    void retranslateUi(QWidget *configWidget)
    {
        configWidget->setWindowTitle(QCoreApplication::translate("configWidget", "Configuration and control", nullptr));
#if QT_CONFIG(tooltip)
        configWidget->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Configuration and control widget.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label->setText(QCoreApplication::translate("configWidget", "mute time (in minutes)", nullptr));
#if QT_CONFIG(tooltip)
        muteTimeSetting->setToolTip(QCoreApplication::translate("configWidget", "Set here the mute time in minutes", nullptr));
#endif // QT_CONFIG(tooltip)
        label_2->setText(QCoreApplication::translate("configWidget", "switch time (in seconds)", nullptr));
#if QT_CONFIG(tooltip)
        switchDelaySetting->setToolTip(QCoreApplication::translate("configWidget", "Set here the delay, when searching for data in a channel, in seconds", nullptr));
#endif // QT_CONFIG(tooltip)
        orderAlfabetical->setText(QCoreApplication::translate("configWidget", "alphabetically", nullptr));
        orderServiceIds->setText(QCoreApplication::translate("configWidget", "serviceIds", nullptr));
        ordersubChannelIds->setText(QCoreApplication::translate("configWidget", "subchannelIds", nullptr));
#if QT_CONFIG(tooltip)
        devicewidgetButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>show or hide the widget for controlling the device.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        devicewidgetButton->setText(QCoreApplication::translate("configWidget", "device", nullptr));
        portSelector->setText(QCoreApplication::translate("configWidget", "port", nullptr));
#if QT_CONFIG(tooltip)
        dlTextButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Save the lines of text that appear as dynamic label</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        dlTextButton->setText(QCoreApplication::translate("configWidget", "dlText", nullptr));
#if QT_CONFIG(tooltip)
        resetButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Push this button to reset, i.e. restart synchronization.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        resetButton->setText(QCoreApplication::translate("configWidget", "reset", nullptr));
#if QT_CONFIG(tooltip)
        fontButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Select one of a few fonts for the display of the service names. Will be effective the next program invocation.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        fontButton->setText(QCoreApplication::translate("configWidget", "font", nullptr));
#if QT_CONFIG(tooltip)
        snrButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>The snrViewer button controls a widget that - when enabled - will show the development of SNR over time. Parameters of the screen, height (the Y-axis) and the number of measurements (X-axis) can be set in the configuration widget.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        snrButton->setText(QCoreApplication::translate("configWidget", "snr viewer", nullptr));
#if QT_CONFIG(tooltip)
        set_coordinatesButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head /><body><p>\n"
"Touching this button will show (or hide) the resulting correlation of the first<br />\n"
" datablock of the DAB ensemble with the values as they should be. <br /><br />\n"
"The \"peaks\" (if any) show the samples where the correlation is strongest. <br />\n"
"More than one peak shows more than one transmitter within\n"
"the reach <br />of this receiver.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        set_coordinatesButton->setText(QCoreApplication::translate("configWidget", "coordinates", nullptr));
#if QT_CONFIG(tooltip)
        loadTableButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>show or hide the widget for showing the tii spectrum</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        loadTableButton->setText(QCoreApplication::translate("configWidget", "load table", nullptr));
#if QT_CONFIG(tooltip)
        dumpButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Push this button to save the raw input. Pushing will cause a menu to appear where a filename can be selected. Please note the big filesizes!</p><p>Push again to stop recording. You can reload it by using the file input (*.sdr) option. </p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        dumpButton->setText(QCoreApplication::translate("configWidget", "Raw dump", nullptr));
#if QT_CONFIG(tooltip)
        skinButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Select one of a few &quot;skin&quot;s for the widgets in the program. Will be effective the next program invocayion.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        skinButton->setText(QCoreApplication::translate("configWidget", "skin", nullptr));
#if QT_CONFIG(tooltip)
        saveServiceSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>The &quot;save service on exit&quot;  selector instructs the software -when set - to save the channel:service combination (audio services only) and on program restart automatically switch to the  channel and start the service.</p><p>Note that is the service cannot be found, a warning will be shown.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        saveServiceSelector->setText(QCoreApplication::translate("configWidget", "    save service on exit", nullptr));
#if QT_CONFIG(tooltip)
        loggerButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Sometime one wants to see what was done, the logger just dumps some data into a file, showing the selections made.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        loggerButton->setText(QCoreApplication::translate("configWidget", "  logger", nullptr));
#if QT_CONFIG(tooltip)
        epg2xmlSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>The EPG detector has two components, one collecting data to present as &quot;time table&quot; for today,</p><p>the other one - using a library written by Julian Cable from the BBC - to generate textual xml.</p><p>If selected, the xml will be generated. Note howevet that we encountered at least one example</p><p>where the library generating the textual xml causes a crash.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        epg2xmlSelector->setText(QCoreApplication::translate("configWidget", " epg to xml", nullptr));
#if QT_CONFIG(tooltip)
        tii_detectorMode->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>a second implementation of the tii decoder can be selected here. The decoder &quot;sees&quot; more tii data, but also more erroneous data.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        tii_detectorMode->setText(QCoreApplication::translate("configWidget", "    new tii detector", nullptr));
#if QT_CONFIG(tooltip)
        utcSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>If you want to see UTC in the logs select this one.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        utcSelector->setText(QCoreApplication::translate("configWidget", " utc Selector", nullptr));
#if QT_CONFIG(tooltip)
        onTop->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Set in the configuration whether or not the main widget should ALWAYS be op top. Note that setting will be applied the next time the program will start.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        onTop->setText(QCoreApplication::translate("configWidget", "main widget on top", nullptr));
#if QT_CONFIG(tooltip)
        closeDirect->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>If you are bored with the explicit request to stop, touch this one, and stopping is direct</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        closeDirect->setText(QCoreApplication::translate("configWidget", "  close without asking", nullptr));
#if QT_CONFIG(tooltip)
        epgSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>When set, the software is instructed to start an EPG handler when an EPG sevice is detected on starting an ensemble.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        epgSelector->setText(QCoreApplication::translate("configWidget", "epg automatic", nullptr));
#if QT_CONFIG(tooltip)
        autoBrowser->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Set in the configuration that on starting the http service a browser will have to be started manually. Default is browser starts automatically. Effective immediately</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        autoBrowser->setText(QCoreApplication::translate("configWidget", "start browser manually", nullptr));
        dcRemovalSelector->setText(QString());
#if QT_CONFIG(tooltip)
        eti_activeSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>eti files can be generated by Qt-DAB-5.x. Since generating an eti file from an ensemble seems incompatible with scanning, the scan button is replaced by an &quot;eti&quot; button when eti is activated.</p><p>Note that building up an eti file requires quite some resources.</p><p>If eti is activated, the scanButton is replaced by the etiButton. Touching that button shows a fileselection menu, after selecting a file the content of the ensemble is sent in eti format to the file. The eti session stops when (a) the eti button is touched again, (b) switching to another channel or (c) disabling eti by unchecking the checkbox.</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        eti_activeSelector->setText(QCoreApplication::translate("configWidget", "eti activated", nullptr));
#if QT_CONFIG(tooltip)
        transmitterTags->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Set in the configuration that - rather than showing all transmitters on a map - only the transmitters that are seen for the currrent channel are made visible.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        transmitterTags->setText(QCoreApplication::translate("configWidget", "current channel transmitters", nullptr));
        clearScan_Selector->setText(QCoreApplication::translate("configWidget", "clear scan", nullptr));
#if QT_CONFIG(tooltip)
        saveSlides->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>If set, slides (pictures) appearing during transmissions are being saved</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        saveSlides->setText(QCoreApplication::translate("configWidget", "save slides", nullptr));
#if QT_CONFIG(tooltip)
        transmSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>When this is set, the transmitter descriptions visible on the map are also stored in a &quot;.csv&quot;  file,</p><p>On starting the httpHandler, a file selection menu will appear.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        transmSelector->setText(QCoreApplication::translate("configWidget", "save transmitters", nullptr));
        scanmodeSelector->setItemText(0, QCoreApplication::translate("configWidget", "single scan", nullptr));
        scanmodeSelector->setItemText(1, QCoreApplication::translate("configWidget", "scan to data", nullptr));
        scanmodeSelector->setItemText(2, QCoreApplication::translate("configWidget", "continuous", nullptr));

#if QT_CONFIG(tooltip)
        scanmodeSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Choose one of three modes for scanning</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        skipList_button->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>show or hide the skipList. Updates in the skipList will be saved.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        skipList_button->setText(QCoreApplication::translate("configWidget", "skipList", nullptr));
#if QT_CONFIG(tooltip)
        skipFile_button->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Select a file for the skipList. If the file does not exist yet, it will be created and initialized with a skipList with no channels set to skip.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        skipFile_button->setText(QCoreApplication::translate("configWidget", "skipFile", nullptr));
        servicesInBackground->setText(QCoreApplication::translate("configWidget", "bg services", nullptr));
#if QT_CONFIG(tooltip)
        decoderSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Decoder selector. Just as an experiment three (almost similar) approaches to map de decoded carriers onto bits are implemented. The default one is based on the geometrical distance between the complex value (normalized to an amplitude 1, and considered as a dot in a plane) and the centerpoint in that plane. The other two use the offset of the &quot;arg&quot; of the value tot PI/4 (all seen in quadrant 1), the larger the offset, the smaller the soft bit value. </p><p>The difference between the two is that the first one takes the measured value of the current carrier value, while the second one looks at the average corner value of that carrier.</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class configWidget: public Ui_configWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIG_2D_HELPER_H
