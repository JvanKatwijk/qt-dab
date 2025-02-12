/********************************************************************************
** Form generated from reading UI file 'spyserver-widget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SPYSERVER_2D_WIDGET_H
#define UI_SPYSERVER_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_spyServer_widget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *spyServer_connect;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QSpinBox *spyServer_gain;
    QCheckBox *autogain_selector;
    QLabel *theState;
    QLabel *nameOfDevice;
    QLabel *deviceNumber;
    QLabel *label;

    void setupUi(QWidget *spyServer_widget)
    {
        if (spyServer_widget->objectName().isEmpty())
            spyServer_widget->setObjectName("spyServer_widget");
        spyServer_widget->resize(223, 311);
        verticalLayout = new QVBoxLayout(spyServer_widget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        spyServer_connect = new QPushButton(spyServer_widget);
        spyServer_connect->setObjectName("spyServer_connect");

        horizontalLayout->addWidget(spyServer_connect);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_2 = new QLabel(spyServer_widget);
        label_2->setObjectName("label_2");

        horizontalLayout_2->addWidget(label_2);

        spyServer_gain = new QSpinBox(spyServer_widget);
        spyServer_gain->setObjectName("spyServer_gain");
        spyServer_gain->setMaximum(999);

        horizontalLayout_2->addWidget(spyServer_gain);


        verticalLayout->addLayout(horizontalLayout_2);

        autogain_selector = new QCheckBox(spyServer_widget);
        autogain_selector->setObjectName("autogain_selector");

        verticalLayout->addWidget(autogain_selector);

        theState = new QLabel(spyServer_widget);
        theState->setObjectName("theState");

        verticalLayout->addWidget(theState);

        nameOfDevice = new QLabel(spyServer_widget);
        nameOfDevice->setObjectName("nameOfDevice");

        verticalLayout->addWidget(nameOfDevice);

        deviceNumber = new QLabel(spyServer_widget);
        deviceNumber->setObjectName("deviceNumber");

        verticalLayout->addWidget(deviceNumber);

        label = new QLabel(spyServer_widget);
        label->setObjectName("label");

        verticalLayout->addWidget(label);


        retranslateUi(spyServer_widget);

        QMetaObject::connectSlotsByName(spyServer_widget);
    } // setupUi

    void retranslateUi(QWidget *spyServer_widget)
    {
        spyServer_widget->setWindowTitle(QCoreApplication::translate("spyServer_widget", "spy_server", nullptr));
        spyServer_connect->setText(QCoreApplication::translate("spyServer_widget", "connect", nullptr));
        label_2->setText(QCoreApplication::translate("spyServer_widget", " gain", nullptr));
        autogain_selector->setText(QCoreApplication::translate("spyServer_widget", "autogain", nullptr));
        theState->setText(QCoreApplication::translate("spyServer_widget", "state", nullptr));
        nameOfDevice->setText(QCoreApplication::translate("spyServer_widget", "TextLabel", nullptr));
        deviceNumber->setText(QCoreApplication::translate("spyServer_widget", "TextLabel", nullptr));
        label->setText(QCoreApplication::translate("spyServer_widget", "spy-server", nullptr));
    } // retranslateUi

};

namespace Ui {
    class spyServer_widget: public Ui_spyServer_widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SPYSERVER_2D_WIDGET_H
