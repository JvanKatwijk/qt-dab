/********************************************************************************
** Form generated from reading UI file 'rtl_tcp-widget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RTL_TCP_2D_WIDGET_H
#define UI_RTL_TCP_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_rtl_tcp_widget
{
public:
    QFrame *frame;
    QLabel *label;
    QLabel *state;
    QPushButton *tcp_connect;
    QPushButton *tcp_disconnect;
    QLabel *connectedLabel;
    QSpinBox *tcp_gain;
    QSpinBox *tcp_ppm;
    QLabel *label_2;
    QLabel *label_3;
    QSpinBox *khzOffset;
    QLabel *label_4;

    void setupUi(QWidget *rtl_tcp_widget)
    {
        if (rtl_tcp_widget->objectName().isEmpty())
            rtl_tcp_widget->setObjectName(QString::fromUtf8("rtl_tcp_widget"));
        rtl_tcp_widget->resize(168, 237);
        frame = new QFrame(rtl_tcp_widget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(0, 0, 161, 231));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 210, 111, 21));
        state = new QLabel(frame);
        state->setObjectName(QString::fromUtf8("state"));
        state->setGeometry(QRect(10, 160, 141, 41));
        tcp_connect = new QPushButton(frame);
        tcp_connect->setObjectName(QString::fromUtf8("tcp_connect"));
        tcp_connect->setGeometry(QRect(10, 10, 111, 21));
        tcp_disconnect = new QPushButton(frame);
        tcp_disconnect->setObjectName(QString::fromUtf8("tcp_disconnect"));
        tcp_disconnect->setGeometry(QRect(10, 40, 111, 21));
        connectedLabel = new QLabel(frame);
        connectedLabel->setObjectName(QString::fromUtf8("connectedLabel"));
        connectedLabel->setGeometry(QRect(10, 159, 131, 31));
        tcp_gain = new QSpinBox(frame);
        tcp_gain->setObjectName(QString::fromUtf8("tcp_gain"));
        tcp_gain->setGeometry(QRect(10, 70, 71, 21));
        tcp_gain->setMaximum(999);
        tcp_ppm = new QSpinBox(frame);
        tcp_ppm->setObjectName(QString::fromUtf8("tcp_ppm"));
        tcp_ppm->setGeometry(QRect(10, 100, 71, 21));
        tcp_ppm->setMinimum(-100);
        label_2 = new QLabel(frame);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(90, 70, 51, 21));
        label_3 = new QLabel(frame);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(90, 100, 61, 21));
        khzOffset = new QSpinBox(frame);
        khzOffset->setObjectName(QString::fromUtf8("khzOffset"));
        khzOffset->setGeometry(QRect(10, 130, 71, 21));
        khzOffset->setMinimum(-100);
        label_4 = new QLabel(frame);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(90, 130, 51, 21));

        retranslateUi(rtl_tcp_widget);

        QMetaObject::connectSlotsByName(rtl_tcp_widget);
    } // setupUi

    void retranslateUi(QWidget *rtl_tcp_widget)
    {
        rtl_tcp_widget->setWindowTitle(QApplication::translate("rtl_tcp_widget", "rtl-tcp control", nullptr));
        label->setText(QApplication::translate("rtl_tcp_widget", "rtl_tcp_client", nullptr));
        state->setText(QString());
        tcp_connect->setText(QApplication::translate("rtl_tcp_widget", "connect", nullptr));
        tcp_disconnect->setText(QApplication::translate("rtl_tcp_widget", "disconnect", nullptr));
        connectedLabel->setText(QString());
        label_2->setText(QApplication::translate("rtl_tcp_widget", " gain", nullptr));
        label_3->setText(QApplication::translate("rtl_tcp_widget", "ppm", nullptr));
        label_4->setText(QApplication::translate("rtl_tcp_widget", "Offset", nullptr));
    } // retranslateUi

};

namespace Ui {
    class rtl_tcp_widget: public Ui_rtl_tcp_widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RTL_TCP_2D_WIDGET_H
