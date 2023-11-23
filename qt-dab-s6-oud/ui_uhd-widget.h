/********************************************************************************
** Form generated from reading UI file 'uhd-widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UHD_2D_WIDGET_H
#define UI_UHD_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_uhdWidget
{
public:
    QFormLayout *formLayout;
    QLabel *label;
    QLabel *label_5;
    QSpinBox *externalGain;
    QLabel *label_4;
    QSpinBox *f_correction;

    void setupUi(QWidget *uhdWidget)
    {
        if (uhdWidget->objectName().isEmpty())
            uhdWidget->setObjectName(QString::fromUtf8("uhdWidget"));
        uhdWidget->resize(211, 146);
        formLayout = new QFormLayout(uhdWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(uhdWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::SpanningRole, label);

        label_5 = new QLabel(uhdWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_5);

        externalGain = new QSpinBox(uhdWidget);
        externalGain->setObjectName(QString::fromUtf8("externalGain"));
        externalGain->setMaximum(103);
        externalGain->setValue(55);

        formLayout->setWidget(1, QFormLayout::FieldRole, externalGain);

        label_4 = new QLabel(uhdWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_4);

        f_correction = new QSpinBox(uhdWidget);
        f_correction->setObjectName(QString::fromUtf8("f_correction"));
        f_correction->setMinimum(-100);
        f_correction->setMaximum(100);

        formLayout->setWidget(2, QFormLayout::FieldRole, f_correction);


        retranslateUi(uhdWidget);

        QMetaObject::connectSlotsByName(uhdWidget);
    } // setupUi

    void retranslateUi(QWidget *uhdWidget)
    {
        uhdWidget->setWindowTitle(QCoreApplication::translate("uhdWidget", "UHD URSP", nullptr));
        label->setText(QCoreApplication::translate("uhdWidget", "UHD device", nullptr));
        label_5->setText(QCoreApplication::translate("uhdWidget", "Gain", nullptr));
        label_4->setText(QCoreApplication::translate("uhdWidget", "ppm", nullptr));
#if QT_CONFIG(tooltip)
        f_correction->setToolTip(QCoreApplication::translate("uhdWidget", "(not yet implemented)", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class uhdWidget: public Ui_uhdWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UHD_2D_WIDGET_H
