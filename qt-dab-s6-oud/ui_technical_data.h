/********************************************************************************
** Form generated from reading UI file 'technical_data.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TECHNICAL_DATA_H
#define UI_TECHNICAL_DATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qwt_plot.h"
#include "qwt_thermo.h"
#include "smallpushbutton.h"

QT_BEGIN_NAMESPACE

class Ui_technical_data
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_2;
    QLabel *programName;
    QPushButton *timeTable_button;
    QHBoxLayout *horizontalLayout_4;
    smallPushButton *framedumpButton;
    smallPushButton *audiodumpButton;
    QFormLayout *formLayout;
    QLabel *label_3;
    QLCDNumber *serviceIdDisplay;
    QLabel *label;
    QLCDNumber *startAddressDisplay;
    QLabel *label_2;
    QLCDNumber *lengthDisplay;
    QLabel *label_6;
    QLCDNumber *subChIdDisplay;
    QLabel *l6;
    QLCDNumber *bitrateDisplay;
    QLabel *label_4;
    QLCDNumber *rsCorrections;
    QLabel *label_11;
    QLCDNumber *ecCorrections;
    QLabel *missedLabel;
    QLCDNumber *missedSamples;
    QGridLayout *gridLayout_2;
    QLabel *label_5;
    QLabel *uepField;
    QLabel *label_12;
    QLabel *codeRate;
    QLabel *label_8;
    QLabel *ASCTy;
    QLabel *label_9;
    QLabel *language;
    QHBoxLayout *horizontalLayout_3;
    QLabel *fmLabel;
    QLabel *fmFrequency;
    QGridLayout *gridLayout;
    QLabel *aacErrorLabel;
    QProgressBar *aacError_display;
    QProgressBar *frameError_display;
    QLabel *frameErrorLabel;
    QProgressBar *rsError_display;
    QLabel *rsErrorLabel;
    QVBoxLayout *verticalLayout;
    QwtThermo *thermoLeft;
    QwtThermo *thermoRight;
    QLabel *motAvailable;
    QHBoxLayout *horizontalLayout;
    QLabel *psLabel;
    QLabel *sbrLabel;
    QLCDNumber *audioRate;
    QwtPlot *audio;

    void setupUi(QWidget *technical_data)
    {
        if (technical_data->objectName().isEmpty())
            technical_data->setObjectName(QString::fromUtf8("technical_data"));
        technical_data->resize(303, 897);
        verticalLayout_2 = new QVBoxLayout(technical_data);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        programName = new QLabel(technical_data);
        programName->setObjectName(QString::fromUtf8("programName"));
        QFont font;
        font.setPointSize(14);
        programName->setFont(font);

        horizontalLayout_2->addWidget(programName);


        horizontalLayout_5->addLayout(horizontalLayout_2);

        timeTable_button = new QPushButton(technical_data);
        timeTable_button->setObjectName(QString::fromUtf8("timeTable_button"));

        horizontalLayout_5->addWidget(timeTable_button);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        framedumpButton = new smallPushButton(technical_data);
        framedumpButton->setObjectName(QString::fromUtf8("framedumpButton"));

        horizontalLayout_4->addWidget(framedumpButton);

        audiodumpButton = new smallPushButton(technical_data);
        audiodumpButton->setObjectName(QString::fromUtf8("audiodumpButton"));

        horizontalLayout_4->addWidget(audiodumpButton);


        verticalLayout_2->addLayout(horizontalLayout_4);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_3 = new QLabel(technical_data);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_3);

        serviceIdDisplay = new QLCDNumber(technical_data);
        serviceIdDisplay->setObjectName(QString::fromUtf8("serviceIdDisplay"));
        serviceIdDisplay->setFrameShape(QFrame::NoFrame);
        serviceIdDisplay->setMode(QLCDNumber::Hex);
        serviceIdDisplay->setSegmentStyle(QLCDNumber::Flat);

        formLayout->setWidget(0, QFormLayout::FieldRole, serviceIdDisplay);

        label = new QLabel(technical_data);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        startAddressDisplay = new QLCDNumber(technical_data);
        startAddressDisplay->setObjectName(QString::fromUtf8("startAddressDisplay"));
        startAddressDisplay->setFrameShape(QFrame::NoFrame);
        startAddressDisplay->setSegmentStyle(QLCDNumber::Flat);

        formLayout->setWidget(1, QFormLayout::FieldRole, startAddressDisplay);

        label_2 = new QLabel(technical_data);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        lengthDisplay = new QLCDNumber(technical_data);
        lengthDisplay->setObjectName(QString::fromUtf8("lengthDisplay"));
        lengthDisplay->setFrameShape(QFrame::NoFrame);
        lengthDisplay->setSegmentStyle(QLCDNumber::Flat);

        formLayout->setWidget(2, QFormLayout::FieldRole, lengthDisplay);

        label_6 = new QLabel(technical_data);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_6);

        subChIdDisplay = new QLCDNumber(technical_data);
        subChIdDisplay->setObjectName(QString::fromUtf8("subChIdDisplay"));
        subChIdDisplay->setFrameShape(QFrame::NoFrame);
        subChIdDisplay->setSegmentStyle(QLCDNumber::Flat);

        formLayout->setWidget(3, QFormLayout::FieldRole, subChIdDisplay);

        l6 = new QLabel(technical_data);
        l6->setObjectName(QString::fromUtf8("l6"));

        formLayout->setWidget(4, QFormLayout::LabelRole, l6);

        bitrateDisplay = new QLCDNumber(technical_data);
        bitrateDisplay->setObjectName(QString::fromUtf8("bitrateDisplay"));
        bitrateDisplay->setFrameShape(QFrame::NoFrame);
        bitrateDisplay->setSegmentStyle(QLCDNumber::Flat);

        formLayout->setWidget(4, QFormLayout::FieldRole, bitrateDisplay);

        label_4 = new QLabel(technical_data);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_4);

        rsCorrections = new QLCDNumber(technical_data);
        rsCorrections->setObjectName(QString::fromUtf8("rsCorrections"));
        rsCorrections->setFrameShape(QFrame::NoFrame);
        rsCorrections->setSegmentStyle(QLCDNumber::Flat);

        formLayout->setWidget(5, QFormLayout::FieldRole, rsCorrections);

        label_11 = new QLabel(technical_data);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_11);

        ecCorrections = new QLCDNumber(technical_data);
        ecCorrections->setObjectName(QString::fromUtf8("ecCorrections"));
        ecCorrections->setFrameShape(QFrame::NoFrame);
        ecCorrections->setSegmentStyle(QLCDNumber::Flat);

        formLayout->setWidget(6, QFormLayout::FieldRole, ecCorrections);

        missedLabel = new QLabel(technical_data);
        missedLabel->setObjectName(QString::fromUtf8("missedLabel"));

        formLayout->setWidget(7, QFormLayout::LabelRole, missedLabel);

        missedSamples = new QLCDNumber(technical_data);
        missedSamples->setObjectName(QString::fromUtf8("missedSamples"));
        missedSamples->setFrameShape(QFrame::NoFrame);
        missedSamples->setSegmentStyle(QLCDNumber::Flat);

        formLayout->setWidget(7, QFormLayout::FieldRole, missedSamples);


        verticalLayout_2->addLayout(formLayout);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_5 = new QLabel(technical_data);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMinimumSize(QSize(0, 17));

        gridLayout_2->addWidget(label_5, 0, 0, 1, 1);

        uepField = new QLabel(technical_data);
        uepField->setObjectName(QString::fromUtf8("uepField"));
        uepField->setMinimumSize(QSize(0, 20));
        uepField->setFrameShape(QFrame::NoFrame);

        gridLayout_2->addWidget(uepField, 0, 1, 1, 1);

        label_12 = new QLabel(technical_data);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout_2->addWidget(label_12, 1, 0, 1, 1);

        codeRate = new QLabel(technical_data);
        codeRate->setObjectName(QString::fromUtf8("codeRate"));

        gridLayout_2->addWidget(codeRate, 1, 1, 1, 1);

        label_8 = new QLabel(technical_data);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_2->addWidget(label_8, 2, 0, 1, 1);

        ASCTy = new QLabel(technical_data);
        ASCTy->setObjectName(QString::fromUtf8("ASCTy"));
        ASCTy->setFrameShape(QFrame::NoFrame);

        gridLayout_2->addWidget(ASCTy, 2, 1, 1, 1);

        label_9 = new QLabel(technical_data);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout_2->addWidget(label_9, 3, 0, 1, 1);

        language = new QLabel(technical_data);
        language->setObjectName(QString::fromUtf8("language"));
        language->setFrameShape(QFrame::NoFrame);

        gridLayout_2->addWidget(language, 3, 1, 1, 1);


        verticalLayout_2->addLayout(gridLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        fmLabel = new QLabel(technical_data);
        fmLabel->setObjectName(QString::fromUtf8("fmLabel"));

        horizontalLayout_3->addWidget(fmLabel);

        fmFrequency = new QLabel(technical_data);
        fmFrequency->setObjectName(QString::fromUtf8("fmFrequency"));

        horizontalLayout_3->addWidget(fmFrequency);


        verticalLayout_2->addLayout(horizontalLayout_3);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        aacErrorLabel = new QLabel(technical_data);
        aacErrorLabel->setObjectName(QString::fromUtf8("aacErrorLabel"));

        gridLayout->addWidget(aacErrorLabel, 2, 0, 1, 1);

        aacError_display = new QProgressBar(technical_data);
        aacError_display->setObjectName(QString::fromUtf8("aacError_display"));
        aacError_display->setValue(24);

        gridLayout->addWidget(aacError_display, 2, 2, 1, 1);

        frameError_display = new QProgressBar(technical_data);
        frameError_display->setObjectName(QString::fromUtf8("frameError_display"));
        frameError_display->setValue(24);

        gridLayout->addWidget(frameError_display, 0, 2, 1, 1);

        frameErrorLabel = new QLabel(technical_data);
        frameErrorLabel->setObjectName(QString::fromUtf8("frameErrorLabel"));

        gridLayout->addWidget(frameErrorLabel, 0, 0, 1, 2);

        rsError_display = new QProgressBar(technical_data);
        rsError_display->setObjectName(QString::fromUtf8("rsError_display"));
        rsError_display->setValue(24);

        gridLayout->addWidget(rsError_display, 1, 2, 1, 1);

        rsErrorLabel = new QLabel(technical_data);
        rsErrorLabel->setObjectName(QString::fromUtf8("rsErrorLabel"));

        gridLayout->addWidget(rsErrorLabel, 1, 0, 1, 2);


        verticalLayout_2->addLayout(gridLayout);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        thermoLeft = new QwtThermo(technical_data);
        thermoLeft->setObjectName(QString::fromUtf8("thermoLeft"));
        thermoLeft->setLowerBound(-20.000000000000000);
        thermoLeft->setUpperBound(4.000000000000000);
        thermoLeft->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(thermoLeft);

        thermoRight = new QwtThermo(technical_data);
        thermoRight->setObjectName(QString::fromUtf8("thermoRight"));
        thermoRight->setLowerBound(-20.000000000000000);
        thermoRight->setUpperBound(4.000000000000000);
        thermoRight->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(thermoRight);


        verticalLayout_2->addLayout(verticalLayout);

        motAvailable = new QLabel(technical_data);
        motAvailable->setObjectName(QString::fromUtf8("motAvailable"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(motAvailable->sizePolicy().hasHeightForWidth());
        motAvailable->setSizePolicy(sizePolicy);
        motAvailable->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(motAvailable);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        psLabel = new QLabel(technical_data);
        psLabel->setObjectName(QString::fromUtf8("psLabel"));

        horizontalLayout->addWidget(psLabel);

        sbrLabel = new QLabel(technical_data);
        sbrLabel->setObjectName(QString::fromUtf8("sbrLabel"));

        horizontalLayout->addWidget(sbrLabel);

        audioRate = new QLCDNumber(technical_data);
        audioRate->setObjectName(QString::fromUtf8("audioRate"));
        audioRate->setFrameShape(QFrame::NoFrame);
        audioRate->setFrameShadow(QFrame::Plain);
        audioRate->setSegmentStyle(QLCDNumber::Flat);

        horizontalLayout->addWidget(audioRate);


        verticalLayout_2->addLayout(horizontalLayout);

        audio = new QwtPlot(technical_data);
        audio->setObjectName(QString::fromUtf8("audio"));
        audio->setMaximumSize(QSize(291, 151));

        verticalLayout_2->addWidget(audio);


        retranslateUi(technical_data);

        QMetaObject::connectSlotsByName(technical_data);
    } // setupUi

    void retranslateUi(QWidget *technical_data)
    {
        technical_data->setWindowTitle(QCoreApplication::translate("technical_data", "Technical Details", nullptr));
#if QT_CONFIG(tooltip)
        programName->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p><span style=\" font-size:14pt;\">Service long label</span></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        programName->setText(QCoreApplication::translate("technical_data", "programName", nullptr));
#if QT_CONFIG(tooltip)
        timeTable_button->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>If configured - opens a new windows with SPI (formerly called EPG) preview.</p><p><br/></p><p>Note that this is still work in progress.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        timeTable_button->setText(QCoreApplication::translate("technical_data", "timeTable", nullptr));
#if QT_CONFIG(tooltip)
        framedumpButton->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>DAB+ audio services are encoded in an AAC format. </p><p>Touching this button will show a menu to store the AAC frames into a file.</p><p>As usual, touching the button again will close the output file. Programs like vlc can handle such files.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        framedumpButton->setText(QCoreApplication::translate("technical_data", "frame dump", nullptr));
#if QT_CONFIG(tooltip)
        audiodumpButton->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>The audio output can be saved as a file in uncompressed PCM format. </p><p>Touching this button will show a menu to save the PCM data into a file.</p><p>The output is a regular &quot;.wav&quot; file which can be handled by many other audio programs.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        audiodumpButton->setText(QCoreApplication::translate("technical_data", "dump audio", nullptr));
        label_3->setText(QCoreApplication::translate("technical_data", "serviceId", nullptr));
        label->setText(QCoreApplication::translate("technical_data", "Start Address of CU", nullptr));
        label_2->setText(QCoreApplication::translate("technical_data", "Used CUs", nullptr));
        label_6->setText(QCoreApplication::translate("technical_data", "Subchannel ID", nullptr));
        l6->setText(QCoreApplication::translate("technical_data", "Bitrate in kBit/s", nullptr));
#if QT_CONFIG(tooltip)
        bitrateDisplay->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>The birtae of the encoding of the AAC datastream is shown here</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_4->setText(QCoreApplication::translate("technical_data", "rs corrections/100", nullptr));
#if QT_CONFIG(tooltip)
        rsCorrections->setToolTip(QCoreApplication::translate("technical_data", "The number of corrections made by the reed-solomon algorithm over 10 DAB+ frames (so the number does not relate to the classic DAB/MP2 frames)", nullptr));
#endif // QT_CONFIG(tooltip)
        label_11->setText(QCoreApplication::translate("technical_data", "ec errors ", nullptr));
#if QT_CONFIG(tooltip)
        ecCorrections->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>RS decoding/recovery uses parity bits. If, however, a parity bit is faulty, recovery of errors may lead to introducing errors. The crc check following the recovery should reveal that.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        missedLabel->setText(QCoreApplication::translate("technical_data", "audio % OK", nullptr));
#if QT_CONFIG(tooltip)
        missedSamples->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>The audiohandler runs with its own clock. If less than the requested number of</p><p>audiosamples is passed on, the percentage = shown here - will drop accordingly.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_5->setText(QCoreApplication::translate("technical_data", "Prot. level:", nullptr));
#if QT_CONFIG(tooltip)
        uepField->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>Shows Protection Level and Type (A or B)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        uepField->setText(QCoreApplication::translate("technical_data", "uepField", nullptr));
        label_12->setText(QCoreApplication::translate("technical_data", "code rate:", nullptr));
        codeRate->setText(QCoreApplication::translate("technical_data", "TextLabel", nullptr));
        label_8->setText(QCoreApplication::translate("technical_data", "Type:", nullptr));
        ASCTy->setText(QCoreApplication::translate("technical_data", "ASCTy", nullptr));
        label_9->setText(QCoreApplication::translate("technical_data", "Language:", nullptr));
#if QT_CONFIG(tooltip)
        language->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>Language (defined by provider)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        language->setText(QCoreApplication::translate("technical_data", "language", nullptr));
#if QT_CONFIG(tooltip)
        fmLabel->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>This DAB service is on FM as well. Feature only used by some DAB services.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        fmLabel->setText(QCoreApplication::translate("technical_data", "also on FM:", nullptr));
        fmFrequency->setText(QCoreApplication::translate("technical_data", "TextLabel", nullptr));
        aacErrorLabel->setText(QCoreApplication::translate("technical_data", "AAC", nullptr));
#if QT_CONFIG(tooltip)
        aacError_display->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>Indicator of the success rate of handling the AAC frames in the DAB+ transmissions.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        frameError_display->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>Frame errors. Indication of the quality of the DAB+ frame detection. 100 is good.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        frameErrorLabel->setText(QCoreApplication::translate("technical_data", "Frame", nullptr));
#if QT_CONFIG(tooltip)
        rsError_display->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>Quality of the DAB+ frames. </p><p>Indicator for the amount of times meaning the frames contain more errors than the Reed Solomon correction can correct. </p><p>100 is good.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        rsErrorLabel->setText(QCoreApplication::translate("technical_data", "RS", nullptr));
#if QT_CONFIG(tooltip)
        motAvailable->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>Slide show indicator</p><p>Green means MOT frames are received.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        motAvailable->setText(QCoreApplication::translate("technical_data", "mot available", nullptr));
        psLabel->setText(QCoreApplication::translate("technical_data", "ps", nullptr));
        sbrLabel->setText(QCoreApplication::translate("technical_data", "sbr", nullptr));
    } // retranslateUi

};

namespace Ui {
    class technical_data: public Ui_technical_data {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TECHNICAL_DATA_H
