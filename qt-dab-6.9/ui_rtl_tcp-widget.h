/********************************************************************************
** Form generated from reading UI file 'rtl_tcp-widget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RTL_TCP_2D_WIDGET_H
#define UI_RTL_TCP_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
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
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_4;
    QLineEdit *addressSelector;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QSpinBox *portSelector;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_1;
    QDoubleSpinBox *PpmSelector;
    QHBoxLayout *horizontalLayout_5;
    QCheckBox *biasTSelector;
    QCheckBox *agcSelector;
    QHBoxLayout *horizontalLayout_7;
    QSpinBox *gainSelector;
    QLabel *gainLabel;
    QLabel *label;
    QLabel *theState;

    void setupUi(QWidget *rtl_tcp_widget)
    {
        if (rtl_tcp_widget->objectName().isEmpty())
            rtl_tcp_widget->setObjectName("rtl_tcp_widget");
        rtl_tcp_widget->resize(214, 310);
        verticalLayout = new QVBoxLayout(rtl_tcp_widget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        tcp_connect = new QPushButton(rtl_tcp_widget);
        tcp_connect->setObjectName("tcp_connect");

        horizontalLayout->addWidget(tcp_connect);

        tcp_disconnect = new QPushButton(rtl_tcp_widget);
        tcp_disconnect->setObjectName("tcp_disconnect");

        horizontalLayout->addWidget(tcp_disconnect);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_4 = new QLabel(rtl_tcp_widget);
        label_4->setObjectName("label_4");

        horizontalLayout_2->addWidget(label_4);

        addressSelector = new QLineEdit(rtl_tcp_widget);
        addressSelector->setObjectName("addressSelector");
        addressSelector->setMinimumSize(QSize(100, 0));
        addressSelector->setText(QString::fromUtf8("127.0.0.1"));
        addressSelector->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_2->addWidget(addressSelector);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_5 = new QLabel(rtl_tcp_widget);
        label_5->setObjectName("label_5");

        horizontalLayout_3->addWidget(label_5);

        portSelector = new QSpinBox(rtl_tcp_widget);
        portSelector->setObjectName("portSelector");
        portSelector->setAlignment(Qt::AlignmentFlag::AlignCenter);
        portSelector->setMaximum(99999);
        portSelector->setValue(1234);

        horizontalLayout_3->addWidget(portSelector);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        label_1 = new QLabel(rtl_tcp_widget);
        label_1->setObjectName("label_1");

        horizontalLayout_4->addWidget(label_1);

        PpmSelector = new QDoubleSpinBox(rtl_tcp_widget);
        PpmSelector->setObjectName("PpmSelector");
        PpmSelector->setMinimum(-150.000000000000000);
        PpmSelector->setMaximum(150.000000000000000);

        horizontalLayout_4->addWidget(PpmSelector);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        biasTSelector = new QCheckBox(rtl_tcp_widget);
        biasTSelector->setObjectName("biasTSelector");

        horizontalLayout_5->addWidget(biasTSelector);


        verticalLayout->addLayout(horizontalLayout_5);

        agcSelector = new QCheckBox(rtl_tcp_widget);
        agcSelector->setObjectName("agcSelector");

        verticalLayout->addWidget(agcSelector);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        gainSelector = new QSpinBox(rtl_tcp_widget);
        gainSelector->setObjectName("gainSelector");
        gainSelector->setMaximum(999);

        horizontalLayout_7->addWidget(gainSelector);

        gainLabel = new QLabel(rtl_tcp_widget);
        gainLabel->setObjectName("gainLabel");

        horizontalLayout_7->addWidget(gainLabel);


        verticalLayout->addLayout(horizontalLayout_7);

        label = new QLabel(rtl_tcp_widget);
        label->setObjectName("label");

        verticalLayout->addWidget(label);

        theState = new QLabel(rtl_tcp_widget);
        theState->setObjectName("theState");

        verticalLayout->addWidget(theState);


        retranslateUi(rtl_tcp_widget);

        QMetaObject::connectSlotsByName(rtl_tcp_widget);
    } // setupUi

    void retranslateUi(QWidget *rtl_tcp_widget)
    {
        rtl_tcp_widget->setWindowTitle(QCoreApplication::translate("rtl_tcp_widget", "rtl-tcp control", nullptr));
        tcp_connect->setText(QCoreApplication::translate("rtl_tcp_widget", "connect", nullptr));
        tcp_disconnect->setText(QCoreApplication::translate("rtl_tcp_widget", "disconnect", nullptr));
        label_4->setText(QCoreApplication::translate("rtl_tcp_widget", "Address", nullptr));
        label_5->setText(QCoreApplication::translate("rtl_tcp_widget", "Port", nullptr));
        label_1->setText(QCoreApplication::translate("rtl_tcp_widget", "Corr.", nullptr));
#if QT_CONFIG(tooltip)
        PpmSelector->setToolTip(QCoreApplication::translate("rtl_tcp_widget", "<html><head/><body><p>ppm control. Tells the device the offset (in ppm) of the observed oscillator offset.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        PpmSelector->setSuffix(QCoreApplication::translate("rtl_tcp_widget", " ppm", nullptr));
        biasTSelector->setText(QCoreApplication::translate("rtl_tcp_widget", "biasT", nullptr));
        agcSelector->setText(QCoreApplication::translate("rtl_tcp_widget", "agc", nullptr));
        gainSelector->setSuffix(QCoreApplication::translate("rtl_tcp_widget", " dB", nullptr));
        gainLabel->setText(QCoreApplication::translate("rtl_tcp_widget", "Gain", nullptr));
        label->setText(QCoreApplication::translate("rtl_tcp_widget", "rtl_tcp_client", nullptr));
        theState->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class rtl_tcp_widget: public Ui_rtl_tcp_widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RTL_TCP_2D_WIDGET_H
