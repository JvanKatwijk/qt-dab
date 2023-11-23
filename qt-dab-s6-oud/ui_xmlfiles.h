/********************************************************************************
** Form generated from reading UI file 'xmlfiles.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XMLFILES_H
#define UI_XMLFILES_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_xmlfile_widget
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *filenameLabel;
    QProgressBar *fileProgress;
    QPushButton *continuousButton;
    QHBoxLayout *horizontalLayout;
    QLCDNumber *currentTime;
    QLCDNumber *totalTime;
    QGridLayout *gridLayout;
    QLabel *recorderName;
    QLabel *recorderVersion;
    QLabel *deviceVersion;
    QLabel *deviceModel;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QLabel *recordingTime;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_4;
    QLabel *modulationtypeLabel;
    QLabel *iqOrderLabel;
    QLabel *containerLabel;
    QLabel *byteOrderLabel;
    QLabel *typeofUnitLabel;
    QVBoxLayout *verticalLayout_3;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QVBoxLayout *verticalLayout_2;
    QLCDNumber *nrBitsDisplay;
    QLCDNumber *nrElementsDisplay;
    QLCDNumber *samplerateDisplay;
    QLCDNumber *frequencyDisplay;

    void setupUi(QWidget *xmlfile_widget)
    {
        if (xmlfile_widget->objectName().isEmpty())
            xmlfile_widget->setObjectName(QString::fromUtf8("xmlfile_widget"));
        xmlfile_widget->resize(296, 364);
        verticalLayout = new QVBoxLayout(xmlfile_widget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        filenameLabel = new QLabel(xmlfile_widget);
        filenameLabel->setObjectName(QString::fromUtf8("filenameLabel"));

        verticalLayout->addWidget(filenameLabel);

        fileProgress = new QProgressBar(xmlfile_widget);
        fileProgress->setObjectName(QString::fromUtf8("fileProgress"));
        fileProgress->setValue(24);

        verticalLayout->addWidget(fileProgress);

        continuousButton = new QPushButton(xmlfile_widget);
        continuousButton->setObjectName(QString::fromUtf8("continuousButton"));

        verticalLayout->addWidget(continuousButton);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        currentTime = new QLCDNumber(xmlfile_widget);
        currentTime->setObjectName(QString::fromUtf8("currentTime"));
        currentTime->setSegmentStyle(QLCDNumber::Flat);

        horizontalLayout->addWidget(currentTime);

        totalTime = new QLCDNumber(xmlfile_widget);
        totalTime->setObjectName(QString::fromUtf8("totalTime"));
        totalTime->setSegmentStyle(QLCDNumber::Flat);

        horizontalLayout->addWidget(totalTime);


        verticalLayout->addLayout(horizontalLayout);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        recorderName = new QLabel(xmlfile_widget);
        recorderName->setObjectName(QString::fromUtf8("recorderName"));

        gridLayout->addWidget(recorderName, 0, 0, 1, 1);

        recorderVersion = new QLabel(xmlfile_widget);
        recorderVersion->setObjectName(QString::fromUtf8("recorderVersion"));

        gridLayout->addWidget(recorderVersion, 0, 1, 1, 1);

        deviceVersion = new QLabel(xmlfile_widget);
        deviceVersion->setObjectName(QString::fromUtf8("deviceVersion"));

        gridLayout->addWidget(deviceVersion, 1, 0, 1, 1);

        deviceModel = new QLabel(xmlfile_widget);
        deviceModel->setObjectName(QString::fromUtf8("deviceModel"));

        gridLayout->addWidget(deviceModel, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_5 = new QLabel(xmlfile_widget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_3->addWidget(label_5);

        recordingTime = new QLabel(xmlfile_widget);
        recordingTime->setObjectName(QString::fromUtf8("recordingTime"));

        horizontalLayout_3->addWidget(recordingTime);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        modulationtypeLabel = new QLabel(xmlfile_widget);
        modulationtypeLabel->setObjectName(QString::fromUtf8("modulationtypeLabel"));

        verticalLayout_4->addWidget(modulationtypeLabel);

        iqOrderLabel = new QLabel(xmlfile_widget);
        iqOrderLabel->setObjectName(QString::fromUtf8("iqOrderLabel"));

        verticalLayout_4->addWidget(iqOrderLabel);

        containerLabel = new QLabel(xmlfile_widget);
        containerLabel->setObjectName(QString::fromUtf8("containerLabel"));

        verticalLayout_4->addWidget(containerLabel);

        byteOrderLabel = new QLabel(xmlfile_widget);
        byteOrderLabel->setObjectName(QString::fromUtf8("byteOrderLabel"));

        verticalLayout_4->addWidget(byteOrderLabel);

        typeofUnitLabel = new QLabel(xmlfile_widget);
        typeofUnitLabel->setObjectName(QString::fromUtf8("typeofUnitLabel"));

        verticalLayout_4->addWidget(typeofUnitLabel);


        horizontalLayout_2->addLayout(verticalLayout_4);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label = new QLabel(xmlfile_widget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_3->addWidget(label);

        label_2 = new QLabel(xmlfile_widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_3->addWidget(label_2);

        label_3 = new QLabel(xmlfile_widget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_3->addWidget(label_3);

        label_4 = new QLabel(xmlfile_widget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout_3->addWidget(label_4);


        horizontalLayout_2->addLayout(verticalLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        nrBitsDisplay = new QLCDNumber(xmlfile_widget);
        nrBitsDisplay->setObjectName(QString::fromUtf8("nrBitsDisplay"));
        nrBitsDisplay->setSegmentStyle(QLCDNumber::Flat);

        verticalLayout_2->addWidget(nrBitsDisplay);

        nrElementsDisplay = new QLCDNumber(xmlfile_widget);
        nrElementsDisplay->setObjectName(QString::fromUtf8("nrElementsDisplay"));
        nrElementsDisplay->setDigitCount(10);
        nrElementsDisplay->setSegmentStyle(QLCDNumber::Flat);

        verticalLayout_2->addWidget(nrElementsDisplay);

        samplerateDisplay = new QLCDNumber(xmlfile_widget);
        samplerateDisplay->setObjectName(QString::fromUtf8("samplerateDisplay"));
        samplerateDisplay->setDigitCount(8);
        samplerateDisplay->setSegmentStyle(QLCDNumber::Flat);

        verticalLayout_2->addWidget(samplerateDisplay);

        frequencyDisplay = new QLCDNumber(xmlfile_widget);
        frequencyDisplay->setObjectName(QString::fromUtf8("frequencyDisplay"));
        frequencyDisplay->setDigitCount(8);
        frequencyDisplay->setSegmentStyle(QLCDNumber::Flat);

        verticalLayout_2->addWidget(frequencyDisplay);


        horizontalLayout_2->addLayout(verticalLayout_2);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(xmlfile_widget);

        QMetaObject::connectSlotsByName(xmlfile_widget);
    } // setupUi

    void retranslateUi(QWidget *xmlfile_widget)
    {
        xmlfile_widget->setWindowTitle(QCoreApplication::translate("xmlfile_widget", "xmlfiles", nullptr));
#if QT_CONFIG(tooltip)
        filenameLabel->setToolTip(QCoreApplication::translate("xmlfile_widget", "<html><head/><body><p>Path and filename to xml-file</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        filenameLabel->setText(QCoreApplication::translate("xmlfile_widget", "TextLabel", nullptr));
#if QT_CONFIG(tooltip)
        fileProgress->setToolTip(QCoreApplication::translate("xmlfile_widget", "<html><head/><body><p>File progress</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        continuousButton->setText(QCoreApplication::translate("xmlfile_widget", "continuous off", nullptr));
#if QT_CONFIG(tooltip)
        currentTime->setToolTip(QCoreApplication::translate("xmlfile_widget", "<html><head/><body><p>Current time in the file</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        totalTime->setToolTip(QCoreApplication::translate("xmlfile_widget", "<html><head/><body><p>Total recorded time of the file</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        recorderName->setText(QCoreApplication::translate("xmlfile_widget", "recorder", nullptr));
#if QT_CONFIG(tooltip)
        recorderVersion->setToolTip(QCoreApplication::translate("xmlfile_widget", "<html><head/><body><p>Shows recording software and version.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        recorderVersion->setText(QCoreApplication::translate("xmlfile_widget", "TextLabel", nullptr));
        deviceVersion->setText(QCoreApplication::translate("xmlfile_widget", "deviceName", nullptr));
#if QT_CONFIG(tooltip)
        deviceModel->setToolTip(QCoreApplication::translate("xmlfile_widget", "<html><head/><body><p>Shows recording device</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        deviceModel->setText(QCoreApplication::translate("xmlfile_widget", "TextLabel", nullptr));
        label_5->setText(QCoreApplication::translate("xmlfile_widget", "recorded at ", nullptr));
#if QT_CONFIG(tooltip)
        recordingTime->setToolTip(QCoreApplication::translate("xmlfile_widget", "<html><head/><body><p>Shows recording time (it's not the current time)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        recordingTime->setText(QCoreApplication::translate("xmlfile_widget", "TextLabel", nullptr));
        modulationtypeLabel->setText(QCoreApplication::translate("xmlfile_widget", "TextLabel", nullptr));
        iqOrderLabel->setText(QCoreApplication::translate("xmlfile_widget", "TextLabel", nullptr));
        containerLabel->setText(QCoreApplication::translate("xmlfile_widget", "TextLabel", nullptr));
        byteOrderLabel->setText(QCoreApplication::translate("xmlfile_widget", "TextLabel", nullptr));
        typeofUnitLabel->setText(QCoreApplication::translate("xmlfile_widget", "TextLabel", nullptr));
        label->setText(QCoreApplication::translate("xmlfile_widget", "nrBits", nullptr));
        label_2->setText(QCoreApplication::translate("xmlfile_widget", "nrElements", nullptr));
        label_3->setText(QCoreApplication::translate("xmlfile_widget", "samplerate", nullptr));
        label_4->setText(QCoreApplication::translate("xmlfile_widget", "frequency", nullptr));
#if QT_CONFIG(tooltip)
        nrBitsDisplay->setToolTip(QCoreApplication::translate("xmlfile_widget", "<html><head/><body><p>Number of bits recorded</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        samplerateDisplay->setToolTip(QCoreApplication::translate("xmlfile_widget", "<html><head/><body><p>Sampling rate in Hz (normally 2048000 Hz)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        frequencyDisplay->setToolTip(QCoreApplication::translate("xmlfile_widget", "<html><head/><body><p>Initial frequency of the recorded ensemble (in kHz)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class xmlfile_widget: public Ui_xmlfile_widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XMLFILES_H
