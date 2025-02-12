/********************************************************************************
** Form generated from reading UI file 'config-helper.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
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
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <smallspinbox.h>
#include "smallcombobox.h"
#include "smallpushbutton.h"

QT_BEGIN_NAMESPACE

class Ui_configWidget
{
public:
    QVBoxLayout *verticalLayout_5;
    QSplitter *splitter;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *orderServiceIds;
    QRadioButton *orderAlfabetical;
    QRadioButton *ordersubChannelIds;
    smallPushButton *audioSelectButton;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    smallPushButton *fontButton;
    QSpinBox *fontSizeSelector;
    smallPushButton *fontColorButton;
    QHBoxLayout *horizontalLayout_4;
    smallPushButton *pathButton;
    QSpinBox *tiiThreshold_setter;
    QWidget *layoutWidget2;
    QVBoxLayout *verticalLayout_3;
    QFormLayout *formLayout;
    QLabel *label;
    smallSpinBox *muteTimeSetting;
    QLabel *label_2;
    smallSpinBox *switchDelaySetting;
    QHBoxLayout *horizontalLayout;
    QLabel *label_3;
    QLCDNumber *loadDisplay;
    QGridLayout *gridLayout;
    smallPushButton *loadTableButton;
    smallPushButton *devicewidgetButton;
    smallPushButton *portSelector;
    smallPushButton *dumpButton;
    smallPushButton *dlTextButton;
    smallPushButton *set_coordinatesButton;
    smallPushButton *scheduleButton;
    smallPushButton *resetButton;
    smallPushButton *skinButton;
    smallPushButton *snrButton;
    QGridLayout *gridLayout_2;
    QCheckBox *upload_selector;
    QCheckBox *utc_selector;
    QCheckBox *epg2xmlSelector;
    QCheckBox *closeDirect_selector;
    QCheckBox *epg_selector;
    QCheckBox *onTop;
    QCheckBox *correlationSelector;
    QCheckBox *etiActivated_selector;
    QCheckBox *localBrowserSelector;
    QCheckBox *clearScan_selector;
    QCheckBox *saveSlides;
    QCheckBox *audioServices_only;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *auto_http;
    QCheckBox *logger_selector;
    smallComboBox *decoderSelector;
    smallComboBox *streamoutSelector;
    smallComboBox *deviceSelector;
    QButtonGroup *buttonGroup;

    void setupUi(QWidget *configWidget)
    {
        if (configWidget->objectName().isEmpty())
            configWidget->setObjectName("configWidget");
        configWidget->resize(494, 364);
        verticalLayout_5 = new QVBoxLayout(configWidget);
        verticalLayout_5->setObjectName("verticalLayout_5");
        splitter = new QSplitter(configWidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Orientation::Horizontal);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName("layoutWidget");
        verticalLayout_4 = new QVBoxLayout(layoutWidget);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        orderServiceIds = new QRadioButton(layoutWidget);
        buttonGroup = new QButtonGroup(configWidget);
        buttonGroup->setObjectName("buttonGroup");
        buttonGroup->addButton(orderServiceIds);
        orderServiceIds->setObjectName("orderServiceIds");

        verticalLayout_2->addWidget(orderServiceIds);

        orderAlfabetical = new QRadioButton(layoutWidget);
        buttonGroup->addButton(orderAlfabetical);
        orderAlfabetical->setObjectName("orderAlfabetical");

        verticalLayout_2->addWidget(orderAlfabetical);

        ordersubChannelIds = new QRadioButton(layoutWidget);
        buttonGroup->addButton(ordersubChannelIds);
        ordersubChannelIds->setObjectName("ordersubChannelIds");

        verticalLayout_2->addWidget(ordersubChannelIds);


        verticalLayout_4->addLayout(verticalLayout_2);

        audioSelectButton = new smallPushButton(layoutWidget);
        audioSelectButton->setObjectName("audioSelectButton");

        verticalLayout_4->addWidget(audioSelectButton);

        splitter->addWidget(layoutWidget);
        layoutWidget1 = new QWidget(splitter);
        layoutWidget1->setObjectName("layoutWidget1");
        verticalLayout = new QVBoxLayout(layoutWidget1);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        fontButton = new smallPushButton(layoutWidget1);
        fontButton->setObjectName("fontButton");

        horizontalLayout_2->addWidget(fontButton);

        fontSizeSelector = new QSpinBox(layoutWidget1);
        fontSizeSelector->setObjectName("fontSizeSelector");
        fontSizeSelector->setMinimum(8);
        fontSizeSelector->setMaximum(15);
        fontSizeSelector->setValue(10);

        horizontalLayout_2->addWidget(fontSizeSelector);


        verticalLayout->addLayout(horizontalLayout_2);

        fontColorButton = new smallPushButton(layoutWidget1);
        fontColorButton->setObjectName("fontColorButton");

        verticalLayout->addWidget(fontColorButton);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        pathButton = new smallPushButton(layoutWidget1);
        pathButton->setObjectName("pathButton");

        horizontalLayout_4->addWidget(pathButton);

        tiiThreshold_setter = new QSpinBox(layoutWidget1);
        tiiThreshold_setter->setObjectName("tiiThreshold_setter");
        tiiThreshold_setter->setMinimum(6);
        tiiThreshold_setter->setValue(6);

        horizontalLayout_4->addWidget(tiiThreshold_setter);


        verticalLayout->addLayout(horizontalLayout_4);

        splitter->addWidget(layoutWidget1);
        layoutWidget2 = new QWidget(splitter);
        layoutWidget2->setObjectName("layoutWidget2");
        verticalLayout_3 = new QVBoxLayout(layoutWidget2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label = new QLabel(layoutWidget2);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        muteTimeSetting = new smallSpinBox(layoutWidget2);
        muteTimeSetting->setObjectName("muteTimeSetting");

        formLayout->setWidget(0, QFormLayout::FieldRole, muteTimeSetting);

        label_2 = new QLabel(layoutWidget2);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        switchDelaySetting = new smallSpinBox(layoutWidget2);
        switchDelaySetting->setObjectName("switchDelaySetting");

        formLayout->setWidget(1, QFormLayout::FieldRole, switchDelaySetting);


        verticalLayout_3->addLayout(formLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_3 = new QLabel(layoutWidget2);
        label_3->setObjectName("label_3");

        horizontalLayout->addWidget(label_3);

        loadDisplay = new QLCDNumber(layoutWidget2);
        loadDisplay->setObjectName("loadDisplay");
        loadDisplay->setFrameShape(QFrame::Shape::NoFrame);
        loadDisplay->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        horizontalLayout->addWidget(loadDisplay);


        verticalLayout_3->addLayout(horizontalLayout);

        splitter->addWidget(layoutWidget2);

        verticalLayout_5->addWidget(splitter);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        loadTableButton = new smallPushButton(configWidget);
        loadTableButton->setObjectName("loadTableButton");
        QFont font;
        font.setPointSize(10);
        loadTableButton->setFont(font);

        gridLayout->addWidget(loadTableButton, 2, 3, 1, 2);

        devicewidgetButton = new smallPushButton(configWidget);
        devicewidgetButton->setObjectName("devicewidgetButton");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(devicewidgetButton->sizePolicy().hasHeightForWidth());
        devicewidgetButton->setSizePolicy(sizePolicy);
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Cantarell")});
        font1.setPointSize(10);
        devicewidgetButton->setFont(font1);

        gridLayout->addWidget(devicewidgetButton, 0, 0, 1, 1);

        portSelector = new smallPushButton(configWidget);
        portSelector->setObjectName("portSelector");
        QFont font2;
        font2.setPointSize(12);
        portSelector->setFont(font2);

        gridLayout->addWidget(portSelector, 0, 1, 1, 1);

        dumpButton = new smallPushButton(configWidget);
        dumpButton->setObjectName("dumpButton");
        sizePolicy.setHeightForWidth(dumpButton->sizePolicy().hasHeightForWidth());
        dumpButton->setSizePolicy(sizePolicy);
        dumpButton->setFont(font);

        gridLayout->addWidget(dumpButton, 2, 5, 1, 1);

        dlTextButton = new smallPushButton(configWidget);
        dlTextButton->setObjectName("dlTextButton");
        dlTextButton->setFont(font);

        gridLayout->addWidget(dlTextButton, 0, 2, 1, 2);

        set_coordinatesButton = new smallPushButton(configWidget);
        set_coordinatesButton->setObjectName("set_coordinatesButton");
        sizePolicy.setHeightForWidth(set_coordinatesButton->sizePolicy().hasHeightForWidth());
        set_coordinatesButton->setSizePolicy(sizePolicy);
        set_coordinatesButton->setFont(font1);

        gridLayout->addWidget(set_coordinatesButton, 2, 1, 1, 2);

        scheduleButton = new smallPushButton(configWidget);
        scheduleButton->setObjectName("scheduleButton");

        gridLayout->addWidget(scheduleButton, 0, 6, 1, 1);

        resetButton = new smallPushButton(configWidget);
        resetButton->setObjectName("resetButton");
        sizePolicy.setHeightForWidth(resetButton->sizePolicy().hasHeightForWidth());
        resetButton->setSizePolicy(sizePolicy);
        resetButton->setFont(font);

        gridLayout->addWidget(resetButton, 0, 4, 1, 2);

        skinButton = new smallPushButton(configWidget);
        skinButton->setObjectName("skinButton");

        gridLayout->addWidget(skinButton, 2, 6, 1, 1);

        snrButton = new smallPushButton(configWidget);
        snrButton->setObjectName("snrButton");
        sizePolicy.setHeightForWidth(snrButton->sizePolicy().hasHeightForWidth());
        snrButton->setSizePolicy(sizePolicy);
        snrButton->setFont(font1);

        gridLayout->addWidget(snrButton, 2, 0, 1, 1);


        verticalLayout_5->addLayout(gridLayout);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName("gridLayout_2");
        upload_selector = new QCheckBox(configWidget);
        upload_selector->setObjectName("upload_selector");

        gridLayout_2->addWidget(upload_selector, 0, 0, 1, 1);

        utc_selector = new QCheckBox(configWidget);
        utc_selector->setObjectName("utc_selector");

        gridLayout_2->addWidget(utc_selector, 0, 1, 1, 1);

        epg2xmlSelector = new QCheckBox(configWidget);
        epg2xmlSelector->setObjectName("epg2xmlSelector");

        gridLayout_2->addWidget(epg2xmlSelector, 0, 2, 1, 1);

        closeDirect_selector = new QCheckBox(configWidget);
        closeDirect_selector->setObjectName("closeDirect_selector");

        gridLayout_2->addWidget(closeDirect_selector, 1, 0, 1, 1);

        epg_selector = new QCheckBox(configWidget);
        epg_selector->setObjectName("epg_selector");

        gridLayout_2->addWidget(epg_selector, 1, 1, 1, 1);

        onTop = new QCheckBox(configWidget);
        onTop->setObjectName("onTop");

        gridLayout_2->addWidget(onTop, 1, 2, 1, 1);

        correlationSelector = new QCheckBox(configWidget);
        correlationSelector->setObjectName("correlationSelector");

        gridLayout_2->addWidget(correlationSelector, 2, 0, 1, 1);

        etiActivated_selector = new QCheckBox(configWidget);
        etiActivated_selector->setObjectName("etiActivated_selector");

        gridLayout_2->addWidget(etiActivated_selector, 2, 1, 1, 1);

        localBrowserSelector = new QCheckBox(configWidget);
        localBrowserSelector->setObjectName("localBrowserSelector");

        gridLayout_2->addWidget(localBrowserSelector, 2, 2, 1, 1);

        clearScan_selector = new QCheckBox(configWidget);
        clearScan_selector->setObjectName("clearScan_selector");

        gridLayout_2->addWidget(clearScan_selector, 3, 0, 1, 1);

        saveSlides = new QCheckBox(configWidget);
        saveSlides->setObjectName("saveSlides");

        gridLayout_2->addWidget(saveSlides, 3, 1, 1, 1);

        audioServices_only = new QCheckBox(configWidget);
        audioServices_only->setObjectName("audioServices_only");

        gridLayout_2->addWidget(audioServices_only, 3, 2, 1, 1);


        verticalLayout_5->addLayout(gridLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        auto_http = new QCheckBox(configWidget);
        auto_http->setObjectName("auto_http");

        horizontalLayout_3->addWidget(auto_http);

        logger_selector = new QCheckBox(configWidget);
        logger_selector->setObjectName("logger_selector");

        horizontalLayout_3->addWidget(logger_selector);

        decoderSelector = new smallComboBox(configWidget);
        decoderSelector->setObjectName("decoderSelector");

        horizontalLayout_3->addWidget(decoderSelector);

        streamoutSelector = new smallComboBox(configWidget);
        streamoutSelector->setObjectName("streamoutSelector");
        streamoutSelector->setMaximumSize(QSize(162, 16777215));

        horizontalLayout_3->addWidget(streamoutSelector);

        deviceSelector = new smallComboBox(configWidget);
        deviceSelector->setObjectName("deviceSelector");

        horizontalLayout_3->addWidget(deviceSelector);


        verticalLayout_5->addLayout(horizontalLayout_3);


        retranslateUi(configWidget);

        QMetaObject::connectSlotsByName(configWidget);
    } // setupUi

    void retranslateUi(QWidget *configWidget)
    {
        configWidget->setWindowTitle(QCoreApplication::translate("configWidget", "Configuration and control", nullptr));
#if QT_CONFIG(tooltip)
        configWidget->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Configuration and control widget.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        orderServiceIds->setText(QCoreApplication::translate("configWidget", "serviceIds", nullptr));
        orderAlfabetical->setText(QCoreApplication::translate("configWidget", "alphabetically", nullptr));
        ordersubChannelIds->setText(QCoreApplication::translate("configWidget", "subchannelIds", nullptr));
#if QT_CONFIG(tooltip)
        audioSelectButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Choose between portaudio or Qt-audio as audio handler (will be effective next program invocation). Note that if no audio channels for the Qt_audio driver can be found, the default setting of portaudio is applied.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        audioSelectButton->setText(QCoreApplication::translate("configWidget", "audio", nullptr));
#if QT_CONFIG(tooltip)
        fontButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>The font for displaying the servicenames in the ensemble display can be set here.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        fontButton->setText(QCoreApplication::translate("configWidget", "font", nullptr));
#if QT_CONFIG(tooltip)
        fontSizeSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>The font size of the service lists (both the ensemble view and the favorites view) can be set.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        fontColorButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>The color used to display the servicenames in the ensemblewidget can be selected here.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        fontColorButton->setText(QCoreApplication::translate("configWidget", "font color", nullptr));
#if QT_CONFIG(tooltip)
        pathButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>The files containing slides - if being saved - will be saved in a folder/directory Qt-DAB-files in the home folder/directory. The tii files as well as the log file (both of caourse when enabled) are also stored in that folder/directory. The path can be altered by entering an EXISTING folder/directory in the menu that appears when the button is touched. That path will be effective the next program invocation.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        pathButton->setText(QCoreApplication::translate("configWidget", "path", nullptr));
#if QT_CONFIG(tooltip)
        tiiThreshold_setter->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>TII threshold. The TII threshold is used in the detection of TII values in the NULL periods of the DAB frames. Of course, the lower the value, the more chance there is to get false positives.</p><p>Default value is 4.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        tiiThreshold_setter->setSuffix(QCoreApplication::translate("configWidget", " dB", nullptr));
#if QT_CONFIG(tooltip)
        label->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>The mute time is the time, the audio will be suppressed when activating the mute function.</p><p>It is expressed in minutes.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label->setText(QCoreApplication::translate("configWidget", "mute time (in min)", nullptr));
#if QT_CONFIG(tooltip)
        muteTimeSetting->setToolTip(QCoreApplication::translate("configWidget", "Set here the mute time in minutes", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        label_2->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>The switch time - expressed in seconds - is the time the processor will wait to see if there is</p><p>any data in a channel. </p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_2->setText(QCoreApplication::translate("configWidget", "switch time (in sec)", nullptr));
#if QT_CONFIG(tooltip)
        switchDelaySetting->setToolTip(QCoreApplication::translate("configWidget", "Set here the delay, when searching for data in a channel, in seconds", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        label_3->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>The CPU load is an indication of the load when executing the program. The load tells the total load on the CPU, i.e. inclusing the load of other programs that might be running.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_3->setText(QCoreApplication::translate("configWidget", "CPU load", nullptr));
#if QT_CONFIG(tooltip)
        loadDisplay->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>The number tells the overall CPU load (so, not only the load caused by running Qt-DAB)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        loadTableButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Touching this button will instruct the software to load a fresh database. Note that this only works if either the software is precompiled or you have installed the small library as discussed in the installation instructions.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        loadTableButton->setText(QCoreApplication::translate("configWidget", "load table", nullptr));
#if QT_CONFIG(tooltip)
        devicewidgetButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>show or hide the widget for controlling the device.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        devicewidgetButton->setText(QCoreApplication::translate("configWidget", "device", nullptr));
#if QT_CONFIG(tooltip)
        portSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>touching this button allws you to select a port for communication with a webbrowser when showing a map.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        portSelector->setText(QCoreApplication::translate("configWidget", "port", nullptr));
#if QT_CONFIG(tooltip)
        dumpButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Push this button to save the raw input. Pushing will cause a menu to appear where a filename can be selected. Please note the big filesizes!</p><p>Push again to stop recording. You can reload it by using the file input (*.sdr) option. </p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        dumpButton->setText(QCoreApplication::translate("configWidget", "Raw dump", nullptr));
#if QT_CONFIG(tooltip)
        dlTextButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Save the lines of text that appear as dynamic label</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        dlTextButton->setText(QCoreApplication::translate("configWidget", "dlText", nullptr));
#if QT_CONFIG(tooltip)
        set_coordinatesButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Touching this button shows a small menu where you can fill in the coordinates (in decimal notation) of your location.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        set_coordinatesButton->setText(QCoreApplication::translate("configWidget", "coordinates", nullptr));
#if QT_CONFIG(tooltip)
        scheduleButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Touching this button allows specifying an element for the scheduler</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        scheduleButton->setText(QCoreApplication::translate("configWidget", "schedule", nullptr));
#if QT_CONFIG(tooltip)
        resetButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Push this button to reset, i.e. restart synchronization.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        resetButton->setText(QCoreApplication::translate("configWidget", "reset", nullptr));
#if QT_CONFIG(tooltip)
        skinButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Select one of a few &quot;skin&quot;s for the widgets in the program. Will be effective the next program invocation.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        skinButton->setText(QCoreApplication::translate("configWidget", "skin", nullptr));
#if QT_CONFIG(tooltip)
        snrButton->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>The snrViewer button controls a widget that - when enabled - will show the development of SNR over time. Parameters of the screen, height (the Y-axis) and the number of measurements (X-axis) can be set in the configuration widget.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        snrButton->setText(QCoreApplication::translate("configWidget", "snr viewer", nullptr));
#if QT_CONFIG(tooltip)
        upload_selector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>&quot;upload enable&quot; is a selector for - if configured - allowing some data to be uploaded to fmlist.org. When touching the ensemblename on top of the left part of the main widget, the current content of the ensemble is shown (in &quot;.csv&quot;  format).  If this selector is enabled - and if the feature is configured - you will be asked to accept (or deny) sending up the contents of the ensemble to &quot;fmlist.org&quot; (the site that is responsible for the database mapping the TII numbers to station names and station attributes.</p><p>The same for rh result of the single scan.</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        upload_selector->setText(QCoreApplication::translate("configWidget", "upload enable", nullptr));
#if QT_CONFIG(tooltip)
        utc_selector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>If you want to see UTC in the logs select this one.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        utc_selector->setText(QCoreApplication::translate("configWidget", " utc Selector", nullptr));
#if QT_CONFIG(tooltip)
        epg2xmlSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>The EPG detector has two components, one collecting data to present as &quot;time table&quot; for today,</p><p>the other one - using a library written by Julian Cable from the BBC - to generate textual xml.</p><p>If selected, the xml will be generated. Note howevet that we encountered at least one example</p><p>where the library generating the textual xml causes a crash.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        epg2xmlSelector->setText(QCoreApplication::translate("configWidget", " epg to xml", nullptr));
#if QT_CONFIG(tooltip)
        closeDirect_selector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>If you are bored with the explicit request to stop, touch this one, and stopping is direct</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        closeDirect_selector->setText(QCoreApplication::translate("configWidget", "close without asking", nullptr));
#if QT_CONFIG(tooltip)
        epg_selector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>When set, the software is instructed to start an EPG handler when an EPG sevice is detected on starting an ensemble.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        epg_selector->setText(QCoreApplication::translate("configWidget", "epg automatic", nullptr));
#if QT_CONFIG(tooltip)
        onTop->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Set in the configuration whether or not the main widget should ALWAYS be op top. Note that setting will be applied the next time the program will start.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        onTop->setText(QCoreApplication::translate("configWidget", "main widget on top", nullptr));
#if QT_CONFIG(tooltip)
        correlationSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Synchronization of the receiver software with the incoming samplestream is dome using correlation. As the scope on the displaywidget often shows is that there is more than a single peak in the correlation. Normally, the strongest peak is used, however, sometimes the difference in stength between peaks is so small, that the &quot;strongest&quot; one changes quite often.</p><p>Selecting this ensures that the software takes the first of the peaks that meers some strength criteria</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        correlationSelector->setText(QCoreApplication::translate("configWidget", "first correlation", nullptr));
#if QT_CONFIG(tooltip)
        etiActivated_selector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>eti files can be generated by Qt-DAB-5.x. Since generating an eti file from an ensemble seems incompatible with scanning, the scan button is replaced by an &quot;eti&quot; button when eti is activated.</p><p>Note that building up an eti file requires quite some resources.</p><p>If eti is activated, the scanButton is replaced by the etiButton. Touching that button shows a fileselection menu, after selecting a file the content of the ensemble is sent in eti format to the file. The eti session stops when (a) the eti button is touched again, (b) switching to another channel or (c) disabling eti by unchecking the checkbox.</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        etiActivated_selector->setText(QCoreApplication::translate("configWidget", "eti activated", nullptr));
#if QT_CONFIG(tooltip)
        localBrowserSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Set in the configuration that on starting the http service a browser will have to be started manually. Default is browser starts automatically. Effective immediately</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        localBrowserSelector->setText(QCoreApplication::translate("configWidget", "start browser manually", nullptr));
#if QT_CONFIG(tooltip)
        clearScan_selector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Setting the selector has effect when scanning. If set, the scan table is cleared whenever a</p><p>new scan (mode &quot;scan once&quot;) is started.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        clearScan_selector->setText(QCoreApplication::translate("configWidget", "clear scan", nullptr));
#if QT_CONFIG(tooltip)
        saveSlides->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>If set, slides (pictures) appearing during transmissions are being saved</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        saveSlides->setText(QCoreApplication::translate("configWidget", "save slides", nullptr));
#if QT_CONFIG(tooltip)
        audioServices_only->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Many ensembles contain both audio and data services (e.g. TPEG, EPG). If selected here, only audio services will be shown in the services list</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        audioServices_only->setText(QCoreApplication::translate("configWidget", "audio services only", nullptr));
#if QT_CONFIG(tooltip)
        auto_http->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>If this checkbox is set, then Qt-DAB will (try to) start up the http handler (i.e. the map handler) on the next invocation of the program.</p><p>Of course, it will only work if</p><p>(a) the local position is filled in, and </p><p>(b) the database with TII data is accessible</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        auto_http->setText(QCoreApplication::translate("configWidget", "auto http", nullptr));
#if QT_CONFIG(tooltip)
        logger_selector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>If set, a log file is maintained (as usual for windows in the folder in the home folder and for Linux in the directory /tmp in a folder/directory Qt-DAB-files) in a txt file :logFile.txt, in which activities such as selecting a device, channel, service, dump are written, preceded by the time of the activity.</p><p>The setting of the selector is maintained between program invocations.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        logger_selector->setText(QCoreApplication::translate("configWidget", "  logger", nullptr));
#if QT_CONFIG(tooltip)
        decoderSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Decoding DAB means interpreting the output of the DPSK on the FFT output, the latter meaning taking the phase difference between corresponding carriers in subsequent FFT symbols.</p><p>Ideally these phases are a multiple of 90 degrees, in reality they are less precise. The decoders here all try to map the results of the previous phase onto &quot;soft bits&quot;, i.e. values between -127 and 127. Each &quot;dot&quot; is mapped onto two bits, one depending on the X coordinate, the other on the &quot;Y&quot; one</p><p>The differences between the decoders is the way they look at the way the measured values differ from the ideal ones. In general, the most simple one, here the &quot;decoder_a&quot;, gives the best results.</p><p>The quality can be seen by looking at the fic BER value, that shows the percentage of the input soft bits in FIC processing that are wrong (and needed to be repaired by the viterbi decoder)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        streamoutSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Select the audio channel, depends on the OS you are running on</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        deviceSelector->setToolTip(QCoreApplication::translate("configWidget", "<html><head/><body><p>Select a device.  Devices appearing in the list here are configured in the Qt-DAB software. Note that the DAB software tries to load the required device interface functions dynamically, so a device appearing in this list is not automatically supported, you need to have installed a device driver</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class configWidget: public Ui_configWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIG_2D_HELPER_H
