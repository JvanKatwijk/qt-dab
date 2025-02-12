/********************************************************************************
** Form generated from reading UI file 'sdrplay-widget-v3.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SDRPLAY_2D_WIDGET_2D_V3_H
#define UI_SDRPLAY_2D_WIDGET_2D_V3_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_sdrplayWidget_v3
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *deviceLabel;
    QLCDNumber *api_version;
    QLabel *overloadLabel;
    QLabel *serialNumber;
    QHBoxLayout *horizontalLayout_2;
    QDoubleSpinBox *ppmControl;
    QCheckBox *notch_selector;
    QLabel *gainDisplay;
    QSpacerItem *horizontalSpacer;
    QSpinBox *GRdBSelector;
    QHBoxLayout *horizontalLayout_4;
    QLabel *ppmLabel;
    QCheckBox *biasT_selector;
    QSpacerItem *horizontalSpacer_3;
    QLabel *gainsliderLabel;
    QHBoxLayout *horizontalLayout_6;
    QVBoxLayout *verticalLayout;
    QCheckBox *agcControl;
    QSpacerItem *verticalSpacer;
    QCheckBox *debugControl;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QSpinBox *lnaGainSetting;
    QLCDNumber *lnaGRdBDisplay;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout;
    QComboBox *tunerSelector;
    QPushButton *dumpButton;
    QSpacerItem *horizontalSpacer_2;
    QComboBox *antennaSelector;

    void setupUi(QWidget *sdrplayWidget_v3)
    {
        if (sdrplayWidget_v3->objectName().isEmpty())
            sdrplayWidget_v3->setObjectName("sdrplayWidget_v3");
        sdrplayWidget_v3->resize(407, 266);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(sdrplayWidget_v3->sizePolicy().hasHeightForWidth());
        sdrplayWidget_v3->setSizePolicy(sizePolicy);
        verticalLayout_3 = new QVBoxLayout(sdrplayWidget_v3);
        verticalLayout_3->setObjectName("verticalLayout_3");
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        deviceLabel = new QLabel(sdrplayWidget_v3);
        deviceLabel->setObjectName("deviceLabel");

        horizontalLayout_5->addWidget(deviceLabel);

        api_version = new QLCDNumber(sdrplayWidget_v3);
        api_version->setObjectName("api_version");
        api_version->setLineWidth(0);
        api_version->setSegmentStyle(QLCDNumber::Flat);

        horizontalLayout_5->addWidget(api_version);

        overloadLabel = new QLabel(sdrplayWidget_v3);
        overloadLabel->setObjectName("overloadLabel");

        horizontalLayout_5->addWidget(overloadLabel);


        verticalLayout_3->addLayout(horizontalLayout_5);

        serialNumber = new QLabel(sdrplayWidget_v3);
        serialNumber->setObjectName("serialNumber");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(serialNumber->sizePolicy().hasHeightForWidth());
        serialNumber->setSizePolicy(sizePolicy1);

        verticalLayout_3->addWidget(serialNumber);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        ppmControl = new QDoubleSpinBox(sdrplayWidget_v3);
        ppmControl->setObjectName("ppmControl");
        ppmControl->setMinimum(-2000.000000000000000);
        ppmControl->setMaximum(20000.000000000000000);

        horizontalLayout_2->addWidget(ppmControl);

        notch_selector = new QCheckBox(sdrplayWidget_v3);
        notch_selector->setObjectName("notch_selector");

        horizontalLayout_2->addWidget(notch_selector);

        gainDisplay = new QLabel(sdrplayWidget_v3);
        gainDisplay->setObjectName("gainDisplay");

        horizontalLayout_2->addWidget(gainDisplay);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        GRdBSelector = new QSpinBox(sdrplayWidget_v3);
        GRdBSelector->setObjectName("GRdBSelector");
        sizePolicy.setHeightForWidth(GRdBSelector->sizePolicy().hasHeightForWidth());
        GRdBSelector->setSizePolicy(sizePolicy);
        GRdBSelector->setMinimum(20);
        GRdBSelector->setMaximum(59);

        horizontalLayout_2->addWidget(GRdBSelector);


        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        ppmLabel = new QLabel(sdrplayWidget_v3);
        ppmLabel->setObjectName("ppmLabel");
        QFont font;
        font.setFamilies({QString::fromUtf8("DejaVu Sans Mono")});
        font.setBold(true);
        font.setItalic(true);
        ppmLabel->setFont(font);

        horizontalLayout_4->addWidget(ppmLabel);

        biasT_selector = new QCheckBox(sdrplayWidget_v3);
        biasT_selector->setObjectName("biasT_selector");

        horizontalLayout_4->addWidget(biasT_selector);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        gainsliderLabel = new QLabel(sdrplayWidget_v3);
        gainsliderLabel->setObjectName("gainsliderLabel");
        gainsliderLabel->setFont(font);

        horizontalLayout_4->addWidget(gainsliderLabel);


        verticalLayout_3->addLayout(horizontalLayout_4);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        agcControl = new QCheckBox(sdrplayWidget_v3);
        agcControl->setObjectName("agcControl");
        agcControl->setFont(font);

        verticalLayout->addWidget(agcControl);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        debugControl = new QCheckBox(sdrplayWidget_v3);
        debugControl->setObjectName("debugControl");

        verticalLayout->addWidget(debugControl);


        horizontalLayout_6->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        lnaGainSetting = new QSpinBox(sdrplayWidget_v3);
        lnaGainSetting->setObjectName("lnaGainSetting");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lnaGainSetting->sizePolicy().hasHeightForWidth());
        lnaGainSetting->setSizePolicy(sizePolicy2);

        horizontalLayout_3->addWidget(lnaGainSetting);

        lnaGRdBDisplay = new QLCDNumber(sdrplayWidget_v3);
        lnaGRdBDisplay->setObjectName("lnaGRdBDisplay");
        lnaGRdBDisplay->setDigitCount(3);
        lnaGRdBDisplay->setSegmentStyle(QLCDNumber::Flat);

        horizontalLayout_3->addWidget(lnaGRdBDisplay);


        verticalLayout_2->addLayout(horizontalLayout_3);

        label_2 = new QLabel(sdrplayWidget_v3);
        label_2->setObjectName("label_2");
        label_2->setFont(font);

        verticalLayout_2->addWidget(label_2);


        horizontalLayout_6->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout_6);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        tunerSelector = new QComboBox(sdrplayWidget_v3);
        tunerSelector->addItem(QString());
        tunerSelector->addItem(QString());
        tunerSelector->setObjectName("tunerSelector");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(tunerSelector->sizePolicy().hasHeightForWidth());
        tunerSelector->setSizePolicy(sizePolicy3);
        tunerSelector->setFont(font);

        horizontalLayout->addWidget(tunerSelector);

        dumpButton = new QPushButton(sdrplayWidget_v3);
        dumpButton->setObjectName("dumpButton");

        horizontalLayout->addWidget(dumpButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        antennaSelector = new QComboBox(sdrplayWidget_v3);
        antennaSelector->addItem(QString());
        antennaSelector->setObjectName("antennaSelector");
        sizePolicy3.setHeightForWidth(antennaSelector->sizePolicy().hasHeightForWidth());
        antennaSelector->setSizePolicy(sizePolicy3);
        antennaSelector->setFont(font);

        horizontalLayout->addWidget(antennaSelector);


        verticalLayout_3->addLayout(horizontalLayout);


        retranslateUi(sdrplayWidget_v3);

        QMetaObject::connectSlotsByName(sdrplayWidget_v3);
    } // setupUi

    void retranslateUi(QWidget *sdrplayWidget_v3)
    {
        sdrplayWidget_v3->setWindowTitle(QCoreApplication::translate("sdrplayWidget_v3", "SDRplay control-v3", nullptr));
        deviceLabel->setText(QCoreApplication::translate("sdrplayWidget_v3", "mirics-SDRplay", nullptr));
#if QT_CONFIG(tooltip)
        api_version->setToolTip(QCoreApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>Version of the SDRplay library detected.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        overloadLabel->setText(QString());
        serialNumber->setText(QString());
#if QT_CONFIG(tooltip)
        notch_selector->setToolTip(QCoreApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>The notch filter. The sdplay Rsp has a notch filter that - for the DAB band - blocks signals from the MW and FM band.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        notch_selector->setText(QCoreApplication::translate("sdrplayWidget_v3", "notch", nullptr));
#if QT_CONFIG(tooltip)
        gainDisplay->setToolTip(QCoreApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>The number gere displays the currently applied attenuation on the incoming signal</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        gainDisplay->setText(QCoreApplication::translate("sdrplayWidget_v3", "0 dB", nullptr));
        ppmLabel->setText(QCoreApplication::translate("sdrplayWidget_v3", "ppm control", nullptr));
        biasT_selector->setText(QCoreApplication::translate("sdrplayWidget_v3", "biasT", nullptr));
        gainsliderLabel->setText(QCoreApplication::translate("sdrplayWidget_v3", "if gain reduction", nullptr));
#if QT_CONFIG(tooltip)
        agcControl->setToolTip(QCoreApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>Touch to switch on automatic gain control - based on the setting of the lnastate and the ifgain.</p><p>If switched on, the lnastate switch and the ifgain slider are switched off</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        agcControl->setText(QCoreApplication::translate("sdrplayWidget_v3", "agc", nullptr));
        debugControl->setText(QCoreApplication::translate("sdrplayWidget_v3", "debug", nullptr));
#if QT_CONFIG(tooltip)
        lnaGainSetting->setToolTip(QCoreApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>Setting the lna state, determining the gain reduction applied in the lna.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        lnaGRdBDisplay->setToolTip(QCoreApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>The display shows the gain reduction in the lna of the device.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_2->setText(QCoreApplication::translate("sdrplayWidget_v3", "lna state selector", nullptr));
        tunerSelector->setItemText(0, QCoreApplication::translate("sdrplayWidget_v3", "Tuner 1", nullptr));
        tunerSelector->setItemText(1, QCoreApplication::translate("sdrplayWidget_v3", "Tuner 2", nullptr));

#if QT_CONFIG(tooltip)
        tunerSelector->setToolTip(QCoreApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>If the device is an rspduo, select the tuner with this combobox</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        dumpButton->setText(QCoreApplication::translate("sdrplayWidget_v3", "dump ", nullptr));
        antennaSelector->setItemText(0, QCoreApplication::translate("sdrplayWidget_v3", "Antenna A", nullptr));

#if QT_CONFIG(tooltip)
        antennaSelector->setToolTip(QCoreApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>Select an antenna (RSP-II)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class sdrplayWidget_v3: public Ui_sdrplayWidget_v3 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SDRPLAY_2D_WIDGET_2D_V3_H
