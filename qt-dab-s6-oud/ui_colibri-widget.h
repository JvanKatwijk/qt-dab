/********************************************************************************
** Form generated from reading UI file 'colibri-widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COLIBRI_2D_WIDGET_H
#define UI_COLIBRI_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_colibriWidget
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *deviceLabel;
    QLabel *nameLabel;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *iqSwitchButton;
    QLabel *switchLabel;
    QHBoxLayout *horizontalLayout;
    QSpinBox *gainSelector;
    QLabel *labelSpinbox_1;
    QLCDNumber *actualGain;
    QLabel *rateLabel;

    void setupUi(QWidget *colibriWidget)
    {
        if (colibriWidget->objectName().isEmpty())
            colibriWidget->setObjectName(QString::fromUtf8("colibriWidget"));
        colibriWidget->resize(257, 291);
        verticalLayout = new QVBoxLayout(colibriWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        deviceLabel = new QLabel(colibriWidget);
        deviceLabel->setObjectName(QString::fromUtf8("deviceLabel"));

        verticalLayout->addWidget(deviceLabel);

        nameLabel = new QLabel(colibriWidget);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

        verticalLayout->addWidget(nameLabel);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        iqSwitchButton = new QPushButton(colibriWidget);
        iqSwitchButton->setObjectName(QString::fromUtf8("iqSwitchButton"));

        horizontalLayout_2->addWidget(iqSwitchButton);

        switchLabel = new QLabel(colibriWidget);
        switchLabel->setObjectName(QString::fromUtf8("switchLabel"));

        horizontalLayout_2->addWidget(switchLabel);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        gainSelector = new QSpinBox(colibriWidget);
        gainSelector->setObjectName(QString::fromUtf8("gainSelector"));
        gainSelector->setMaximum(73);
        gainSelector->setValue(20);

        horizontalLayout->addWidget(gainSelector);

        labelSpinbox_1 = new QLabel(colibriWidget);
        labelSpinbox_1->setObjectName(QString::fromUtf8("labelSpinbox_1"));

        horizontalLayout->addWidget(labelSpinbox_1);

        actualGain = new QLCDNumber(colibriWidget);
        actualGain->setObjectName(QString::fromUtf8("actualGain"));
        actualGain->setFrameShape(QFrame::NoFrame);
        actualGain->setSegmentStyle(QLCDNumber::Flat);

        horizontalLayout->addWidget(actualGain);


        verticalLayout->addLayout(horizontalLayout);

        rateLabel = new QLabel(colibriWidget);
        rateLabel->setObjectName(QString::fromUtf8("rateLabel"));

        verticalLayout->addWidget(rateLabel);


        retranslateUi(colibriWidget);

        QMetaObject::connectSlotsByName(colibriWidget);
    } // setupUi

    void retranslateUi(QWidget *colibriWidget)
    {
        colibriWidget->setWindowTitle(QCoreApplication::translate("colibriWidget", "lime control", nullptr));
        deviceLabel->setText(QCoreApplication::translate("colibriWidget", "colibri handler", nullptr));
        nameLabel->setText(QCoreApplication::translate("colibriWidget", "TextLabel", nullptr));
        iqSwitchButton->setText(QCoreApplication::translate("colibriWidget", "I/Q", nullptr));
        switchLabel->setText(QCoreApplication::translate("colibriWidget", "I/Q", nullptr));
        labelSpinbox_1->setText(QCoreApplication::translate("colibriWidget", "gain", nullptr));
        rateLabel->setText(QCoreApplication::translate("colibriWidget", "2560000", nullptr));
    } // retranslateUi

};

namespace Ui {
    class colibriWidget: public Ui_colibriWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COLIBRI_2D_WIDGET_H
