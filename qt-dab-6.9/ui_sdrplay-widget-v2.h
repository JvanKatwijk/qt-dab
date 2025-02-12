/********************************************************************************
** Form generated from reading UI file 'sdrplay-widget-v2.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SDRPLAY_2D_WIDGET_2D_V2_H
#define UI_SDRPLAY_2D_WIDGET_2D_V2_H

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

class Ui_sdrplayWidget
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *deviceLabel;
    QPushButton *dumpButton;
    QSpacerItem *horizontalSpacer_4;
    QLCDNumber *api_version;
    QLabel *serialNumber;
    QHBoxLayout *horizontalLayout_2;
    QSpinBox *ppmControl;
    QSpacerItem *horizontalSpacer;
    QSpinBox *GRdBSelector;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
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
    QSpacerItem *horizontalSpacer_2;
    QComboBox *antennaSelector;

    void setupUi(QWidget *sdrplayWidget)
    {
        if (sdrplayWidget->objectName().isEmpty())
            sdrplayWidget->setObjectName("sdrplayWidget");
        sdrplayWidget->resize(353, 279);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(sdrplayWidget->sizePolicy().hasHeightForWidth());
        sdrplayWidget->setSizePolicy(sizePolicy);
        verticalLayout_3 = new QVBoxLayout(sdrplayWidget);
        verticalLayout_3->setObjectName("verticalLayout_3");
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        deviceLabel = new QLabel(sdrplayWidget);
        deviceLabel->setObjectName("deviceLabel");

        horizontalLayout_5->addWidget(deviceLabel);

        dumpButton = new QPushButton(sdrplayWidget);
        dumpButton->setObjectName("dumpButton");

        horizontalLayout_5->addWidget(dumpButton);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_4);

        api_version = new QLCDNumber(sdrplayWidget);
        api_version->setObjectName("api_version");
        api_version->setLineWidth(0);
        api_version->setSegmentStyle(QLCDNumber::Flat);

        horizontalLayout_5->addWidget(api_version);


        verticalLayout_3->addLayout(horizontalLayout_5);

        serialNumber = new QLabel(sdrplayWidget);
        serialNumber->setObjectName("serialNumber");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(serialNumber->sizePolicy().hasHeightForWidth());
        serialNumber->setSizePolicy(sizePolicy1);

        verticalLayout_3->addWidget(serialNumber);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        ppmControl = new QSpinBox(sdrplayWidget);
        ppmControl->setObjectName("ppmControl");
        sizePolicy.setHeightForWidth(ppmControl->sizePolicy().hasHeightForWidth());
        ppmControl->setSizePolicy(sizePolicy);
        ppmControl->setMinimum(-200);
        ppmControl->setMaximum(200);

        horizontalLayout_2->addWidget(ppmControl);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        GRdBSelector = new QSpinBox(sdrplayWidget);
        GRdBSelector->setObjectName("GRdBSelector");
        sizePolicy.setHeightForWidth(GRdBSelector->sizePolicy().hasHeightForWidth());
        GRdBSelector->setSizePolicy(sizePolicy);
        GRdBSelector->setMinimum(20);
        GRdBSelector->setMaximum(59);

        horizontalLayout_2->addWidget(GRdBSelector);


        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        label = new QLabel(sdrplayWidget);
        label->setObjectName("label");
        QFont font;
        font.setFamilies({QString::fromUtf8("DejaVu Sans Mono")});
        font.setBold(true);
        font.setItalic(true);
        label->setFont(font);

        horizontalLayout_4->addWidget(label);

        biasT_selector = new QCheckBox(sdrplayWidget);
        biasT_selector->setObjectName("biasT_selector");

        horizontalLayout_4->addWidget(biasT_selector);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        gainsliderLabel = new QLabel(sdrplayWidget);
        gainsliderLabel->setObjectName("gainsliderLabel");
        gainsliderLabel->setFont(font);

        horizontalLayout_4->addWidget(gainsliderLabel);


        verticalLayout_3->addLayout(horizontalLayout_4);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        agcControl = new QCheckBox(sdrplayWidget);
        agcControl->setObjectName("agcControl");
        agcControl->setFont(font);

        verticalLayout->addWidget(agcControl);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        debugControl = new QCheckBox(sdrplayWidget);
        debugControl->setObjectName("debugControl");

        verticalLayout->addWidget(debugControl);


        horizontalLayout_6->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        lnaGainSetting = new QSpinBox(sdrplayWidget);
        lnaGainSetting->setObjectName("lnaGainSetting");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lnaGainSetting->sizePolicy().hasHeightForWidth());
        lnaGainSetting->setSizePolicy(sizePolicy2);

        horizontalLayout_3->addWidget(lnaGainSetting);

        lnaGRdBDisplay = new QLCDNumber(sdrplayWidget);
        lnaGRdBDisplay->setObjectName("lnaGRdBDisplay");
        lnaGRdBDisplay->setDigitCount(3);
        lnaGRdBDisplay->setSegmentStyle(QLCDNumber::Flat);

        horizontalLayout_3->addWidget(lnaGRdBDisplay);


        verticalLayout_2->addLayout(horizontalLayout_3);

        label_2 = new QLabel(sdrplayWidget);
        label_2->setObjectName("label_2");
        label_2->setFont(font);

        verticalLayout_2->addWidget(label_2);


        horizontalLayout_6->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout_6);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        tunerSelector = new QComboBox(sdrplayWidget);
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

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        antennaSelector = new QComboBox(sdrplayWidget);
        antennaSelector->addItem(QString());
        antennaSelector->addItem(QString());
        antennaSelector->setObjectName("antennaSelector");
        sizePolicy3.setHeightForWidth(antennaSelector->sizePolicy().hasHeightForWidth());
        antennaSelector->setSizePolicy(sizePolicy3);
        antennaSelector->setFont(font);

        horizontalLayout->addWidget(antennaSelector);


        verticalLayout_3->addLayout(horizontalLayout);


        retranslateUi(sdrplayWidget);

        QMetaObject::connectSlotsByName(sdrplayWidget);
    } // setupUi

    void retranslateUi(QWidget *sdrplayWidget)
    {
        sdrplayWidget->setWindowTitle(QCoreApplication::translate("sdrplayWidget", "SDRplay control", nullptr));
        deviceLabel->setText(QCoreApplication::translate("sdrplayWidget", "mirics-SDRplay", nullptr));
#if QT_CONFIG(tooltip)
        dumpButton->setToolTip(QCoreApplication::translate("sdrplayWidget", "<html><head/><body><p>Dumps the raw input from the SDRplay into a self describing file (with header in xml format containing recorded time, container format, frequency, device name, Qt-DAB version)</p><p><br/></p><p>Opens a 'Save as ...' dialog. Press again to stop recording.</p><p><br/></p><p>Very experimental! The resulting file can be read-in by using the xml file handler (if configured)</p><p><br/></p><p>Warning: Produces large files!</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        dumpButton->setText(QCoreApplication::translate("sdrplayWidget", "dump", nullptr));
#if QT_CONFIG(tooltip)
        api_version->setToolTip(QCoreApplication::translate("sdrplayWidget", "<html><head/><body><p>Version of the SDRplay library detected.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        serialNumber->setText(QString());
#if QT_CONFIG(tooltip)
        ppmControl->setToolTip(QCoreApplication::translate("sdrplayWidget", "<html><head/><body><p>ppm control. Tells the device the offset (in ppm) of the observed oscillator offset.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label->setText(QCoreApplication::translate("sdrplayWidget", "ppm control", nullptr));
        biasT_selector->setText(QCoreApplication::translate("sdrplayWidget", "biasT", nullptr));
        gainsliderLabel->setText(QCoreApplication::translate("sdrplayWidget", "if gain reduction", nullptr));
#if QT_CONFIG(tooltip)
        agcControl->setToolTip(QCoreApplication::translate("sdrplayWidget", "<html><head/><body><p>Touch to switch on automatic gain control - based on the setting of the lnastate and the ifgain.</p><p>If switched on, the lnastate switch and the ifgain slider are switched off</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        agcControl->setText(QCoreApplication::translate("sdrplayWidget", "agc", nullptr));
        debugControl->setText(QCoreApplication::translate("sdrplayWidget", "debug", nullptr));
#if QT_CONFIG(tooltip)
        lnaGainSetting->setToolTip(QCoreApplication::translate("sdrplayWidget", "<html><head/><body><p>Setting the lna state, determining the gain reduction applied in the lna.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        lnaGRdBDisplay->setToolTip(QCoreApplication::translate("sdrplayWidget", "<html><head/><body><p>The display shows the gain reduction in the lna of the device.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_2->setText(QCoreApplication::translate("sdrplayWidget", "lna state selector", nullptr));
        tunerSelector->setItemText(0, QCoreApplication::translate("sdrplayWidget", "Tuner 1", nullptr));
        tunerSelector->setItemText(1, QCoreApplication::translate("sdrplayWidget", "Tuner 2", nullptr));

#if QT_CONFIG(tooltip)
        tunerSelector->setToolTip(QCoreApplication::translate("sdrplayWidget", "<html><head/><body><p>If the device is an rspduo, select the tuner with this combobox</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        antennaSelector->setItemText(0, QCoreApplication::translate("sdrplayWidget", "Antenna A", nullptr));
        antennaSelector->setItemText(1, QCoreApplication::translate("sdrplayWidget", "Antenna B", nullptr));

#if QT_CONFIG(tooltip)
        antennaSelector->setToolTip(QCoreApplication::translate("sdrplayWidget", "<html><head/><body><p>Select an antenna (RSP-II)</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class sdrplayWidget: public Ui_sdrplayWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SDRPLAY_2D_WIDGET_2D_V2_H
