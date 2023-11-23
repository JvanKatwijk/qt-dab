/********************************************************************************
** Form generated from reading UI file 'rtl_tcp-widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RTL_TCP_2D_WIDGET_H
#define UI_RTL_TCP_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_rtl_tcp_widget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *tcp_connect;
    QPushButton *tcp_disconnect;
    QFormLayout *formLayout;
    QSpinBox *tcp_gain;
    QLabel *label_2;
    QSpinBox *tcp_ppm;
    QLabel *label_3;
    QSpinBox *khzOffset;
    QLabel *label_4;
    QLabel *theState;
    QLabel *label;

    void setupUi(QWidget *rtl_tcp_widget)
    {
        if (rtl_tcp_widget->objectName().isEmpty())
            rtl_tcp_widget->setObjectName(QString::fromUtf8("rtl_tcp_widget"));
        rtl_tcp_widget->resize(221, 264);
        verticalLayout = new QVBoxLayout(rtl_tcp_widget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        tcp_connect = new QPushButton(rtl_tcp_widget);
        tcp_connect->setObjectName(QString::fromUtf8("tcp_connect"));

        horizontalLayout->addWidget(tcp_connect);

        tcp_disconnect = new QPushButton(rtl_tcp_widget);
        tcp_disconnect->setObjectName(QString::fromUtf8("tcp_disconnect"));

        horizontalLayout->addWidget(tcp_disconnect);


        verticalLayout->addLayout(horizontalLayout);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        tcp_gain = new QSpinBox(rtl_tcp_widget);
        tcp_gain->setObjectName(QString::fromUtf8("tcp_gain"));
        tcp_gain->setMaximum(999);

        formLayout->setWidget(0, QFormLayout::LabelRole, tcp_gain);

        label_2 = new QLabel(rtl_tcp_widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(0, QFormLayout::FieldRole, label_2);

        tcp_ppm = new QSpinBox(rtl_tcp_widget);
        tcp_ppm->setObjectName(QString::fromUtf8("tcp_ppm"));
        tcp_ppm->setMinimum(-100);

        formLayout->setWidget(1, QFormLayout::LabelRole, tcp_ppm);

        label_3 = new QLabel(rtl_tcp_widget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(1, QFormLayout::FieldRole, label_3);

        khzOffset = new QSpinBox(rtl_tcp_widget);
        khzOffset->setObjectName(QString::fromUtf8("khzOffset"));
        khzOffset->setMinimum(-100);

        formLayout->setWidget(2, QFormLayout::LabelRole, khzOffset);

        label_4 = new QLabel(rtl_tcp_widget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(2, QFormLayout::FieldRole, label_4);


        verticalLayout->addLayout(formLayout);

        theState = new QLabel(rtl_tcp_widget);
        theState->setObjectName(QString::fromUtf8("theState"));

        verticalLayout->addWidget(theState);

        label = new QLabel(rtl_tcp_widget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);


        retranslateUi(rtl_tcp_widget);

        QMetaObject::connectSlotsByName(rtl_tcp_widget);
    } // setupUi

    void retranslateUi(QWidget *rtl_tcp_widget)
    {
        rtl_tcp_widget->setWindowTitle(QCoreApplication::translate("rtl_tcp_widget", "rtl-tcp control", nullptr));
        tcp_connect->setText(QCoreApplication::translate("rtl_tcp_widget", "connect", nullptr));
        tcp_disconnect->setText(QCoreApplication::translate("rtl_tcp_widget", "disconnect", nullptr));
        label_2->setText(QCoreApplication::translate("rtl_tcp_widget", " gain", nullptr));
        label_3->setText(QCoreApplication::translate("rtl_tcp_widget", "ppm", nullptr));
        label_4->setText(QCoreApplication::translate("rtl_tcp_widget", "Offset", nullptr));
        theState->setText(QString());
        label->setText(QCoreApplication::translate("rtl_tcp_widget", "rtl_tcp_client", nullptr));
    } // retranslateUi

};

namespace Ui {
    class rtl_tcp_widget: public Ui_rtl_tcp_widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RTL_TCP_2D_WIDGET_H
