/********************************************************************************
** Form generated from reading UI file 'technical_data.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
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
#include "smallpushbutton.h"

QT_BEGIN_NAMESPACE

class Ui_technical_data
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_2;
    QLabel *programName;
    QPushButton *timeTable_button;
    QFormLayout *formLayout_2;
    QHBoxLayout *horizontalLayout_4;
    smallPushButton *framedumpButton;
    smallPushButton *audiodumpButton;
    QLabel *label_3;
    QLCDNumber *serviceIdDisplay;
    QLabel *label;
    QLCDNumber *startAddressDisplay;
    QLabel *label_2;
    QLCDNumber *lengthDisplay;
    QLabel *label_6;
    QLCDNumber *subChIdDisplay;
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
    QLabel *fmFrequency;
    QLabel *fmLabel;
    QGridLayout *gridLayout;
    QLabel *aacErrorLabel;
    QProgressBar *aacError_display;
    QProgressBar *frameError_display;
    QLabel *frameErrorLabel;
    QProgressBar *rsError_display;
    QLabel *rsErrorLabel;
    QFormLayout *formLayout;
    QLabel *label_4;
    QLCDNumber *rsCorrections;
    QLabel *label_11;
    QLCDNumber *ecCorrections;
    QLabel *missedLabel;
    QLCDNumber *missedSamples;
    QHBoxLayout *horizontalLayout;
    QLabel *stereoLabel;
    QLabel *bitRateLabel;
    QLabel *psLabel;
    QLabel *sbrLabel;
    QLCDNumber *audioRate;
    QwtPlot *audio;

    void setupUi(QWidget *technical_data)
    {
        if (technical_data->objectName().isEmpty())
            technical_data->setObjectName("technical_data");
        technical_data->resize(309, 695);
        verticalLayout = new QVBoxLayout(technical_data);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        programName = new QLabel(technical_data);
        programName->setObjectName("programName");
        QFont font;
        font.setPointSize(14);
        programName->setFont(font);

        horizontalLayout_2->addWidget(programName);


        horizontalLayout_5->addLayout(horizontalLayout_2);

        timeTable_button = new QPushButton(technical_data);
        timeTable_button->setObjectName("timeTable_button");

        horizontalLayout_5->addWidget(timeTable_button);


        verticalLayout->addLayout(horizontalLayout_5);

        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName("formLayout_2");
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        framedumpButton = new smallPushButton(technical_data);
        framedumpButton->setObjectName("framedumpButton");

        horizontalLayout_4->addWidget(framedumpButton);

        audiodumpButton = new smallPushButton(technical_data);
        audiodumpButton->setObjectName("audiodumpButton");

        horizontalLayout_4->addWidget(audiodumpButton);


        formLayout_2->setLayout(0, QFormLayout::SpanningRole, horizontalLayout_4);

        label_3 = new QLabel(technical_data);
        label_3->setObjectName("label_3");

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_3);

        serviceIdDisplay = new QLCDNumber(technical_data);
        serviceIdDisplay->setObjectName("serviceIdDisplay");
        serviceIdDisplay->setFrameShape(QFrame::Shape::NoFrame);
        serviceIdDisplay->setMode(QLCDNumber::Mode::Hex);
        serviceIdDisplay->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, serviceIdDisplay);

        label = new QLabel(technical_data);
        label->setObjectName("label");

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label);

        startAddressDisplay = new QLCDNumber(technical_data);
        startAddressDisplay->setObjectName("startAddressDisplay");
        startAddressDisplay->setFrameShape(QFrame::Shape::NoFrame);
        startAddressDisplay->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        formLayout_2->setWidget(2, QFormLayout::FieldRole, startAddressDisplay);

        label_2 = new QLabel(technical_data);
        label_2->setObjectName("label_2");

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_2);

        lengthDisplay = new QLCDNumber(technical_data);
        lengthDisplay->setObjectName("lengthDisplay");
        lengthDisplay->setFrameShape(QFrame::Shape::NoFrame);
        lengthDisplay->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        formLayout_2->setWidget(3, QFormLayout::FieldRole, lengthDisplay);

        label_6 = new QLabel(technical_data);
        label_6->setObjectName("label_6");

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_6);

        subChIdDisplay = new QLCDNumber(technical_data);
        subChIdDisplay->setObjectName("subChIdDisplay");
        subChIdDisplay->setFrameShape(QFrame::Shape::NoFrame);
        subChIdDisplay->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        formLayout_2->setWidget(4, QFormLayout::FieldRole, subChIdDisplay);


        verticalLayout->addLayout(formLayout_2);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName("gridLayout_2");
        label_5 = new QLabel(technical_data);
        label_5->setObjectName("label_5");
        label_5->setMinimumSize(QSize(0, 17));

        gridLayout_2->addWidget(label_5, 0, 0, 1, 1);

        uepField = new QLabel(technical_data);
        uepField->setObjectName("uepField");
        uepField->setMinimumSize(QSize(0, 20));
        uepField->setFrameShape(QFrame::Shape::NoFrame);

        gridLayout_2->addWidget(uepField, 0, 1, 1, 1);

        label_12 = new QLabel(technical_data);
        label_12->setObjectName("label_12");

        gridLayout_2->addWidget(label_12, 1, 0, 1, 1);

        codeRate = new QLabel(technical_data);
        codeRate->setObjectName("codeRate");

        gridLayout_2->addWidget(codeRate, 1, 1, 1, 1);

        label_8 = new QLabel(technical_data);
        label_8->setObjectName("label_8");

        gridLayout_2->addWidget(label_8, 2, 0, 1, 1);

        ASCTy = new QLabel(technical_data);
        ASCTy->setObjectName("ASCTy");
        ASCTy->setFrameShape(QFrame::Shape::NoFrame);

        gridLayout_2->addWidget(ASCTy, 2, 1, 1, 1);

        label_9 = new QLabel(technical_data);
        label_9->setObjectName("label_9");

        gridLayout_2->addWidget(label_9, 3, 0, 1, 1);

        language = new QLabel(technical_data);
        language->setObjectName("language");
        language->setFrameShape(QFrame::Shape::NoFrame);

        gridLayout_2->addWidget(language, 3, 1, 1, 1);


        verticalLayout->addLayout(gridLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        fmFrequency = new QLabel(technical_data);
        fmFrequency->setObjectName("fmFrequency");

        horizontalLayout_3->addWidget(fmFrequency);

        fmLabel = new QLabel(technical_data);
        fmLabel->setObjectName("fmLabel");

        horizontalLayout_3->addWidget(fmLabel);


        verticalLayout->addLayout(horizontalLayout_3);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        aacErrorLabel = new QLabel(technical_data);
        aacErrorLabel->setObjectName("aacErrorLabel");

        gridLayout->addWidget(aacErrorLabel, 2, 0, 1, 1);

        aacError_display = new QProgressBar(technical_data);
        aacError_display->setObjectName("aacError_display");
        aacError_display->setMinimumSize(QSize(0, 0));
        aacError_display->setMaximumSize(QSize(16777215, 20));
        aacError_display->setStyleSheet(QString::fromUtf8("QProgressBar::chunk { background: #e6e600; }"));
        aacError_display->setValue(24);

        gridLayout->addWidget(aacError_display, 2, 2, 1, 1);

        frameError_display = new QProgressBar(technical_data);
        frameError_display->setObjectName("frameError_display");
        frameError_display->setMinimumSize(QSize(0, 0));
        frameError_display->setMaximumSize(QSize(16777215, 20));
        frameError_display->setStyleSheet(QString::fromUtf8("QProgressBar::chunk { background: #e6e600; }"));
        frameError_display->setValue(24);

        gridLayout->addWidget(frameError_display, 0, 2, 1, 1);

        frameErrorLabel = new QLabel(technical_data);
        frameErrorLabel->setObjectName("frameErrorLabel");

        gridLayout->addWidget(frameErrorLabel, 0, 0, 1, 2);

        rsError_display = new QProgressBar(technical_data);
        rsError_display->setObjectName("rsError_display");
        rsError_display->setMinimumSize(QSize(0, 0));
        rsError_display->setMaximumSize(QSize(16777215, 20));
        rsError_display->setStyleSheet(QString::fromUtf8("QProgressBar::chunk { background: #e6e600; }"));
        rsError_display->setValue(24);

        gridLayout->addWidget(rsError_display, 1, 2, 1, 1);

        rsErrorLabel = new QLabel(technical_data);
        rsErrorLabel->setObjectName("rsErrorLabel");

        gridLayout->addWidget(rsErrorLabel, 1, 0, 1, 2);


        verticalLayout->addLayout(gridLayout);

        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label_4 = new QLabel(technical_data);
        label_4->setObjectName("label_4");

        formLayout->setWidget(0, QFormLayout::LabelRole, label_4);

        rsCorrections = new QLCDNumber(technical_data);
        rsCorrections->setObjectName("rsCorrections");
        rsCorrections->setFrameShape(QFrame::Shape::NoFrame);
        rsCorrections->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        formLayout->setWidget(0, QFormLayout::FieldRole, rsCorrections);

        label_11 = new QLabel(technical_data);
        label_11->setObjectName("label_11");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_11);

        ecCorrections = new QLCDNumber(technical_data);
        ecCorrections->setObjectName("ecCorrections");
        ecCorrections->setFrameShape(QFrame::Shape::NoFrame);
        ecCorrections->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        formLayout->setWidget(1, QFormLayout::FieldRole, ecCorrections);

        missedLabel = new QLabel(technical_data);
        missedLabel->setObjectName("missedLabel");

        formLayout->setWidget(2, QFormLayout::LabelRole, missedLabel);

        missedSamples = new QLCDNumber(technical_data);
        missedSamples->setObjectName("missedSamples");
        missedSamples->setFrameShape(QFrame::Shape::NoFrame);
        missedSamples->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        formLayout->setWidget(2, QFormLayout::FieldRole, missedSamples);


        verticalLayout->addLayout(formLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        stereoLabel = new QLabel(technical_data);
        stereoLabel->setObjectName("stereoLabel");

        horizontalLayout->addWidget(stereoLabel);

        bitRateLabel = new QLabel(technical_data);
        bitRateLabel->setObjectName("bitRateLabel");

        horizontalLayout->addWidget(bitRateLabel);

        psLabel = new QLabel(technical_data);
        psLabel->setObjectName("psLabel");

        horizontalLayout->addWidget(psLabel);

        sbrLabel = new QLabel(technical_data);
        sbrLabel->setObjectName("sbrLabel");

        horizontalLayout->addWidget(sbrLabel);

        audioRate = new QLCDNumber(technical_data);
        audioRate->setObjectName("audioRate");
        audioRate->setFrameShape(QFrame::Shape::NoFrame);
        audioRate->setFrameShadow(QFrame::Shadow::Plain);
        audioRate->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        horizontalLayout->addWidget(audioRate);


        verticalLayout->addLayout(horizontalLayout);

        audio = new QwtPlot(technical_data);
        audio->setObjectName("audio");
        audio->setMaximumSize(QSize(291, 151));

        verticalLayout->addWidget(audio);


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
        framedumpButton->setText(QCoreApplication::translate("technical_data", "save AAC", nullptr));
#if QT_CONFIG(tooltip)
        audiodumpButton->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>The audio output can be saved as a file in uncompressed PCM format. </p><p>Touching this button will show a menu to save the PCM data into a file.</p><p>The output is a regular &quot;.wav&quot; file which can be handled by many other audio programs.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        audiodumpButton->setText(QCoreApplication::translate("technical_data", "save WAV", nullptr));
        label_3->setText(QCoreApplication::translate("technical_data", "serviceId", nullptr));
        label->setText(QCoreApplication::translate("technical_data", "Start Address of CU", nullptr));
        label_2->setText(QCoreApplication::translate("technical_data", "Used CUs", nullptr));
        label_6->setText(QCoreApplication::translate("technical_data", "Subchannel ID", nullptr));
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
        fmFrequency->setText(QCoreApplication::translate("technical_data", "TextLabel", nullptr));
#if QT_CONFIG(tooltip)
        fmLabel->setToolTip(QCoreApplication::translate("technical_data", "<html><head/><body><p>This DAB service is on FM as well. Feature only used by some DAB services.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        fmLabel->setText(QCoreApplication::translate("technical_data", "also on FM:", nullptr));
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
        stereoLabel->setText(QCoreApplication::translate("technical_data", "stereo", nullptr));
        bitRateLabel->setText(QCoreApplication::translate("technical_data", "bitRate", nullptr));
        psLabel->setText(QCoreApplication::translate("technical_data", "ps", nullptr));
        sbrLabel->setText(QCoreApplication::translate("technical_data", "sbr", nullptr));
    } // retranslateUi

};

namespace Ui {
    class technical_data: public Ui_technical_data {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TECHNICAL_DATA_H
