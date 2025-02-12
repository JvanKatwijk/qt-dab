/********************************************************************************
** Form generated from reading UI file 'spyserver-widget-8.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPYSERVER_2D_WIDGET_2D_8_H
#define UI_SPYSERVER_2D_WIDGET_2D_8_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_spyServer_widget_8
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *spyServer_connect;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QSpinBox *spyServer_gain;
    QLabel *theState;
    QLabel *nameOfDevice;
    QLabel *deviceNumber;
    QLabel *label;

    void setupUi(QWidget *spyServer_widget_8)
    {
        if (spyServer_widget_8->objectName().isEmpty())
            spyServer_widget_8->setObjectName("spyServer_widget_8");
        spyServer_widget_8->resize(224, 311);
        verticalLayout = new QVBoxLayout(spyServer_widget_8);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        spyServer_connect = new QPushButton(spyServer_widget_8);
        spyServer_connect->setObjectName("spyServer_connect");

        horizontalLayout->addWidget(spyServer_connect);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_2 = new QLabel(spyServer_widget_8);
        label_2->setObjectName("label_2");

        horizontalLayout_2->addWidget(label_2);

        spyServer_gain = new QSpinBox(spyServer_widget_8);
        spyServer_gain->setObjectName("spyServer_gain");
        spyServer_gain->setMaximum(999);

        horizontalLayout_2->addWidget(spyServer_gain);


        verticalLayout->addLayout(horizontalLayout_2);

        theState = new QLabel(spyServer_widget_8);
        theState->setObjectName("theState");

        verticalLayout->addWidget(theState);

        nameOfDevice = new QLabel(spyServer_widget_8);
        nameOfDevice->setObjectName("nameOfDevice");

        verticalLayout->addWidget(nameOfDevice);

        deviceNumber = new QLabel(spyServer_widget_8);
        deviceNumber->setObjectName("deviceNumber");

        verticalLayout->addWidget(deviceNumber);

        label = new QLabel(spyServer_widget_8);
        label->setObjectName("label");

        verticalLayout->addWidget(label);


        retranslateUi(spyServer_widget_8);

        QMetaObject::connectSlotsByName(spyServer_widget_8);
    } // setupUi

    void retranslateUi(QWidget *spyServer_widget_8)
    {
        spyServer_widget_8->setWindowTitle(QCoreApplication::translate("spyServer_widget_8", "spy_server", nullptr));
        spyServer_connect->setText(QCoreApplication::translate("spyServer_widget_8", "connect", nullptr));
        label_2->setText(QCoreApplication::translate("spyServer_widget_8", " gain", nullptr));
        theState->setText(QCoreApplication::translate("spyServer_widget_8", "state", nullptr));
        nameOfDevice->setText(QCoreApplication::translate("spyServer_widget_8", "TextLabel", nullptr));
        deviceNumber->setText(QCoreApplication::translate("spyServer_widget_8", "TextLabel", nullptr));
        label->setText(QCoreApplication::translate("spyServer_widget_8", "spy-server 8 bits", nullptr));
    } // retranslateUi

};

namespace Ui {
    class spyServer_widget_8: public Ui_spyServer_widget_8 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPYSERVER_2D_WIDGET_2D_8_H
