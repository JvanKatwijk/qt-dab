/********************************************************************************
** Form generated from reading UI file 'soapy-widget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SOAPY_2D_WIDGET_H
#define UI_SOAPY_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_soapyWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *deviceLabel;
    QCheckBox *agcControl;
    QLabel *serialNumber;
    QLabel *deviceNameLabel;
    QHBoxLayout *horizontalLayout_2;
    QSpinBox *spinBox_1;
    QLabel *labelSpinbox_1;
    QHBoxLayout *horizontalLayout_3;
    QSpinBox *spinBox_2;
    QLabel *labelSpinbox_2;
    QComboBox *comboBox_1;
    QLabel *statusLabel;

    void setupUi(QWidget *soapyWidget)
    {
        if (soapyWidget->objectName().isEmpty())
            soapyWidget->setObjectName(QString::fromUtf8("soapyWidget"));
        soapyWidget->resize(264, 337);
        verticalLayout = new QVBoxLayout(soapyWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        deviceLabel = new QLabel(soapyWidget);
        deviceLabel->setObjectName(QString::fromUtf8("deviceLabel"));

        horizontalLayout->addWidget(deviceLabel);

        agcControl = new QCheckBox(soapyWidget);
        agcControl->setObjectName(QString::fromUtf8("agcControl"));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu Sans Mono"));
        font.setBold(true);
        font.setItalic(true);
        font.setWeight(75);
        agcControl->setFont(font);

        horizontalLayout->addWidget(agcControl);


        verticalLayout->addLayout(horizontalLayout);

        serialNumber = new QLabel(soapyWidget);
        serialNumber->setObjectName(QString::fromUtf8("serialNumber"));

        verticalLayout->addWidget(serialNumber);

        deviceNameLabel = new QLabel(soapyWidget);
        deviceNameLabel->setObjectName(QString::fromUtf8("deviceNameLabel"));
        QFont font1;
        font1.setPointSize(13);
        font1.setBold(true);
        font1.setItalic(true);
        font1.setWeight(75);
        deviceNameLabel->setFont(font1);

        verticalLayout->addWidget(deviceNameLabel);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        spinBox_1 = new QSpinBox(soapyWidget);
        spinBox_1->setObjectName(QString::fromUtf8("spinBox_1"));

        horizontalLayout_2->addWidget(spinBox_1);

        labelSpinbox_1 = new QLabel(soapyWidget);
        labelSpinbox_1->setObjectName(QString::fromUtf8("labelSpinbox_1"));

        horizontalLayout_2->addWidget(labelSpinbox_1);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        spinBox_2 = new QSpinBox(soapyWidget);
        spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));

        horizontalLayout_3->addWidget(spinBox_2);

        labelSpinbox_2 = new QLabel(soapyWidget);
        labelSpinbox_2->setObjectName(QString::fromUtf8("labelSpinbox_2"));

        horizontalLayout_3->addWidget(labelSpinbox_2);


        verticalLayout->addLayout(horizontalLayout_3);

        comboBox_1 = new QComboBox(soapyWidget);
        comboBox_1->setObjectName(QString::fromUtf8("comboBox_1"));

        verticalLayout->addWidget(comboBox_1);

        statusLabel = new QLabel(soapyWidget);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));

        verticalLayout->addWidget(statusLabel);


        retranslateUi(soapyWidget);

        QMetaObject::connectSlotsByName(soapyWidget);
    } // setupUi

    void retranslateUi(QWidget *soapyWidget)
    {
        soapyWidget->setWindowTitle(QApplication::translate("soapyWidget", "soapy control", nullptr));
        deviceLabel->setText(QApplication::translate("soapyWidget", "soapy", nullptr));
#ifndef QT_NO_TOOLTIP
        agcControl->setToolTip(QApplication::translate("soapyWidget", "<html><head/><body><p>Touch to switch on automatic gain control - based on the setting of the lnastate and the ifgain.</p><p>If switched on, the lnastate switch and the ifgain slider are switched off</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        agcControl->setText(QApplication::translate("soapyWidget", "agc", nullptr));
        serialNumber->setText(QApplication::translate("soapyWidget", "serial number", nullptr));
        deviceNameLabel->setText(QApplication::translate("soapyWidget", "TextLabel", nullptr));
        labelSpinbox_1->setText(QApplication::translate("soapyWidget", "TextLabel", nullptr));
        labelSpinbox_2->setText(QApplication::translate("soapyWidget", "TextLabel", nullptr));
        statusLabel->setText(QApplication::translate("soapyWidget", "statusLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class soapyWidget: public Ui_soapyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SOAPY_2D_WIDGET_H
