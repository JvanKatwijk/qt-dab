/********************************************************************************
** Form generated from reading UI file 'sdrplay-widget-v3.ui'
**
** Created by: Qt User Interface Compiler version 5.12.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SDRPLAY_2D_WIDGET_2D_V3_H
#define UI_SDRPLAY_2D_WIDGET_2D_V3_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
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
    QSpacerItem *horizontalSpacer_4;
    QLCDNumber *api_version;
    QLabel *serialNumber;
    QHBoxLayout *horizontalLayout_2;
    QSpinBox *ppmControl;
    QSpacerItem *horizontalSpacer;
    QSpinBox *GRdBSelector;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
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
            sdrplayWidget_v3->setObjectName(QString::fromUtf8("sdrplayWidget_v3"));
        sdrplayWidget_v3->resize(411, 272);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(sdrplayWidget_v3->sizePolicy().hasHeightForWidth());
        sdrplayWidget_v3->setSizePolicy(sizePolicy);
        verticalLayout_3 = new QVBoxLayout(sdrplayWidget_v3);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        deviceLabel = new QLabel(sdrplayWidget_v3);
        deviceLabel->setObjectName(QString::fromUtf8("deviceLabel"));

        horizontalLayout_5->addWidget(deviceLabel);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_4);

        api_version = new QLCDNumber(sdrplayWidget_v3);
        api_version->setObjectName(QString::fromUtf8("api_version"));
        api_version->setLineWidth(0);
        api_version->setSegmentStyle(QLCDNumber::Flat);

        horizontalLayout_5->addWidget(api_version);


        verticalLayout_3->addLayout(horizontalLayout_5);

        serialNumber = new QLabel(sdrplayWidget_v3);
        serialNumber->setObjectName(QString::fromUtf8("serialNumber"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(serialNumber->sizePolicy().hasHeightForWidth());
        serialNumber->setSizePolicy(sizePolicy1);

        verticalLayout_3->addWidget(serialNumber);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        ppmControl = new QSpinBox(sdrplayWidget_v3);
        ppmControl->setObjectName(QString::fromUtf8("ppmControl"));
        sizePolicy.setHeightForWidth(ppmControl->sizePolicy().hasHeightForWidth());
        ppmControl->setSizePolicy(sizePolicy);
        ppmControl->setMinimum(-200);
        ppmControl->setMaximum(200);

        horizontalLayout_2->addWidget(ppmControl);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        GRdBSelector = new QSpinBox(sdrplayWidget_v3);
        GRdBSelector->setObjectName(QString::fromUtf8("GRdBSelector"));
        sizePolicy.setHeightForWidth(GRdBSelector->sizePolicy().hasHeightForWidth());
        GRdBSelector->setSizePolicy(sizePolicy);
        GRdBSelector->setMinimum(20);
        GRdBSelector->setMaximum(59);

        horizontalLayout_2->addWidget(GRdBSelector);


        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label = new QLabel(sdrplayWidget_v3);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu Sans Mono"));
        font.setBold(true);
        font.setItalic(true);
        font.setWeight(75);
        label->setFont(font);

        horizontalLayout_4->addWidget(label);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        gainsliderLabel = new QLabel(sdrplayWidget_v3);
        gainsliderLabel->setObjectName(QString::fromUtf8("gainsliderLabel"));
        gainsliderLabel->setFont(font);

        horizontalLayout_4->addWidget(gainsliderLabel);


        verticalLayout_3->addLayout(horizontalLayout_4);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        agcControl = new QCheckBox(sdrplayWidget_v3);
        agcControl->setObjectName(QString::fromUtf8("agcControl"));
        agcControl->setFont(font);

        verticalLayout->addWidget(agcControl);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        debugControl = new QCheckBox(sdrplayWidget_v3);
        debugControl->setObjectName(QString::fromUtf8("debugControl"));

        verticalLayout->addWidget(debugControl);


        horizontalLayout_6->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        lnaGainSetting = new QSpinBox(sdrplayWidget_v3);
        lnaGainSetting->setObjectName(QString::fromUtf8("lnaGainSetting"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lnaGainSetting->sizePolicy().hasHeightForWidth());
        lnaGainSetting->setSizePolicy(sizePolicy2);

        horizontalLayout_3->addWidget(lnaGainSetting);

        lnaGRdBDisplay = new QLCDNumber(sdrplayWidget_v3);
        lnaGRdBDisplay->setObjectName(QString::fromUtf8("lnaGRdBDisplay"));
        lnaGRdBDisplay->setDigitCount(3);
        lnaGRdBDisplay->setSegmentStyle(QLCDNumber::Flat);

        horizontalLayout_3->addWidget(lnaGRdBDisplay);


        verticalLayout_2->addLayout(horizontalLayout_3);

        label_2 = new QLabel(sdrplayWidget_v3);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);

        verticalLayout_2->addWidget(label_2);


        horizontalLayout_6->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout_6);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        tunerSelector = new QComboBox(sdrplayWidget_v3);
        tunerSelector->addItem(QString());
        tunerSelector->addItem(QString());
        tunerSelector->setObjectName(QString::fromUtf8("tunerSelector"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(tunerSelector->sizePolicy().hasHeightForWidth());
        tunerSelector->setSizePolicy(sizePolicy3);
        tunerSelector->setFont(font);

        horizontalLayout->addWidget(tunerSelector);

        dumpButton = new QPushButton(sdrplayWidget_v3);
        dumpButton->setObjectName(QString::fromUtf8("dumpButton"));

        horizontalLayout->addWidget(dumpButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        antennaSelector = new QComboBox(sdrplayWidget_v3);
        antennaSelector->addItem(QString());
        antennaSelector->addItem(QString());
        antennaSelector->setObjectName(QString::fromUtf8("antennaSelector"));
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
        sdrplayWidget_v3->setWindowTitle(QApplication::translate("sdrplayWidget_v3", "SDRplay control-v3", nullptr));
        deviceLabel->setText(QApplication::translate("sdrplayWidget_v3", "mirics-SDRplay", nullptr));
#ifndef QT_NO_TOOLTIP
        api_version->setToolTip(QApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>Version of the SDRplay library detected.</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        serialNumber->setText(QString());
#ifndef QT_NO_TOOLTIP
        ppmControl->setToolTip(QApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>ppm control. Tells the device the offset (in ppm) of the observed oscillator offset.</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("sdrplayWidget_v3", "ppm control", nullptr));
        gainsliderLabel->setText(QApplication::translate("sdrplayWidget_v3", "if gain reduction", nullptr));
#ifndef QT_NO_TOOLTIP
        agcControl->setToolTip(QApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>Touch to switch on automatic gain control - based on the setting of the lnastate and the ifgain.</p><p>If switched on, the lnastate switch and the ifgain slider are switched off</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        agcControl->setText(QApplication::translate("sdrplayWidget_v3", "agc", nullptr));
        debugControl->setText(QApplication::translate("sdrplayWidget_v3", "debug", nullptr));
#ifndef QT_NO_TOOLTIP
        lnaGainSetting->setToolTip(QApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>Setting the lna state, determining the gain reduction applied in the lna.</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        lnaGRdBDisplay->setToolTip(QApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>The display shows the gain reduction in the lna of the device.</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("sdrplayWidget_v3", "lna state selector", nullptr));
        tunerSelector->setItemText(0, QApplication::translate("sdrplayWidget_v3", "Tuner 1", nullptr));
        tunerSelector->setItemText(1, QApplication::translate("sdrplayWidget_v3", "Tuner 2", nullptr));

#ifndef QT_NO_TOOLTIP
        tunerSelector->setToolTip(QApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>If the device is an rspduo, select the tuner with this combobox</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        dumpButton->setText(QApplication::translate("sdrplayWidget_v3", "dump ", nullptr));
        antennaSelector->setItemText(0, QApplication::translate("sdrplayWidget_v3", "Antenna A", nullptr));
        antennaSelector->setItemText(1, QApplication::translate("sdrplayWidget_v3", "Antenna B", nullptr));

#ifndef QT_NO_TOOLTIP
        antennaSelector->setToolTip(QApplication::translate("sdrplayWidget_v3", "<html><head/><body><p>Select an antenna (RSP-II)</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class sdrplayWidget_v3: public Ui_sdrplayWidget_v3 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SDRPLAY_2D_WIDGET_2D_V3_H
